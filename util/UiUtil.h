#ifndef UIUTIL_H
#define UIUTIL_H

#pragma once

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
class QString;
class QWidget;
class QStackedWidget;
class ScheduleDescription;
class ScheduleItem;
class QTableView;
class QAbstractItemView;
class QModelIndex;
class QPoint;
class QPushButton;
class QLineEdit;

template<typename ScheduleItem> class QList;

/**
 * Ui 相关的辅助工具类
 */
class UiUtil {
public:
    static void loadQss();   // 为整个应用程序加载 QSS
    static void loadFonts(); // 加载字体
    static void updateQss(QWidget* widget); // 修改过 widget 的属性后，使此属性对应的 Style Sheet 生效
    static void installLoadQssShortcut(QWidget *parent); // 安装加载 QSS 的快捷键: Ctrl + L

    // 把 widget 加入到 stacked widget 里, 可以设置向四个方向的伸展
    static void addWidgetIntoStackedWidget(QWidget *widget, QStackedWidget *stackedWidget,
                                           bool toLeft = true,
                                           bool toTop = true,
                                           bool toRight = true,
                                           bool toBottom = true);
    // 把使用上面的函数 addWidgetIntoStackedWidget 添加过的 widget 设置为它的当前 widget
    static void setCurrentWidgetOfStackedWidget(QWidget *widget, QStackedWidget *stackedWidget);

    // 设置 widget 的 padding 和 spacing
    static void setWidgetPaddingAndSpacing(QWidget *widget, int padding, int spacing);

    static QModelIndex getSelectedIndex(QAbstractItemView *view);
    static void appendTableViewRow(QTableView *view, int editColumn);
    static void removeTableViewSelectedRow(QTableView *view);
    static void setTabViewColumnSpace(QTableView *view); //QTableView 按内容设置标题宽度


    // 移动窗口到屏幕的中间
    static void centerWindow(QWidget *window);

    // 查找 w 所在窗口的顶层窗口
    static QWidget* findParentWindow(QWidget *w);

    // 返回 view 在全局坐标 globalPosition 处的 index
    static QModelIndex indexAt(QAbstractItemView *itemView, const QPoint &globalPosition);

    // 下载预览 url 的图片
    static void previewImage(const QString &url, const QString &dir);

    // 给 QLineEdit 最右边创建一个按钮
    static QPushButton* createLineEditRightButton(QLineEdit *edit);

    // 删除 widget 中的所有子 widgets
    static void emptyWidget(QWidget *widget);

    //  依靠文件锁实现单例
    static bool mySingleApp();

    //  QLineEdit正则表达式范围
    static void QRegExp_Int_Rang(QLineEdit *lineEdit, int low , int high);

    //  QLineEdit正则表达式 ip
    static void QRegExp_ipv4(QLineEdit *lineEdit);
};

#endif // UIUTIL_H
