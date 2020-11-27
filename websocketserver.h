#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QWebSocketServer>
#include <QThread>
#include <QJsonObject>

class WebSocketServer: public QObject
{
    Q_OBJECT
public:
    WebSocketServer(quint16 port, QObject *parent = nullptr);
    ~WebSocketServer() override;
    void stop();
    bool isRunning();
    QString address;

public slots:
    void sendData(QJsonObject obj);
    void onDataRecived(QByteArray data);

signals:
    void getData(QJsonObject obj);
    void connected();

private slots:
    void onNewConnection();
    void processMessage(const QString &message);
    void socketDisconnected();

private:
    QWebSocketServer* m_server = nullptr;
    QWebSocket* m_client = nullptr;
    bool running = false;
};

#endif // WEBSOCKETSERVER_H
