/*****************************************************************************

File        : server.cpp

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

#include <iostream>
#include <QApplication>
#include <QImage>
#include "freenect_usal.h"
#include "server.h"
//#include "mainwindow.h"

extern int die;// first defined in freenect_usal.h

Server::Server(QObject *parent) : QTcpServer(parent)
{
    server = new QTcpServer(this);
    if( !server->listen(QHostAddress::Any,9999) ){
        sendMessage(QLatin1String("Server no Listen errooooor"));
        close();
    }
    size = 0;//indica que no hemos leido nada

QMetaObject::Connection SN;
    SN = connect(server, &Server::newConnection, this, &Server::socketConnect);
    if( !(bool)SN ) std::cerr << "falla en Server::server newConnection -> socketConnect";

}

Server::~Server()
{
    free(rgb_back);
    free(rgb_mid);
    free(rgb_front);
}
//-----SIGNALS---only in server.h------------------------

//-----SLOTS---------------------------------------------
void Server::startKinect()//hay que simplificar con funciones de la API_FU
{
    die = 0;//achtung!!! usas die en freenect_usal.h y aquí en while????

    emit sendMessage(QLatin1String("activamos Kinect"));

    //-------------iniciamos freenect
    rgb_back = (uint8_t*)malloc(W*H*3);//buffers
    rgb_mid = (uint8_t*)malloc(W*H*3);
    rgb_front = (uint8_t*)malloc(W*H*3);

    if (freenect_init(&context, NULL) < 0) {// init freenect context
        emit sendMessage(QLatin1String("error freenect_init()"));
        return;
    }
    emit sendMessage(QLatin1String("freenect_init() started"));

    //freenect_set_log_level(context, FREENECT_LOG_DEBUG);
    freenect_select_subdevices(context, (freenect_device_flags)(FREENECT_DEVICE_CAMERA));
    int nr_devices = freenect_num_devices(context);

    QString msg;
    msg.setNum(nr_devices);
    msg.prepend("Number of devices found: ");
    emit sendMessage(msg);

    int user_device_number = 0;// only one device?

    if (nr_devices < 1) {
        freenect_shutdown(context);
        return;
    }
    if (freenect_open_device(context, &device, user_device_number) < 0) {
        emit sendMessage(QLatin1String("Could not open device"));
        freenect_shutdown(context);
        return;
    }
    int aux = pthread_create(&fu_thread, NULL, fu_thread_function, NULL);// load buffer whith rgb
    if (aux) {
        emit sendMessage(QLatin1String("pthread_create failed"));
        freenect_shutdown(context);
        return;
    }
    while(!die){//------------------------------------bucle while

        pthread_mutex_lock(&buffer_mutex);
        u_int8_t *aux;
        if(got_rgb){
            aux = rgb_front;
            rgb_front = rgb_mid;
            rgb_mid = aux;
            got_rgb = 0;
        }
        pthread_mutex_unlock(&buffer_mutex);

        image = QImage( rgb_front ,
                        W,
                        H,
                        QImage::Format_RGB888,
                        NULL, NULL);//generate image from buffer

        emit sendImageMain(image);
        qApp->processEvents();//stay responsive to button click
    }//---------------------------------------------------end while

}

void Server::stopKinect()
{
    die = 1;
    emit sendMessage(QLatin1String("estamos saliendo stopKinect"));
}

void Server::socketConnect()
{
    sendMessage(QLatin1String("entra en socketConnect"));

    socket = server->nextPendingConnection();

    QMetaObject::Connection SN;
    SN = connect(socket, &QTcpSocket::readyRead, this, &Server::readRequest);
    if( !(bool)SN ) std::cerr << "falla en QTcpSocket::readyRead -> Server::readRequest";
}

void Server::readRequest()//and send image to client
{
    sendMessage(QLatin1String("entra en readRequest"));//DEBUG

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_0);
    if (size == 0) {
        if (socket->bytesAvailable() < sizeof(quint32))//siempre comprobar hay datos a leer
            return;
        in >> size;//read size of incomming data
    }
    if (socket->bytesAvailable() < size)//hasta que no estén todos no seguimos
        return;

    quint32 flag;//si envía 0 will stop and disconnect, si != 0 enviamos pixmap
    in >> flag;

    std::cerr << "tamaño: " << size << "  info: " << flag << "  \n";//DEBUG

    if( flag == 0 ){//para poder request disconnect
        socket->disconnectFromHost();
        sendMessage(QLatin1String("Cliente ordena desconectar"));
        return;
    }//---------------solo pasa si mensaje recibido completo y flag != 0
    size = 0;//para volver a recibir mensajes

    emit sendImageMain(image);//DEBUG
    sendMessage(QLatin1String("imagen a MainWindow"));//DEBUG

    //---------------------------send image to client
    QByteArray buff;
    QDataStream out(&buff, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);
    out << quint32(0) << image;
    out.device()->seek(0);//puntero a inicio buff
    out << quint32(buff.size() - sizeof(quint32));//escribimos tamaño buf al principio

    std::cerr << "tamaño antes enviado: " << sizeof(buff) << "  \n";//DEBUG

    socket->write(buff);//enviamos
    //---------------------------sended image to client

    std::cerr << "tamaño enviado: " << sizeof(buff) << "  \n";//DEBUG

    sendMessage(QLatin1String("image a Cliente enviado"));//DEBUG
}


