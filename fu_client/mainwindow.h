#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "client.h"

class QHostAddress;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:
    void receiveImage(QImage img);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

    int port;
    QHostAddress address;
};

#endif // MAINWINDOW_H
