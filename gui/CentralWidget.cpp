#include "ui_CentralWidget.h"
#include "CentralWidget.h"
#include "TopWindow.h"
#include "MessageBox.h"
#include "util/UiUtil.h"
#include "MySystemTray.h"
#include "console.h"
#include "dialog_setting.h"
#include "udpwork.hpp"
#include "global.h"
#include "qxtglobalshortcut.h"
#include "widgetdatacontrol.h"
#include <QMenu>
#include <QSystemTrayIcon>
#include <QDebug>
#include <QHash>
#include <QList>
#include <QButtonGroup>
#include <QMessageBox>
#include <QtGlobal>
#include <QLabel>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QDateTime>
#include <QSettings>


extern quint8 Check(QByteArray &ba);
quint8 Check(QByteArray &ba){
    int len=ba.length();
    quint8 sum=0;
    for(int i=0;i<len;i++){
        sum ^= ba[i];
    }
    qDebug("%02X",sum);
    return sum;
}
QByteArrayList modeArrayList; //切换模式命令
static qint8 mode_index=0;

/*-----------------------------------------------------------------------------|
 |                            CentralWidgetPrivate                             |
 |----------------------------------------------------------------------------*/
class CentralWidgetPrivate {
public:
    CentralWidgetPrivate(CentralWidget *owner): owner(owner) {
        groupButtons  = new QButtonGroup(owner);
        switchButtons = new QButtonGroup(owner);
        switchButtons->setExclusive(true);
    }

    CentralWidget *owner;
    QButtonGroup  *groupButtons;  // 侧边栏的分组按钮组
    QButtonGroup  *switchButtons; // 侧边栏切换界面的按钮组
    QList<QAbstractButton *> itemButtons; // 侧边栏的所有 class 为 GroupItemButton 的按钮
    QHash<QAbstractButton *, QWidget *> buttonWidgetHash; // key 是侧边栏切换界面的按钮的指针，value 是右侧 widget 的指针
};

/*-----------------------------------------------------------------------------|
 |                                CentralWidget                                |
 |----------------------------------------------------------------------------*/
CentralWidget::CentralWidget(QWidget *parent) : QWidget(parent), ui(new Ui::CentralWidget), d(new CentralWidgetPrivate(this)) ,isConnected(false){


    initializeUi();
    handleEvents();

    console = new Console;
    console->setEnabled(false);

    ui->verticalLayout->addWidget(console);
    //    serial = new QSerialPort(this);
    settings = new Dialog_Setting;
    ctrlWork =new Controller; //多线程
    ui->itemButton16->setEnabled(true);
    ui->itemButton17->setEnabled(false);
    status = new QLabel;
    clip =QGuiApplication::clipboard();
    initActionsConnections();
    //    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
    //            this, &CentralWidget::handleError);

    connect(this, &CentralWidget::sStartUdpToWork, ctrlWork, &Controller::StartUdp);
    connect(this, &CentralWidget::sCloseUdpToWork, ctrlWork, &Controller::closeUdpSocket);
    connect(this, &CentralWidget::sWrite, ctrlWork, &Controller::doWrite);//多线程写
    connect(ctrlWork, &Controller::doRead,this, &CentralWidget::readDatafromWorkTh);//多线程返回
    connect(ctrlWork, &Controller::handleResults,this, &CentralWidget::handleResults);//多线程返回
    timWaitUdpRead=new QTimer(this); // 等待命令返回
    QObject::connect(timWaitUdpRead, SIGNAL(timeout()), this, SLOT(update()));
    QStringList list;
    list<<"B1C"
       <<"B1P"
      <<"B3C"
     <<"B3P"
    <<"B1C+L1"
    <<"B3C+L1";
    ui->comboBox->clear();
    ui->comboBox->addItems(list);

    modeArrayList<<"$CCMSS,Z,2,B1,C,,,,*15"         //"B1C"
                <<"$CCMSS,Z,2,B1,P,,,,*06"         //"B1P"
               <<"$CCMSS,Z,2,B3,C,,,,*17"         //"B3C"
              <<"$CCMSS,Z,2,B3,P,,,,*04"         //"B3P"
             <<"$CCMSS,Z,2,B1,C,L1,C,,*2B"      //"B1C+L1"
            <<"$CCMSS,Z,2,B3,C,L1,C,,*29";     //"B3C+L1";

    // TODO: 显示第一个按钮对应的 widget，这里只是为了演示
    ui->groupButton2->click();
    ui->itemButton15->click();
#if 0
    //    全局快捷键
    QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(this);

    if(shortcut->setShortcut(QKeySequence("Shift+1")))
    {

        connect(shortcut, &QxtGlobalShortcut::activated,
                [=]() {qDebug() << "shortcut activated";});

    }
    else
    {
        qDebug()<<"快捷键已占用";
    }
#endif
}

CentralWidget::~CentralWidget() {
    delete settings;
    delete ui;
    delete d;
}

/**
 * 初始化界面
 */
void CentralWidget::initializeUi() {
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);

    // 去掉窗口和侧边栏的 padding 和 margin
    UiUtil::setWidgetPaddingAndSpacing(this, 0, 0);
    UiUtil::setWidgetPaddingAndSpacing(ui->sideBarWidget, 0, 0);

    // [可选] 启用加载样式的快捷键 Ctrl + L，方便调试，修改样式文件后按下快捷键即可加载，不需要重启程序
    UiUtil::installLoadQssShortcut(this);

    //    MyTrayIcon(); //托盘
    // 搜集处理侧边栏的按钮
    // 1. 属性 class 为 GroupButton 的按钮放入 d->groupButtons，用来切换隐藏和显示 GroupItemButton
    // 2. 属性 class 为 GroupItemButton 的按钮都放入 d->itemButtons，
    //    如果它的 action 属性不为 popup 则把它添加到一个 QButtonGroup d->switchButtons 中，它们的作用是用来切换界面的
    // 3. 并把 QPushButton 设置为 flat 的效果
    QObjectList children = ui->sideBarWidget->children();
    for (QObject *child : children) {
        QAbstractButton *button = qobject_cast<QAbstractButton*>(child); // 可能是 QPushButton，也可能是 QToolButton
        QString className = child->property("class").toString();
        QString action    = child->property("action").toString();

        if (NULL == button) { continue; }

        if ("GroupButton" == className) {
            // 分组的按钮放到一个组里
            d->groupButtons->addButton(button);
        } else if ("GroupItemButton" == className) {
            d->itemButtons.append(button);
            button->hide();

            //            切换界面的按钮放到一个组里
            if ("popup" != action) {
                button->setCheckable(true);
                d->switchButtons->addButton(button);
            }
            //其他的自己绑定槽
        }

        // 把 QPushButton 设置为 flat 的效果，这样 QSS 的效果更好
        QPushButton *pushButton = qobject_cast<QPushButton *>(button);
        if (NULL != pushButton) {
            pushButton->setFlat(true);
        }
    }
}

/**
 * 信号槽事件处理
 */
void CentralWidget::handleEvents() {
    // 点击侧边栏的分组按钮，隐藏其他分组的按钮，显示当前分组的按钮

    connect(d->groupButtons, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), [this] (QAbstractButton *button) {
        for (QAbstractButton *itemButton : d->itemButtons) {
            QString groupName = button->property("groupName").toString();
            QString itemGroupName = itemButton->property("groupName").toString();

            if (itemGroupName == groupName) {
                itemButton->show();
            } else {
                itemButton->hide();
            }
        }
    });

    // 点击侧边栏切换界面的按钮，切换 widget
    // 1. 如果按钮对应的 widget 不存在则创建它
    // 2. 如果存在则显示出来
    connect(d->switchButtons, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), [this] (QAbstractButton *button) {
        // [1] 创建
        if (!d->buttonWidgetHash.contains(button)) {
            createWidgetInContentStackedWidget(button);
        }

        // [2] 显示
        QWidget *targetWidget = d->buttonWidgetHash.value(button);
        UiUtil::setCurrentWidgetOfStackedWidget(targetWidget, ui->contentStackedWidget);
    });

    // TODO: 普通按钮的事件处理，这里只是为了演示
    //    connect(ui->itemButton8, &QPushButton::clicked, [this] {
    //        QMessageBox::aboutQt(this);
    //    });

    // TODO: 使用自定义无边框窗口显示弹出对话框，这里只是为了演示
    //    connect(ui->itemButton7, &QPushButton::clicked, [] {
    //        MessageBox::message("<b>公司</b>: 花果山再来一瓶科技信息技术有限公司<br>"
    //                            "<b>法人</b>: 齐天大圣<br>"
    //                            "<b>版本</b>: Release 1.1.3<br>"
    //                            "<center><img src='image/common/fairy.png' width=64 height=64></center>", 350, 140);
    //    });

}

/**
 * 创建需要在内容区 stacked widget 中显示的 widget
 * @param button 侧边栏切换界面的按钮
 */
void CentralWidget::createWidgetInContentStackedWidget(QAbstractButton *button) {
    // TODO: 创建 widget，需要根据实际的 widget 类来创建

    //        QWidget *w = new Form_PRD();
    //        d->buttonWidgetHash.insert(ui->itemButton9, w);
    //        w->setStyleSheet("background: #2d8cf0");
    //        UiUtil::addWidgetIntoStackedWidget(w, ui->contentStackedWidget);


    if (button == ui->itemButton15) {
        d->buttonWidgetHash.insert(ui->itemButton15, (QWidget *)settings);
        settings->resize(400,300); //TODO
        UiUtil::addWidgetIntoStackedWidget(settings, ui->contentStackedWidget,false,false,false,false);
    }

}


void CentralWidget::MyTrayIcon()               //实现托盘功能
{

    QString title = tr("M音乐");
    QString text = tr("给你不一样的音乐体验");

    mytrayIcon = new SystemTray(this);

    mytrayIcon->show();
    mytrayIcon->showMessage(title,text,QSystemTrayIcon::Information,4000);

    connect(mytrayIcon,SIGNAL(no_vol_signal()),this,SLOT(setSilent()));
    connect(mytrayIcon,SIGNAL(play_signal()),this,SLOT(on_playButton_clicked()));
    connect(mytrayIcon,SIGNAL(forward_signal()),this,SLOT(on_nextButton_clicked()));
    connect(mytrayIcon,SIGNAL(backward_signal()),this,SLOT(on_lastButton_clicked()));
    connect(mytrayIcon,SIGNAL(quit_signal()),this,SLOT(myclose()));
    connect(mytrayIcon,SIGNAL(changevol_signal(qreal)),this,SLOT(traytochangvol(qreal)));
    connect(mytrayIcon,SIGNAL(addfile_signal()),this,SLOT(addfile()));
    connect(mytrayIcon,SIGNAL(clearlist_signal()),this,SLOT(traytoclearlist_slot()));
    connect(mytrayIcon,SIGNAL(show_action_signal()),this,SLOT(showThisWindow()));
    connect(mytrayIcon,SIGNAL(show_lrc_signal()),this,SLOT(SetLrcShown()));
    //托盘点击响应
    connect(mytrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));


}
void CentralWidget::iconIsActived(QSystemTrayIcon::ActivationReason reason)    //托盘点击响应事件
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:          //单击
    case QSystemTrayIcon::DoubleClick:      //双击
        this->showNormal();
        this->raise();
        break;
    default:
        break;
    }
}













void CentralWidget::openUdpSocket()
{

    //    if(isConnected)
    g_socket_setting = settings->settings();

    emit sCloseUdpToWork();
    emit sStartUdpToWork();
    ui->itemButton16->setEnabled(false);
    ui->itemButton17->setEnabled(true);

}

void CentralWidget::closeUdpSocket()
{
    //    if(isConnected)
    emit sCloseUdpToWork();
    console->setEnabled(false);
    ui->itemButton16->setEnabled(true);
    ui->itemButton17->setEnabled(false);
    //    showStatusMessage(tr("Disconnected"));
}

void CentralWidget::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

void CentralWidget::writeData(const QByteArray &data)
{
    emit sWrite(data);
}



void CentralWidget::initActionsConnections()
{
    //    connect(ui->itemButton16, &QAction::triggered, this, &CentralWidget::openUdpSocket);
    //    connect(ui->itemButton17, &QAction::triggered, this, &CentralWidget::closeUdpSocket);
    //    connect(ui->actionQuit, &QAction::triggered, this, &CentralWidget::close);
    //    connect(ui->actionConfigure, &QAction::triggered, settings, &CentralWidget::show);
    //    connect(ui->actionClear, &QAction::triggered, console, &Console::clear);
    //    connect(ui->actionAbout, &QAction::triggered, this, &CentralWidget::about);
    //    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void CentralWidget::showStatusMessage(const QString &message)
{

    Q_UNUSED(message);
    //    status->setText(message);
}










void  CentralWidget::UdpReadDecode(){

}

//其他信息返回
void CentralWidget::handleResults(const QByteArray &data)
{
    //    Q_UNUSED(data);
    //    qDebug()<<"====="<<data;
    if(data =="udpsocket bind success"){
        qDebug()<<"  >>      udpsocket bind success";
        isConnected =true;
    }else     if(data =="udpsocket bind fail"){
        qDebug()<<"udpsocket bind fail";
        isConnected =false;
    }
}
/// 读取串口数据报文
void CentralWidget::readDatafromWorkTh(const QByteArray &data)
{
#if 0

#else
    Q_UNUSED(data);

    //添加到文本框
    console->putData(data.toStdString().c_str());
#endif


}

void CentralWidget::autoDo()
{
    if(isConnected){
        timWaitUdpRead->stop();
        timWaitUdpRead->start(3000);
    }else{
        QString curTim=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss 串口未打开\n");
        console->putData(curTim.toStdString().c_str());
        QMessageBox message(QMessageBox::NoIcon,("提示"), "串口未打开", QMessageBox::Yes , NULL);
        message.exec();
    }
}

void CentralWidget::save()
{
    //    QSettings settings("GPS.ini", QSettings::IniFormat);
    //    WidgetDataControl::save(&settings, ui->page_2, WidgetDataControl::controlFlagsAll,"");
}

bool CentralWidget::userReallyWantsToQuit()
{
    QMessageBox message(QMessageBox::NoIcon, tr("关闭"), tr("关闭会使软件退出."), QMessageBox::Yes | QMessageBox::No, NULL);
    if(message.exec() == QMessageBox::Yes)
    {

        QMessageBox message(QMessageBox::NoIcon,tr( "保存"), tr("是否保存配置？"), QMessageBox::Yes | QMessageBox::No, nullptr);
        if(message.exec() == QMessageBox::Yes)
        {
            save();
        }
        return true;
    }else{
        return false;
    }

}
void CentralWidget::closeEvent(QCloseEvent *event)
{
    save();
    event->accept();
}


void CentralWidget::update()
{
    if(sender()==timWaitUdpRead){
    }
}

void CentralWidget::on_pushButton_clicked()
{
    QByteArray data= modeArrayList.at(mode_index);
    //    data.append("\r\n"); //行尾是否添加、\r\n
    if(isConnected){
        emit sWrite(data);  //写数据
        console->putData(data+"\r\n");
        console->putData(data.toHex()+"\r\n"); //QByteArray::toHex() 需要自己添加占位 QByteArray::toHex(‘ ’)在5.7内未实现
    }else{
        console->putData("端口未绑定\r\n");

#if 0
        QMessageBox message(QMessageBox::NoIcon,("提示"), "端口未绑定", QMessageBox::Yes , NULL);
        message.exec();
#endif

    }
}

void CentralWidget::on_comboBox_currentIndexChanged(int index)
{
    mode_index =index;
}

void CentralWidget::on_itemButton16_clicked()
{
    this->openUdpSocket();
//    isConnected =false;
}

void CentralWidget::on_itemButton17_clicked()
{
    this->closeUdpSocket();
    isConnected =false;
}
