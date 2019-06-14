#ifndef WORK_H
#define WORK_H
#include <QProcess>
#include <QThread>
#include <QDebug>
//多线程使用 moveToThread
class Worker : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const QString &parameter) {
        Q_UNUSED(parameter);
        QString result;
        /* ... here is the expensive or blocking operation ... */
        emit resultReady(result);
    }
    void doPingWork(const QString &ip) {
        QString result;
        QProcess builder;
        builder.setProcessChannelMode(QProcess::MergedChannels);
        builder.start("ping", QStringList() << ip <<" -n 1");
        //        builder.startDetached("ping", QStringList() << ip);
        if (!builder.waitForFinished()){
            result=builder.errorString();
        }
        else{

            QByteArray out=builder.readAllStandardOutput();
            if(out.indexOf("TTL")!=-1){
                result="PingPass";
            }else{
                result="PingTimeOut";
            }
        }
        emit resultReady(result);
    }

signals:
    void resultReady(const QString &result);
};

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller() {
        Worker *worker = new Worker;
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        connect(this, &Controller::operate, worker, &Worker::doPingWork);
        connect(worker, &Worker::resultReady, this, &Controller::handleResults);
        workerThread.start();
    }
    ~Controller() {
        workerThread.quit();
        workerThread.wait();
    }
public slots:
    void CallOperate(const QString & CMD){
        emit operate(CMD);
    }
signals:
    void operate(const QString &);
    void handleResults(const QString &);
};

#endif // WORK_H
