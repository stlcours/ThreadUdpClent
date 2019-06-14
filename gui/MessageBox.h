#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QWidget>

class QPushButton;

class TopWindow;
namespace Ui {
    class MessageBox;
}

/**
 * 自定义消息对话框，提供普通消息对话框和确认消息对话框，使用方式如下:
 *     MessageBox::message("Your Message")
 *     if (MessageBox::confirm("确定要删除 XX 吗?")) {}
 */
class MessageBox : public QWidget {
    Q_OBJECT

public:
    explicit MessageBox(const QString &message, bool confirm = true);
    ~MessageBox();

    /**
     * @brief 显示模态消息对话框，但是不会阻塞当前线程。
     *        消息可以是普通文本，也可以是 HTML(可以有 <img><font> 等标签，能够显示丰富的消息样式)
     *        可以指定宽度和高度，如果宽度和高度小于 sizeHint() 给出的大小，则使用 sizeHint()，
     *        如果大于 sizeHint() 则使用我们指定的宽度和高度
     *
     * @param msg    要显示的消息
     * @param width  消息对话框的宽度
     * @param height 消息对话框的高度
     * @param windowPaddings // 参考 TopWindow 的构造函数参数说明，一下几个参数也是一样的
     * @param borderImageBorders
     * @param borderImagePath
     * @param borderImageHorizontalStretch
     * @param borderImageVerticalStretch
     */
    static void message(const QString  &msg, int width = 300, int height = 150,
                        const QMargins &windowPaddings     = QMargins(13, 13, 13, 13),
                        const QMargins &borderImageBorders = QMargins(18, 18, 18, 18),
                        const QString  &borderImagePath    = QString("image/top-window/colorful-border.png"),
                        bool  borderImageHorizontalStretch = false,
                        bool  borderImageVerticalStretch   = false);

    /**
     * @brief 显示模态确认对话框，会阻塞当前线程。
     *        消息可以是普通文本，也可以是 HTML(可以有 <img><font> 等标签，能够显示丰富的消息样式)
     *        可以指定宽度和高度，如果宽度和高度小于 sizeHint() 给出的大小，则使用 sizeHint()，
     *        如果大于 sizeHint() 则使用我们指定的宽度和高度
     *
     * @param msg    要显示的消息
     * @param width  消息对话框的宽度
     * @param height 消息对话框的高度
     * @param windowPaddings  // 参考 TopWindow 的构造函数参数说明，一下几个参数也是一样的
     * @param borderImageBorders
     * @param borderImagePath
     * @param borderImageHorizontalStretch
     * @param borderImageVerticalStretch
     * @return 点击确认返回 true，点击取消返回 false
     */
    static bool confirm(const QString  &msg, int width = 300, int height = 150,
                        const QMargins &windowPaddings     = QMargins(13, 13, 13, 13),
                        const QMargins &borderImageBorders = QMargins(18, 18, 18, 18),
                        const QString  &borderImagePath    = QString("image/top-window/colorful-border.png"),
                        bool  borderImageHorizontalStretch = false,
                        bool  borderImageVerticalStretch   = false);

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    static void setWindowForMessageBox(TopWindow *window, int width, int height); // 设置 TopWindow 的属性

    Ui::MessageBox *ui;
    QPushButton *closeButton;
    bool result = false;
};

#endif // MESSAGEBOX_H
