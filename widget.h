#ifndef WIDGET_H
#define WIDGET_H

#include <QLabel>
#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeView>
#include <QResource>
#include <QMediaPlayer>
#include <QFileSystemModel>

#include "websocketserver.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    
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
    void updateInfo();
    void updateAction();
    void onRunServer();
private:
    static QStringList supportedMimeTypes();

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
    QString formatTime(qint64 timeMilliSeconds);

    WebSocketServer* server;
};
#endif // WIDGET_H
