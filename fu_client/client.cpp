#include <QImage>
#include "client.h"
#define H 640
#define W 480
#include<iostream>


Client::Client(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(read()));
    flagRequest = 1;

}

//-----SIGNALS---only in mainwindow.h------------------------

//-----SLOTS---------------------------------------------
void Client::connect()
{
    socket->connectToHost(QHostAddress::LocalHost,9999);
    size = 0;
}

void Client::read()
{
    //socket->setReadBufferSize((640*480*3)*50);
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_0);
    if(size == 0){
        if(socket->bytesAvailable() < (int)sizeof(quint32))
            return;
        in >> size;
        std::cerr << "valor de size: " << size << "\n";
    }
    if(socket->bytesAvailable() < size){//no lee till full mensaje received
        std::cerr << "hay para escribir: " << socket->bytesAvailable() << "\n";//DEBUG
        return;
    }

    QImage image;
    in >> image;

    //image = QImage("/home/nadie/imagen/borinonegre(Xylocopa violacea L).jpg");//DEBUG

    size = 0;//mensage recibido, abrimos poder volver a recibir
    emit sendImage(image);//manda a MainWindow;
    std::cerr << "Tamaño recibido: " << sizeof(image) << "\n";//DEBUG
    std::cerr << "mandada image a MainWindow \n";//DEBUG

    if(flagRequest) request();
}

void Client::request()
{
    QByteArray buff;
    QDataStream out(&buff, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);
    out << (quint32)0;
    out << (quint32)flagRequest;
    out.device()->seek(0);//puntero a inicio
    out << (quint32)(buff.size() - (int)sizeof(quint32));//escribimos tamaño al principio

    std::cerr << "tamaño buff: " << buff.size() << "  \n";//DEBUG

    socket->write(buff);

    std::cerr << "request a server \n";//DEBUG
}

void Client::close()
{
    flagRequest = 0;
    socket->close();
}
