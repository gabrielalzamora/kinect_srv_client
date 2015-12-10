/*****************************************************************************

File        : mainwindow.h

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QGraphicsScene;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

signals:
    void sendMessage(QString);

public slots:
//    void receiveIp(QString ipAddress);
    void receiveMessage(QString msg);
    void receiveImage(QImage img);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
};

#endif // MAINWINDOW_H
