#ifndef GLOBAL_H
#define GLOBAL_H
#include <QHostAddress>
#include<QString>
//#include<QSerialPort>

struct UdpSocketSettings {
    QString localIp ;
    int listnerPort;
    QString remoteIp;
    int remotePort;
};
extern UdpSocketSettings g_socket_setting;
#endif // GLOBAL_H
