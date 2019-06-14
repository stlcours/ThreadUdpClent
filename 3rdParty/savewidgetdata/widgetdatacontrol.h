#ifndef WIDGETDATACONTROL_H
#define WIDGETDATACONTROL_H

#include <QString>



class QSettings;
class QWidget;

class WidgetDataControl
{
public:
    WidgetDataControl();
    enum controlFlagsEnum
    {
        controlFlagsRecursion       = 0x1,
        controlFlagsPos             = 0x2,
        controlFlagsSize            = 0x4,
        controlFlagsGeometry        = 0x6,
        controlFlagsEnabled         = 0x8,
        //        00000001
        //        00000010
        //        00000100
        //        00001000

        controlFlagsQLineEdit       =   0x10,
        controlFlagsQTextEdit       =   0x20,
        controlFlagsQSpinBox        =   0x40,
        controlFlagsQComboBox       =   0x60,
        controlFlagsQDoubleSpinBox  =   0x80,


        //         00010000
        //         00100000
        //         01000000
        //         10000000
        controlFlagsQTimeEdit       =  0x100,
        controlFlagsQDateEdit       =  0x200,
        controlFlagsQDateTimeEdit   =  0x400,
        controlFlagsQDial           =  0x800,

        controlFlagsQSlider         = 0x1000,
        controlFlagsQAbstractSlider = 0x2000,


        controlFlagsInputWidgets    = 0xfff0,

        controlFlagsQLabel          = 0x10000,
        controlFlagsQTextBrowser    = 0x20000,
        controlFlagsQLCDNumber      = 0x40000,
        controlFlagsQProgressBar    = 0x80000,
        controlFlagsDisplayWidgets  = 0xf0000,

        controlFlagsQRadioButton    = 0x1000000,
        controlFlagsQCheckBox       = 0x2000000,
        controlFlagsButtons         = 0xf000000,



        controlFlagsAll             = 0xffffffff

    };

public:

    static void  save(QSettings *settings, const QWidget *widget, const controlFlagsEnum &controlFlags, QString Group);
    static void read(QSettings *settings, QWidget *widget, const controlFlagsEnum &controlFlags, QString Group);
};

#endif // WIDGETDATACONTROL_H
