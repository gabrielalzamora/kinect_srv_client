/*****************************************************************************

File        : server.h

Proyect     : API Kinect
Copyright (c) 2015  Gabriel Alzamora

based on the examples of the Qt Toolkit
Copyright (c) 2015 The Qt Company Ltd
http://www.qt.io/licensing/

Author      : Gabriel Alzamora     <gabrielalzamora@usal.es>
Tutor       : Raul Alves           <ralves@usal.es>
Version     : 1.0
Description : read Kinect & show on QT gui

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/

#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
//#include <QPixmap>
#include <QImage>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = 0);
    ~Server();

signals:
//    void sendIp(QString ipAddress);
    void sendMessage(QString msg);
    void sendImageMain(QImage video);

public slots:
    void startKinect();
    void stopKinect();
    void socketConnect();
    void readRequest();

private:
    void incomingConnection(int socketId);
    QTcpServer *server;
    QTcpSocket *socket;
    quint32 size;
    QImage image;

};

#endif // SERVER_H
