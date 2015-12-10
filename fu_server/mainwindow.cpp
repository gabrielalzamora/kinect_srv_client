/*****************************************************************************

File        : mainwindow.cpp

Proyect     : API Kinect
Copyright (c) 2015  Gabriel Alzamora

Author      : Gabriel Alzamora     <gabrielalzamora@usal.es>
Tutor       : Raul Alves           <ralves@usal.es>
Version     : 1.0
Description : read Kinect & serve it to clients

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
#include <QCloseEvent>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QGraphicsScene>
#include <QHostAddress>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"

extern int die;//declared through server.h -> freenect_usal.h

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    Server *server = new Server(this);

    QMetaObject::Connection SN;//DEBUG

    SN = connect(ui->pbKinect, &QPushButton::clicked, server, &Server::startKinect);
    if( !(bool)SN ) std::cerr << "falla pbKinect -> startKineck";//DEBUG

    SN = connect(ui->pbQuit, &QPushButton::clicked, server, &Server::stopKinect);
    if( !(bool)SN ) std::cerr << "falla pbQuit -> stopKinect";//DEBUG

    SN = connect(this, &MainWindow::sendMessage, this, &MainWindow::receiveMessage);
    if( !(bool)SN ) std::cerr << "falla MainWindow::sendMessage -> receiveMessage";//DEBUG

    SN = connect(server, &Server::sendMessage, this, &MainWindow::receiveMessage);
    if( !(bool)SN ) std::cerr << "falla Server::sendMessage -> receiveMessage";//DEBUG

    SN = connect(server, &Server::sendImageMain, this, &MainWindow::receiveImage);
    if( !(bool)SN ) std::cerr << "falla sendImageMain::sendMessage -> receiveImage";//DEBUG

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit ui->pbQuit->clicked();
    event->accept();
}

//-----SIGNALS---only in mainwindow.h------------------------

//-----SLOTS---------------------------------------------
void MainWindow::receiveMessage(QString msg)
{
    QFile file("log.txt");
    QTextStream log(&file);
    if(!file.open(QIODevice::Append | QIODevice::Text)){
        //ui->labelIn->setText("log.txt no se abre");
    }else{
        log.setDevice(&file);//asignamos fichero log.txt al I/O log
        //log.seek(file.size());
        log << "msg received ---> \t" << msg  << endl;
    }
    file.close();
}

void MainWindow::receiveImage(QImage img)
{
    scene->addPixmap(QPixmap::fromImage(img));
    ui->graphicsView->show();
}
