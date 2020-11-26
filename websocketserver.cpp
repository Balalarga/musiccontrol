#include "websocketserver.h"

#include <QWebSocket>
#include <QNetworkInterface>

static QString getIdentifier(QWebSocket *peer)
{
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       QString::number(peer->peerPort()));
}

WebSocketServer::WebSocketServer(quint16 port, QObject *parent) :
    QObject(parent),
    m_server(new QWebSocketServer(QStringLiteral("Music Server"),
                                            QWebSocketServer::NonSecureMode,
                                            this))
{
    if (m_server->listen(QHostAddress::Any, port))
    {
        QTextStream(stdout) << "Music Server listening on port " << port << '\n';
        connect(m_server, &QWebSocketServer::newConnection,
                this, &WebSocketServer::onNewConnection);
    }
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost){
             this->address = address.toString();
             break;
        }
    }
    running = true;
}

WebSocketServer::~WebSocketServer()
{
    if(m_client)
        m_client->close();
    if(m_server)
        m_server->close();
}

void WebSocketServer::stop()
{
    if(m_client)
        m_client->abort();
    if(m_server)
        m_server->close();
    running = false;
}

bool WebSocketServer::isRunning()
{
    return running;
}

void WebSocketServer::onDataRecived(QByteArray data)
{
    qDebug()<<"Recived: ";
    qDebug()<<data;
}
#include <QJsonDocument>
void WebSocketServer::sendData(QJsonObject obj)
{
    if(!m_client)
        return;
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson();
    m_client->sendBinaryMessage(bytes);
}


void WebSocketServer::onNewConnection()
{
    auto pSocket = m_server->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(pSocket) << " connected!\n";
    pSocket->setParent(this);

    connect(pSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketServer::processMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived,
            this, &WebSocketServer::onDataRecived);
    connect(pSocket, &QWebSocket::disconnected,
            this, &WebSocketServer::socketDisconnected);

    m_client = pSocket;
    QJsonObject obj;
    obj["name"] = "name";
    obj["some"] = "some";
    sendData(obj);
}

void WebSocketServer::processMessage(const QString &message)
{
    qDebug()<<QJsonValue(message).toObject()["name"];
    qDebug()<<QJsonValue(message).toObject()["data"];
    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
    if (m_client == pSender) //don't echo message back to sender
        m_client->sendTextMessage(message);
}

void WebSocketServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
    if (pClient)
    {
        m_client->deleteLater();
    }
}
