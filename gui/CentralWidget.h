#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QtCore/QtGlobal>
#include <QClipboard>
#include <QMainWindow>
#include <QMutex>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QCryptographicHash>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QWidget>


QT_BEGIN_NAMESPACE
class QLabel;
class Console;
class Dialog_Setting;
class QAbstractButton;
class CentralWidgetPrivate;
class QMenu;
class SystemTray;
namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE







namespace Ui {
class CentralWidget;
}

class CentralWidget : public QWidget {
    Q_OBJECT

public:
    explicit CentralWidget(QWidget *parent = 0);
    ~CentralWidget();


private slots:
    void MyTrayIcon();
    void iconIsActived(QSystemTrayIcon::ActivationReason reason);
private:
    void initializeUi(); // 初始化界面
    void handleEvents(); // 信号槽事件处理
    void createWidgetInContentStackedWidget(QAbstractButton *button); // 创建需要在内容区 stacked widget 中显示的 widget



public slots:
    void update();
private slots:
    void openUdpSocket();
    void closeUdpSocket();
    void about();
    void writeData(const QByteArray &datavoid );

//    void handleError(QSerialPort::SerialPortError error);
    void UdpReadDecode();
    void readDatafromWorkTh(const QByteArray &data);
    void handleResults(const QByteArray &data);
    void save();
    bool userReallyWantsToQuit();
    void on_pushButton_clicked();
    void on_comboBox_currentIndexChanged(int index);
    void on_itemButton16_clicked();
    void on_itemButton17_clicked();

signals:

    void sWrite(const QByteArray &message);
    void sStartUdpToWork();
    void sCloseUdpToWork();

protected:
    void closeEvent(QCloseEvent *event);
protected slots:
    void autoDo();
private:
    void initActionsConnections();
    void showStatusMessage(const QString &message);

private:
    Ui::CentralWidget *ui;
    CentralWidgetPrivate *d;
    QSystemTrayIcon *trayIcon;          //系统托盘
    QAction *quitAction;                //系统托盘退出项
    QMenu   *trayIconMenu;              //系统托盘菜单
    QMenu   *rightmenu;                 //右键菜单
    SystemTray *mytrayIcon;
    QLabel *status;
    Console *console;
    Dialog_Setting *settings;

    QSerialPort *serial;
    void readPendingDatagrams();
    QByteArray reData;          //多线程控制操作
    class Controller *ctrlWork; //多线程控制操作
    QClipboard *clip =nullptr;  //剪切板操作
    QMutex mutex;
    QTimer *timWaitUdpRead = nullptr;

    bool isConnected;


signals:
    //输出
    void show_GGA(const QByteArray &data);//描述定位数据
};

#endif // CENTRALWIDGET_H
