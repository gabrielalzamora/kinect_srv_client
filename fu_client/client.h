#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
//#include <QTcpSocket>
#include <QtNetwork>
//#include <QPixmap>
#include <QImage>


class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);

signals:
    void sendImage(QImage video);//to MainWindow

public slots:
    void connect();
    void read();
    void request();
    void close();

private slots:

private:
    QTcpSocket *socket;
    quint32 size;
    int flagRequest;

};

#endif // CLIENT_H
