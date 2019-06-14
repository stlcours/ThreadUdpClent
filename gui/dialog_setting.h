#ifndef DIALOG_SETTING_H
#define DIALOG_SETTING_H

#include <QDialog>
#include "global.h"
namespace Ui {
class Dialog_Setting;
}

class Dialog_Setting : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Setting(QWidget *parent = 0);
    ~Dialog_Setting();

    UdpSocketSettings settings() const;

private slots:
    void on_applyButton_clicked();

private:
    Ui::Dialog_Setting *ui;
    struct  UdpSocketSettings currentSettings;
};

#endif // DIALOG_SETTING_H
