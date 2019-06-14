#include "ui_TopWindow.h"
#include "TopWindow.h"
#include "util/UiUtil.h"
#include "util/NinePatchPainter.h"
//#include <QSimpleUpdater.h>
#include <QDebug>
#include <QSizeGrip>
#include <QPainter>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QMovie>
#include <QMenu>

class TopWindowPrivate {
public:
    TopWindowPrivate(const QMargins &windowPaddings,
                     const QMargins &borderImageBorders,
                     const QString  &borderImagePath,
                     bool  borderImageHorizontalStretch,
                     bool  borderImageVerticalStretch) {
        ninePatchPainter = new NinePatchPainter(QPixmap(borderImagePath),
                                                borderImageBorders.left(),
                                                borderImageBorders.top(),
                                                borderImageBorders.right(),
                                                borderImageBorders.bottom(),
                                                borderImageHorizontalStretch,
                                                borderImageVerticalStretch);
        sizeGrip = new QSizeGrip(nullptr);
        padding  = windowPaddings;
    }

    NinePatchPainter *ninePatchPainter; // 九宫格绘图工具类
    QPoint windowPositionBeforeMoving;  // 移动窗口前窗口左上角的坐标
    QPoint mousePressedPosition;        // 按下鼠标时鼠标的全局坐标
    QSizeGrip *sizeGrip;         // 修改窗口大小的 handle
    QMargins   padding;          // 窗口的 padding
    bool       resizable = true; // 默认是可以缩放窗口的
};

TopWindow::TopWindow(QWidget *centralWidget,
                     const QMargins &windowPaddings,
                     const QMargins &borderImageBorders,
                     const QString  &borderImagePath,
                     bool  borderImageHorizontalStretch,
                     bool  borderImageVerticalStretch) : ui(new Ui::TopWindow) {
    ui->setupUi(this);
    ui->restoreButton->hide();
    ui->titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); //设置标题伸展
    initData();   //菜单
    // FramelessWindowHint属性设置窗口去除边框;
    // WindowMinimizeButtonHint 属性设置在窗口最小化时，点击任务栏窗口可以显示出原窗口;
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_TranslucentBackground);	// 设置窗口背景透明;
    setWindowTitle(centralWidget->windowTitle());// 默认使用 centralWidget 的标题


    d = new TopWindowPrivate(windowPaddings, borderImageBorders, borderImagePath,
                             borderImageHorizontalStretch, borderImageVerticalStretch);

    // 去掉 padding 和 spacing
    QGridLayout *l = qobject_cast<QGridLayout*>(layout());
    l->setSpacing(0);
    l->setContentsMargins(d->padding);

    // 替换 central widget
    centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    delete l->replaceWidget(ui->centralWidget, centralWidget);
    delete ui->centralWidget;
    // 添加 size grip 到窗口右下角
    l->addWidget(d->sizeGrip, 1, 0, Qt::AlignRight | Qt::AlignBottom);

    MyTopWindowStyle();





    //    //背景GIG图;
    //    QLabel* pBack = new QLabel(this);
    //    QMovie *movie = new QMovie();
    //    movie->setFileName(":/images/back.gif");
    //    movie->start();
    //    movie->stop();
    //    pBack->setMovie(movie);
    //    movie->start();
    //    pBack->move(0, 0);




    // 信号槽事件处理
    handleEvents();
}

TopWindow::~TopWindow() {
    delete ui;
    delete d;
}

void TopWindow::setTitle(const QString &title) {
    ui->titleLabel->setText(title);
}

void TopWindow::setTitleBar(QWidget *titleBar) {
    ui->titleBar->hide();
    delete layout()->replaceWidget(ui->titleBar, titleBar);
}

void TopWindow::setTitleBarVisible(bool visible) {
    ui->titleBar->setVisible(visible);
}

void TopWindow::setTitleBarButtonsVisible(bool min, bool max, bool close) {
    ui->minButton->setVisible(min);
    ui->maxButton->setVisible(max);
    ui->closeButton->setVisible(close);
}

void TopWindow::setResizable(bool resizable) {
    d->resizable = resizable;
    d->sizeGrip->setVisible(resizable);
}

void TopWindow::showMaximized() {
    layout()->setContentsMargins(0, 0, 0, 0); // 最大化窗口时不需要阴影，所以去掉窗口的 padding
    ui->maxButton->hide();
    ui->restoreButton->show();
    d->sizeGrip->setVisible(false);

    QWidget::showMaximized();
}

void TopWindow::showNormal() {
    layout()->setContentsMargins(d->padding); // 恢复窗口大小时显示阴影，所以加上窗口的 padding
    ui->maxButton->show();
    ui->restoreButton->hide();
    d->sizeGrip->setVisible(d->resizable);

    QWidget::showNormal();
}

void TopWindow::showModal() {
    // 作为 Dialog 需要同时设置 Qt::Dialog | Qt::Popup 两个 flags
    setWindowFlags(Qt::Dialog | Qt::Popup | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    UiUtil::centerWindow(this);
    show();

    // 进入局部事件循环，阻塞代码继续往下走，窗口关闭时结束此局部事件循环，控制权交还给 QApplication
    // The event loop returns from the call to quit().
    QEventLoop loop;
    connect(this, &TopWindow::aboutToClose, &loop, &QEventLoop::quit);
    loop.exec();
}

// 使用九宫格的方式绘制背景
void TopWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    d->ninePatchPainter->paint(&painter, rect());

#if 0
    ///背景加载;
    QPixmap m_pixmapBg;
    m_pixmapBg.load(":/res/image/frame.png");
    painter.drawPixmap(m_pixmapBg.rect(), m_pixmapBg);
#endif
    //重写paintEvent，用在主窗体边框圆角
    //    QPainter painter(this);
    //    painter.setPen(Qt::black);  //边框色
    //    painter.drawRoundedRect(this->rect(), 5, 5); //圆角5像素
}

// 鼠标按下时记录此时鼠标的全局坐标和窗口左上角的坐标
void TopWindow::mousePressEvent(QMouseEvent *event) {
    d->mousePressedPosition       = event->globalPos();
    d->windowPositionBeforeMoving = frameGeometry().topLeft();
}
// 鼠标放开时设置 mousePressedPosition 为空坐标
void TopWindow::mouseReleaseEvent(QMouseEvent *) {
    d->mousePressedPosition = QPoint();
}

// 鼠标移动时如果 mousePressedPosition 不为空，则说明需要移动窗口
// 鼠标移动的位移差，就是窗口移动的位移差
void TopWindow::mouseMoveEvent(QMouseEvent *event) {
    // 最大化时不可以移动窗口
    if (isMaximized()) { return; }

    if (!d->mousePressedPosition.isNull()) {
        QPoint delta       = event->globalPos() - d->mousePressedPosition;
        QPoint newPosition = d->windowPositionBeforeMoving + delta;
        move(newPosition);
    }
}

// 关闭窗口时发送 aboutClose() 信号，模态对话框收到此信号时好结束事件循环
void TopWindow::closeEvent(QCloseEvent *event) {
    emit aboutToClose();
    QWidget::closeEvent(event);
}
//双击放大，缩小
void TopWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(ui->maxButton->isVisible()){
        showMaximized(); // 最大化
    }
    else if(ui->restoreButton->isVisible()){
        showNormal(); // 恢复窗口大小
    }
    return QWidget::mouseDoubleClickEvent(event);
}

void TopWindow::handleEvents() {
    connect(ui->minButton, &QPushButton::clicked, [this] {
        showMinimized(); // 最小化
    });

    connect(ui->maxButton, &QPushButton::clicked, [this] {
        showMaximized(); // 最大化
    });

    connect(ui->restoreButton, &QPushButton::clicked, [this] {
        showNormal(); // 恢复窗口大小
    });

    connect(ui->closeButton, &QPushButton::clicked, [this] {
        //        close(); // 关闭窗口
        myclose();
    });

    connect(ui->menuBtn, &QPushButton::clicked, [this] {
        menuShow();
    });
}
// 该方法主要是让标题栏中的标题显示为滚动的效果;
void TopWindow::onRollTitle()
{
    static int nPos = 0;
    QString titleContent = this->windowTitle();
    // 当截取的位置比字符串长时，从头开始;
    if (nPos > titleContent.length())
        nPos = 0;
    ui->titleLabel->setText(titleContent.mid(nPos));
    nPos++;
}

// 一般情况下标题栏中的标题内容是不滚动的，但是既然自定义就看自己需要嘛，想怎么设计就怎么搞O(∩_∩)O！
void TopWindow::setTitleRoll()
{
    connect(&m_titleRollTimer, SIGNAL(timeout()), this, SLOT(onRollTitle()));
    m_titleRollTimer.start(200);
}
// 设置标题栏图标;
void TopWindow::setTitleIcon(QString filePath)
{
    QPixmap titleIcon(filePath);
    ui->IcoLabel->setPixmap(titleIcon.scaled(25 , 25));
}

void TopWindow::myclose()
{
    //界面淡出
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    connect(animation, SIGNAL(finished()), this, SLOT(close()));
}


void TopWindow::onlineUpdater(){
#if 0
    static  QString url = "https://github.com/alex-spataru/QSimpleUpdater/blob/master/tutorial/definitions/updates.json";
    QSimpleUpdater::getInstance()->setNotifyOnUpdate (url, true);
    QSimpleUpdater::getInstance()->setNotifyOnFinish (url, false);
    QSimpleUpdater::getInstance()->checkForUpdates (url);
    QSimpleUpdater::getInstance()->setDownloaderEnabled (url, true);
#endif
}

void TopWindow::MyTopWindowStyle(){

#if 0
    //设置初始背景颜色，深蓝色
    QPalette p;
    p.setBrush(this->backgroundRole(),QBrush(QColor(0,0,127,255)));
    this->setPalette(p);
    this->setAutoFillBackground(true);
#endif

    //界面渐变出现
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}

bool TopWindow::eventFilter(QObject *obj, QEvent *e)
{
#if 0
    if (obj == ui->widgetTitle)
    {
        if(e->type() == QEvent::MouseButtonDblClick)
        {
            return true;
        }
    }
#endif
    return QObject::eventFilter(obj, e);
}
void TopWindow::initData()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    m_menu = new QMenu();
    m_menu->addAction(tr("设置"));
    m_menu->addAction(tr("新版特性"));
    m_menu->addAction(tr("检查更新"));
    m_menu->addSeparator();
    m_menu->addAction(tr("在线帮助"));
    m_menu->addAction(tr("论坛求助"));
    m_menu->addAction(tr("360网站"));
    m_menu->addAction(tr("隐私保护"));
    m_menu->addAction(tr("关于我们"));
    QAction *m_AactionAboutQt = new QAction(tr("&About Qt"), this);
    connect(m_AactionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    m_menu->addAction(m_AactionAboutQt);


#if 0
    ///背景加载;
    m_pixmapBg.load(":/res/image/frame.png");
    QVector <QPushButton* > m_vecBtn;
    m_vecBtn.push_back(ui->minButton);
    m_vecBtn.push_back(ui->maxButton);
    for (int i = 0; i != m_vecBtn.size(); ++i)
    {
        ///功能选择键只有一个被选中;
        m_vecBtn[i]->setCheckable(true);
        m_vecBtn[i]->setAutoExclusive(true);
    }

    ///状态栏加上链接QLabel;
    ui->label_checkUpdate->setText(
                tr("<a href=http://www.360.cn>"
                   "<font color=white>检查更新</font></a>"));
    ui->label_checkUpdate->setOpenExternalLinks(true);
#endif

}

void TopWindow::menuShow()
{
//    m_menu->exec(this->mapToGlobal(QPoint(700, 20)));
}


