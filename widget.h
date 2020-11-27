#ifndef WIDGET_H
#define WIDGET_H

#include <QLabel>
#include <QWidget>
#include <QSlider>
#include <QTreeView>
#include <QResource>
#include <QJsonArray>
#include <QJsonObject>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QFileSystemModel>

#include "websocketserver.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

signals:
    void dataUpdated(QJsonObject obj);
    void durationUpdated();
    
private slots:
    void updateDuration(qint64 duration);
    void setTrackPosition(int position);
    void togglePlayback();
    void openFolder();
    void playUrl(const QString filename);
    void selectedTrack(const QModelIndex &index);
    void nextTrack();
    void prevTrack();
    void volumeChanged(int value);
    void seekForward();
    void seekBackward();
    void updateState(QMediaPlayer::State state);
    void updatePosition(qint64 position);
    void setPosition(int position);
    void updateAction();
    void onRunServer();
    void volumeReleased();
    void positionReleased();
    void wasConnected();
    void setProperty(QJsonObject obj);
    void setVolumeProperty(int value);
    void setPositionProperty(int value);
    void sendData(QJsonObject obj);

private:
    static QStringList supportedMimeTypes();
    QString formatTime(qint64 timeMilliSeconds);
    void updateTracks();

private:
    QPushButton* runServer;
    QSlider* track;
    QSlider* volume;
    QTreeView* trackList;
    QPushButton* play;
    QPushButton* next;
    QPushButton* prev;
    QPushButton* selectFolder;
    QMediaPlayer mediaPlayer;
    QLabel* infoLabel;
    QLabel* timeLabel;

    QModelIndex currentFile;
    QFileSystemModel* model;

    WebSocketServer* server;
    QJsonObject currentConfig;

    bool mousePressed = false;
};
#endif // WIDGET_H
