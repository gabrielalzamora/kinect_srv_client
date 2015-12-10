
#include "client.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    Client *client = new Client(this);

    connect(ui->pbConnect, &QPushButton::clicked, client, &Client::connect);
    connect(ui->pbQuit, &QPushButton::clicked, client, &Client::close);
    connect(ui->pbNext, &QPushButton::clicked, client, &Client::request);
    connect(client, &Client::sendImage, this, &MainWindow::receiveImage);

}

MainWindow::~MainWindow()
{
    delete ui;
}
//-----SIGNALS---only in mainwindow.h------------------------

//-----SLOTS---------------------------------------------
void MainWindow::receiveImage(QImage img)
{
    scene->addPixmap(QPixmap::fromImage(img));
    ui->graphicsView->show();
}
