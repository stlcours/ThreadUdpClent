#ifndef WORK_H
#define WORK_H
#include <QProcess>
#include <QThread>
#include <QDebug>
#include <QtNetwork>
#include <QUdpSocket>
#include "gui/dialog_setting.h"
#include "global.h"


class udpsocketPortWorker : public QObject
{
    Q_OBJECT

public slots:
    void doWrite(const QByteArray &writeData) {
        qDebug("%s", __func__);
        qint64 size =  udpsocket.writeDatagram(writeData,QHostAddress("127.0.0.1"),p.remotePort);
//        qint64 size =  udpsocket.writeDatagram(writeData,QHostAddress(p.remoteIp),p.remotePort);
        qDebug()<<size;

    }
    void readyReadSlot()
    {
        qDebug("%s", __func__);
        QByteArray Buffer;
        Buffer.resize(udpsocket.pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpsocket.readDatagram(Buffer.data(), Buffer.size(), &sender, &senderPort);
#if 1
        qDebug() << "Message from:" << sender.toString();
        qDebug() << "Message port:" << senderPort;
        qDebug() << "Message: " << Buffer;
        qDebug() << "Message.toHex: " << Buffer.toHex();
        qDebug() << "Message.size: " << Buffer.size();
#endif
        if(Buffer.size()){
            emit doRead(Buffer);
        }
    }
    void StartUdp()
    {
        p= g_socket_setting;
        bool ok=  udpsocket.bind(QHostAddress(p.localIp),p.listnerPort);
        connect(&udpsocket,&QUdpSocket::readyRead,this,&udpsocketPortWorker::readyReadSlot);
        if(ok)
        {
            qDebug()<<"udpsocket->open success";
            QByteArray    Results ="udpsocket bind success";
            emit  resultReady(Results);
        }else{
            QByteArray    Results ="udpsocket bind fail";
            qDebug()<<"udpsocket->open fail";
            emit  resultReady(Results);
        }
    }
    void closeUdpSocket()
    {
        qDebug("%s", __func__);
        udpsocket.close();
    }

signals:
    void doRead(const QByteArray &result);      //读取的数据,若解析时需要修改数据，先进行数据拷贝
    void resultReady(const QByteArray &result); //返回需要执行的结果，或者其他错误信息
private:
    QUdpSocket udpsocket;
    UdpSocketSettings p;
};

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller() {
        udpsocketPortWorker *worker = new udpsocketPortWorker;
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);

        connect(this, &Controller::StartUdp, worker, &udpsocketPortWorker::StartUdp); //open
        connect(this, &Controller::closeUdpSocket, worker, &udpsocketPortWorker::closeUdpSocket); //close
        connect(this, &Controller::doWrite, worker, &udpsocketPortWorker::doWrite); //多线程写
        connect(worker, &udpsocketPortWorker::doRead, this, &Controller::doRead);//多线程返回

        connect(worker, &udpsocketPortWorker::resultReady, this, &Controller::handleResults);//多线程返回
        workerThread.start();
    }
    ~Controller() {
        workerThread.quit();
        workerThread.wait();
    }
public slots:

signals:
    void doWrite(const QByteArray &);
    void doRead(const QByteArray &);
    void handleResults(const QByteArray &);
    void StartUdp();
    void closeUdpSocket();
};

#endif // WORK_H
