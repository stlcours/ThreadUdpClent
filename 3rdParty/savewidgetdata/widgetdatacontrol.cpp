#include "widgetdatacontrol.h"

#include <QSettings>
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QDial>
#include <QSlider>
#include <QAbstractSlider>
#include <QLabel>
#include <QTextBrowser>
#include <QLCDNumber>
#include <QProgressBar>
#include <QRadioButton>
#include <QCheckBox>
#include <QSettings>
#include <QComboBox>

WidgetDataControl::WidgetDataControl()
{

}

void WidgetDataControl::save(QSettings *settings, const QWidget *widget, const controlFlagsEnum &controlFlags,QString Group="")
{
    if(Group.isEmpty()) {
        if(widget->objectName().isEmpty()) { return; }
        settings->beginGroup(widget->objectName());
    }else{
        settings->beginGroup(Group);
    }


    const QObjectList children = widget->children();
    //comboBox_13
    foreach(QObject *now, children)
    {
        if(now->objectName().isEmpty()) { continue; }
        if(now->objectName().indexOf("qt_") == 0) { continue; }

        if((controlFlags & controlFlagsPos) && now->inherits("QWidget"))
        {
            settings->setValue(now->objectName() + ".pos", qobject_cast<QWidget *>(now)->pos());
        }
        if((controlFlags & controlFlagsSize) && now->inherits("QWidget"))
        {
            settings->setValue(now->objectName() + ".size", qobject_cast<QWidget *>(now)->size());
        }
        if((controlFlags & controlFlagsEnabled) && now->inherits("QWidget"))
        {
            settings->setValue(now->objectName() + ".enabled", qobject_cast<QWidget *>(now)->isEnabled());
        }

        if((controlFlags & controlFlagsQLineEdit) && now->inherits("QLineEdit"))
        {
            settings->setValue(now->objectName() + ".QLineEdit", qobject_cast<QLineEdit *>(now)->text());
            continue;
        }
        if((controlFlags & controlFlagsQTextEdit) && now->inherits("QTextEdit"))
        {
            settings->setValue(now->objectName() + ".QTextEdit", qobject_cast<QTextEdit *>(now)->toPlainText());
            continue;
        }
        if((controlFlags & controlFlagsQSpinBox) && now->inherits("QSpinBox"))
        {
            settings->setValue(now->objectName() + ".QSpinBox", qobject_cast<QSpinBox *>(now)->value());
            continue;
        } //add
        if((controlFlags & controlFlagsQComboBox) && now->inherits("QComboBox"))
        {
            settings->setValue(now->objectName() + ".QComboBox", qobject_cast<QComboBox *>(now)->currentIndex());
            continue;
        }

        if((controlFlags & controlFlagsQDoubleSpinBox) && now->inherits("QDoubleSpinBox"))
        {
            settings->setValue(now->objectName() + ".QDoubleSpinBox", qobject_cast<QDoubleSpinBox *>(now)->value());
            continue;
        }
        if((controlFlags & controlFlagsQTimeEdit) && now->inherits("QTimeEdit"))
        {
            settings->setValue(now->objectName() + ".QTimeEdit", qobject_cast<QTimeEdit *>(now)->time());
            continue;
        }
        if((controlFlags & controlFlagsQDateEdit) && now->inherits("QDateEdit"))
        {
            settings->setValue(now->objectName() + ".QDateEdit", qobject_cast<QDateEdit *>(now)->date());
            continue;
        }
        if((controlFlags & controlFlagsQDateTimeEdit) && now->inherits("QDateTimeEdit"))
        {
            settings->setValue(now->objectName() + ".QDateTimeEdit", qobject_cast<QDateTimeEdit *>(now)->dateTime());
            continue;
        }
        if((controlFlags & controlFlagsQDial) && now->inherits("QDial"))
        {
            settings->setValue(now->objectName() + ".QDial", qobject_cast<QDial *>(now)->value());
            continue;
        }
        if((controlFlags & controlFlagsQSlider) && now->inherits("QSlider"))
        {
            settings->setValue(now->objectName() + ".QSlider", qobject_cast<QSlider *>(now)->value());
            continue;
        }
        if((controlFlags & controlFlagsQAbstractSlider) && now->inherits("QAbstractSlider"))
        {
            settings->setValue(now->objectName() + ".QAbstractSlider", qobject_cast<QAbstractSlider *>(now)->value());
            continue;
        }

        if((controlFlags & controlFlagsQLabel) && now->inherits("QLabel"))
        {
            settings->setValue(now->objectName() + ".QLabel", qobject_cast<QLabel *>(now)->text());
            continue;
        }
        if((controlFlags & controlFlagsQTextBrowser) && now->inherits("QTextBrowser"))
        {
            settings->setValue(now->objectName() + ".QTextBrowser", qobject_cast<QTextBrowser *>(now)->toPlainText());
            continue;
        }
        if((controlFlags & controlFlagsQLCDNumber) && now->inherits("QLCDNumber"))
        {
            settings->setValue(now->objectName() + ".QLCDNumber", qobject_cast<QLCDNumber *>(now)->value());
            continue;
        }
        if((controlFlags & controlFlagsQProgressBar) && now->inherits("QProgressBar"))
        {
            settings->setValue(now->objectName() + ".QProgressBar", qobject_cast<QProgressBar *>(now)->value());
            continue;
        }

        if((controlFlags & controlFlagsQRadioButton) && now->inherits("QRadioButton"))
        {
            settings->setValue(now->objectName() + ".QRadioButton", qobject_cast<QRadioButton *>(now)->isChecked());
            continue;
        }
        if((controlFlags & controlFlagsQCheckBox) && now->inherits("QCheckBox"))
        {
            settings->setValue(now->objectName() + ".QCheckBox", qobject_cast<QCheckBox *>(now)->isChecked());
            continue;
        }
    }

    if(controlFlags & controlFlagsRecursion)
    {
        foreach(QObject *now, children)
        {
            if(now->inherits("QWidget"))
            {
                save(settings, qobject_cast<QWidget *>(now), controlFlags);
            }
        }
    }

    settings->endGroup();
}

void WidgetDataControl::read(QSettings *settings, QWidget *widget, const controlFlagsEnum &controlFlags,QString Group="")
{
    if(Group.isEmpty()) {
        if(widget->objectName().isEmpty()) { return; }
        settings->beginGroup(widget->objectName());
    }else{
        settings->beginGroup(Group);
    }
    const QObjectList children = widget->children();

    foreach(QObject *now, children)
    {
        if(now->objectName().isEmpty()) { continue; }
        if(now->objectName().indexOf("qt_") == 0) { continue; }

        if((controlFlags & controlFlagsPos) && now->inherits("QWidget") && settings->contains(now->objectName() + ".pos"))
        {
            qobject_cast<QWidget *>(now)->move(settings->value(now->objectName() + ".pos").toPoint());
        }
        if((controlFlags & controlFlagsSize) && now->inherits("QWidget") && settings->contains(now->objectName() + ".size"))
        {
            qobject_cast<QWidget *>(now)->resize(settings->value(now->objectName() + ".size").toSize());
        }
        if((controlFlags & controlFlagsSize) && now->inherits("QWidget") && settings->contains(now->objectName() + ".enabled"))
        {
            qobject_cast<QWidget *>(now)->setEnabled(settings->value(now->objectName() + ".enabled").toBool());
        }

        if((controlFlags & controlFlagsQLineEdit) && now->inherits("QLineEdit") && settings->contains(now->objectName() + ".QLineEdit"))
        {
            qobject_cast<QLineEdit *>(now)->setText(settings->value(now->objectName() + ".QLineEdit").toString());
            continue;
        }
        if((controlFlags & controlFlagsQTextEdit) && now->inherits("QTextEdit") && settings->contains(now->objectName() + ".QTextEdit"))
        {
            qobject_cast<QTextEdit *>(now)->setPlainText(settings->value(now->objectName() + ".QTextEdit").toString());
            continue;
        }
        if((controlFlags & controlFlagsQSpinBox) && now->inherits("QSpinBox") && settings->contains(now->objectName() + ".QSpinBox"))
        {
            qobject_cast<QSpinBox *>(now)->setValue(settings->value(now->objectName() + ".QSpinBox").toInt());
            continue;
        }
        ///ADD by yzhu798
        if((controlFlags & controlFlagsQComboBox) && now->inherits("QComboBox") && settings->contains(now->objectName() + ".QComboBox"))
        {
            bool ok;
            qobject_cast<QComboBox *>(now)->setCurrentIndex(settings->value(now->objectName() + ".QComboBox").toInt(&ok));
            continue;
        }
        if((controlFlags & controlFlagsQDoubleSpinBox) && now->inherits("QDoubleSpinBox") && settings->contains(now->objectName() + ".QDoubleSpinBox"))
        {
            qobject_cast<QDoubleSpinBox *>(now)->setValue(settings->value(now->objectName() + ".QDoubleSpinBox").toDouble());
            continue;
        }
        if((controlFlags & controlFlagsQTimeEdit) && now->inherits("QTimeEdit") && settings->contains(now->objectName() + ".QTimeEdit"))
        {
            qobject_cast<QTimeEdit *>(now)->setTime(settings->value(now->objectName() + ".QTimeEdit").toTime());
            continue;
        }
        if((controlFlags & controlFlagsQDateEdit) && now->inherits("QDateEdit") && settings->contains(now->objectName() + ".QDateEdit"))
        {
            qobject_cast<QDateEdit *>(now)->setDate(settings->value(now->objectName() + ".QDateEdit").toDate());
            continue;
        }
        if((controlFlags & controlFlagsQDateTimeEdit) && now->inherits("QDateTimeEdit") && settings->contains(now->objectName() + ".QDateTimeEdit"))
        {
            qobject_cast<QDateTimeEdit *>(now)->setDateTime(settings->value(now->objectName() + ".QDateTimeEdit").toDateTime());
            continue;
        }
        if((controlFlags & controlFlagsQDial) && now->inherits("QDial") && settings->contains(now->objectName() + ".QDial"))
        {
            qobject_cast<QDial *>(now)->setValue(settings->value(now->objectName() + ".QDial").toInt());
            continue;
        }
        if((controlFlags & controlFlagsQSlider) && now->inherits("QSlider") && settings->contains(now->objectName() + ".QSlider"))
        {
            qobject_cast<QSlider *>(now)->setValue(settings->value(now->objectName() + ".QSlider").toInt());
            continue;
        }
        if((controlFlags & controlFlagsQAbstractSlider) && now->inherits("QAbstractSlider") && settings->contains(now->objectName() + ".QAbstractSlider"))
        {
            qobject_cast<QAbstractSlider *>(now)->setValue(settings->value(now->objectName() + ".QAbstractSlider").toInt());
            continue;
        }

        if((controlFlags & controlFlagsQLabel) && now->inherits("QLabel") && settings->contains(now->objectName() + ".QLabel"))
        {
            qobject_cast<QLabel *>(now)->setText(settings->value(now->objectName() + ".QLabel").toString());
            continue;
        }
        if((controlFlags & controlFlagsQTextBrowser) && now->inherits("QTextBrowser") && settings->contains(now->objectName() + ".QTextBrowser"))
        {
            qobject_cast<QTextBrowser *>(now)->setText(settings->value(now->objectName() + ".QTextBrowser").toString());
            continue;
        }
        if((controlFlags & controlFlagsQLCDNumber) && now->inherits("QLCDNumber") && settings->contains(now->objectName() + ".QLCDNumber"))
        {
            qobject_cast<QLCDNumber *>(now)->display(settings->value(now->objectName() + ".QLCDNumber").toInt());
            continue;
        }
        if((controlFlags & controlFlagsQProgressBar) && now->inherits("QProgressBar") && settings->contains(now->objectName() + ".QProgressBar"))
        {
            qobject_cast<QProgressBar *>(now)->setValue(settings->value(now->objectName() + ".QProgressBar").toInt());
            continue;
        }

        if((controlFlags & controlFlagsQRadioButton) && now->inherits("QRadioButton") && settings->contains(now->objectName() + ".QRadioButton"))
        {
            qobject_cast<QRadioButton *>(now)->setChecked(settings->value(now->objectName() + ".QRadioButton").toBool());
            continue;
        }
        if((controlFlags & controlFlagsQCheckBox) && now->inherits("QCheckBox") && settings->contains(now->objectName() + ".QCheckBox"))
        {
            qobject_cast<QCheckBox *>(now)->setChecked(settings->value(now->objectName() + ".QCheckBox").toBool());
            continue;
        }
    }

    if(controlFlags & controlFlagsRecursion)
    {
        foreach(QObject *now, children)
        {
            if(now->inherits("QWidget"))
            {
                read(settings, qobject_cast<QWidget *>(now), controlFlags);
            }
        }
    }

    settings->endGroup();
}
