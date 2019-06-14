    setWindowFlags(Qt::FramelessWindowHint);    //去掉标题栏
    m_pixmap.load(":/img/plant.png");           //加载
    resize(m_pixmap.size());                    //设置窗口大小为图片大小

    // 方法一：
    /*
    setAutoFillBackground(true);                //设置自动填充
    setMask(m_pixmap.mask());                   //设置图片透明的地方为穿透
    */

    // 方法二：
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("image: url(:/img/plant.png);");