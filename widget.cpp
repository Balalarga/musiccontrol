#include "widget.h"

#include <QFileDialog>
#include <QStandardPaths>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    Q_INIT_RESOURCE(res);
    QHBoxLayout* sliderLayout = new QHBoxLayout(this);
    QVBoxLayout* mainLayout = new QVBoxLayout();
    QHBoxLayout* buttons = new QHBoxLayout();
    QHBoxLayout* trackLayout = new QHBoxLayout();
    QHBoxLayout* controlLayout = new QHBoxLayout();
    trackList = new QTreeView(this);
    model = new QFileSystemModel(this);
    volume = new QSlider(this);
    track = new QSlider(Qt::Horizontal, this);
    play = new QPushButton(this);
    next = new QPushButton(this);
    prev = new QPushButton(this);
    selectFolder = new QPushButton("Открыть");
    infoLabel = new QLabel(this);
    timeLabel = new QLabel(this);
    runServer = new QPushButton("Запустить сервер");

    trackList->setModel(model);
    trackList->hideColumn(4);
    trackList->hideColumn(3);
    trackList->hideColumn(2);
    trackList->hideColumn(1);
    QString path = QStandardPaths::standardLocations(QStandardPaths::MusicLocation).value(0, QDir::homePath());
    model->setRootPath(path);
    model->setNameFilterDisables(false);
    model->setNameFilters(QStringList()<<"*.mp3");
    trackList->setRootIndex(model->index(path));


    QString styleSheet = "QPushButton{border:none;background-color:rgba(255, 255, 255, 0);}";
    play->setStyleSheet(styleSheet);
    play->setIcon(QIcon(":icons/play.png"));
    play->setIconSize({50, 50});
    next->setStyleSheet(styleSheet);
    next->setIcon(QIcon(":icons/next.png"));
    next->setIconSize({50, 50});
    prev->setStyleSheet(styleSheet);
    prev->setIcon(QIcon(":icons/prev.png"));
    prev->setIconSize({50, 50});

    sliderLayout->addLayout(mainLayout);
    sliderLayout->addWidget(volume);
    volume->setRange(0, 100);
    volume->setValue(30);
    mediaPlayer.setVolume(30);
    connect(volume, &QSlider::valueChanged, this, &Widget::volumeChanged);
    controlLayout->addWidget(selectFolder);
    controlLayout->addWidget(runServer);
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(trackList);
    mainLayout->addWidget(infoLabel);
    mainLayout->addLayout(buttons);
    buttons->addWidget(prev);
    buttons->addWidget(play);
    buttons->addWidget(next);
    mainLayout->addLayout(trackLayout);
    trackLayout->addWidget(track);
    trackLayout->addWidget(timeLabel);

    connect(play, &QPushButton::clicked, this, &Widget::togglePlayback);
    connect(next, &QPushButton::clicked, this, &Widget::nextTrack);
    connect(prev, &QPushButton::clicked, this, &Widget::prevTrack);
    connect(selectFolder, &QPushButton::clicked, this, &Widget::openFolder);
    connect(track, &QSlider::valueChanged, this, &Widget::setTrackPosition);
    connect(trackList, &QTreeView::doubleClicked, this, &Widget::selectedTrack);
    connect(track, &QSlider::valueChanged, this, &Widget::setPosition);
    connect(&mediaPlayer, &QMediaPlayer::stateChanged,
            this, &Widget::updateState);
    connect(runServer, &QPushButton::clicked, this, &Widget::onRunServer);
    connect(&mediaPlayer, &QMediaPlayer::positionChanged, this, &Widget::updatePosition);
    connect(&mediaPlayer, &QMediaPlayer::durationChanged, this, &Widget::updateDuration);
    connect(&mediaPlayer, &QMediaPlayer::stateChanged, this, &Widget::updateAction);
     play->setEnabled(false);
}

#include <QMessageBox>
void Widget::onRunServer(){
    int port = 5413;
    if(!server->isRunning()){
        server = new WebSocketServer(port);

        runServer->setText("Остановить сервер");
        QMessageBox::about(this, "Информация о сервере", "Сервер запущен");
    }
    else{
        server->stop();
        runServer->setText("Запустить сервер");
        QMessageBox::about(this, "Информация о сервере", "Сервер остановлен");
    }
}

void Widget::updateAction(){
    switch (mediaPlayer.state()) {
        case QMediaPlayer::PlayingState:

            break;
        case QMediaPlayer::PausedState:

            break;
        case QMediaPlayer::StoppedState:
            if(mediaPlayer.mediaStatus() == QMediaPlayer::EndOfMedia)
                nextTrack();
            break;
    }
}

Widget::~Widget()
{
}

void Widget::setTrackPosition(int position)
{
    if (qAbs(mediaPlayer.position() - position) > 99)
        mediaPlayer.setPosition(position);
}

void Widget::togglePlayback()
{
    if (mediaPlayer.mediaStatus() == QMediaPlayer::NoMedia)
        openFolder();
    else if (mediaPlayer.state() == QMediaPlayer::PlayingState)
        mediaPlayer.pause();
    else
        mediaPlayer.play();
}

void Widget::openFolder()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Folder"));
    fileDialog.setMimeTypeFilters(Widget::supportedMimeTypes());
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MusicLocation).value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted){
        QString path = fileDialog.selectedFiles().first();
        model->setRootPath(path);
        trackList->setRootIndex(model->index(path));
    }
}

#include <QRegExp>
void Widget::playUrl(const QString filename)
{
    QUrl url = QUrl(model->rootPath()+'/'+filename);
    qDebug()<<url;
    play->setEnabled(true);
    if (url.isLocalFile()) {
        const QString filePath = url.toLocalFile();
        setWindowFilePath(filePath);
        infoLabel->setText(filename.section(QRegExp("*.mp3"), 0));
        currentFile = model->index(filename);
    } else {
        setWindowFilePath(QString());
        infoLabel->setText(filename.section(QRegExp("*.mp3"), 0));
    }
    mediaPlayer.setMedia(url);
    mediaPlayer.play();
}

void Widget::selectedTrack(const QModelIndex &index)
{
    QString filename = index.data().toString();
    qDebug()<<filename;
    currentFile = index;
    playUrl(filename);
}
#include <QAbstractItemView>
void Widget::nextTrack()
{
    qDebug()<<"next";
    int row;
    if(model->rootDirectory().count() > (uint)currentFile.row()+1)
        row = currentFile.row()+1;
    else
        row = 0;
    auto newItem = model->index(row,
                                currentFile.column(),
                                currentFile.parent());
    currentFile = newItem;
    trackList->clearSelection();
    playUrl(currentFile.data().toString());
}

void Widget::prevTrack()
{
    qDebug()<<"prev";
    int row;
    if(currentFile.row()-1 > -1)
        row = currentFile.row()-1;
    else
        row = model->rootDirectory().count() - 1;
    auto newItem = model->index(row,
                                currentFile.column(),
                                currentFile.parent());
    currentFile = newItem;
    trackList->clearSelection();
    playUrl(currentFile.data().toString());
}

void Widget::volumeChanged(int value)
{
    mediaPlayer.setVolume(value);
}

void Widget::seekForward()
{
    track->triggerAction(QSlider::SliderPageStepAdd);
}

void Widget::seekBackward()
{
    track->triggerAction(QSlider::SliderPageStepSub);
}

QStringList Widget::supportedMimeTypes()
{
    QStringList result = QMediaPlayer::supportedMimeTypes();
    if (result.isEmpty())
        result.append(QStringLiteral("audio/mpeg"));
    return result;
}

void Widget::updateState(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::PlayingState) {
        play->setIcon(QIcon(":icons/pause.png"));
    } else {
        play->setIcon(QIcon(":icons/play.png"));
    }
}

void Widget::updatePosition(qint64 position)
{
    track->setValue(position);
    timeLabel->setText(formatTime(position)
                       +"/"+
                       formatTime(mediaPlayer.duration()));
}

void Widget::updateDuration(qint64 duration)
{
    track->setRange(0, duration);
    track->setEnabled(duration > 0);
    track->setPageStep(duration / 10);
    updateInfo();
}

void Widget::setPosition(int position)
{
    if (qAbs(mediaPlayer.position() - position) > 99)
        mediaPlayer.setPosition(position);
}
void Widget::updateInfo()
{
//    if (!currentFile.data().toString().isEmpty())
//        return;
//    timeLabel->setText(formatTime(mediaPlayer.duration())
//                       +"/"+
//                       formatTime(track->value()));
}

QString Widget::formatTime(qint64 timeMilliSeconds)
{
    qint64 seconds = timeMilliSeconds / 1000;
    const qint64 minutes = seconds / 60;
    seconds -= minutes * 60;
    return QStringLiteral("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}
