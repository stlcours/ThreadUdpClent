#include "dialog_setting.h"
#include "ui_dialog_setting.h"

Dialog_Setting::Dialog_Setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Setting)
{
    ui->setupUi(this);
    QRegExp rx2("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    ui->lineEdit->setValidator(new QRegExpValidator(rx2, this));
    ui->lineEdit_2->setValidator(new QRegExpValidator(rx2, this));

    currentSettings.localIp =ui->lineEdit->text();
    currentSettings.listnerPort =ui->spinBox->value();

    currentSettings.remoteIp =ui->lineEdit_2->text();
    currentSettings.remotePort =ui->spinBox_2->value();
}

Dialog_Setting::~Dialog_Setting()
{
    delete ui;
}

UdpSocketSettings Dialog_Setting::settings() const
{
    return currentSettings;
}

void Dialog_Setting::on_applyButton_clicked()
{
    currentSettings.localIp =ui->lineEdit->text();
    currentSettings.listnerPort =ui->spinBox->value();

    currentSettings.remoteIp =ui->lineEdit_2->text();
    currentSettings.remotePort =ui->spinBox_2->value();
}
