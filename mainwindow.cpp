#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStyle>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTime>
#include <QStandardPaths>
#include <QTimer>
#include <QEvent>
#include <QAudioOutput>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);

    m_player->setVideoOutput(ui->videoWidget);
    m_player->setAudioOutput(m_audioOutput);

    // 设置图标
    ui->playBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->stopBtn->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->fullscreenBtn->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));

    // 信号槽连接
    connect(ui->openBtn, &QPushButton::clicked, this, &MainWindow::onOpenFile);
    connect(ui->playBtn, &QPushButton::clicked, this, &MainWindow::onPlayPause);
    connect(ui->stopBtn, &QPushButton::clicked, this, &MainWindow::onStop);
    connect(ui->fullscreenBtn, &QPushButton::clicked, this, &MainWindow::onFullscreen);
    
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);
    connect(ui->progressSlider, &QSlider::sliderMoved, this, &MainWindow::onSliderMoved);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);
    
    connect(ui->playlistWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onPlaylistDoubleClicked);

    // 初始设置
    m_audioOutput->setVolume(0.7);
    ui->volumeSlider->setValue(70);

    // 设置定时器用于自动隐藏控制栏
    m_hideTimer = new QTimer(this);
    m_hideTimer->setInterval(3000);
    connect(m_hideTimer, &QTimer::timeout, this, &MainWindow::hideControlBar);

    // 开启鼠标追踪
    this->setMouseTracking(true);
    ui->centralwidget->setMouseTracking(true);
    ui->videoWidget->setMouseTracking(true);
    ui->videoWidget->installEventFilter(this);

    // 应用白色现代皮肤 (QSS)
    this->setStyleSheet(
        "QMainWindow { background-color: #ffffff; }"
        "QWidget#centralwidget { background-color: #ffffff; }"
        "QVideoWidget { background-color: #000000; border: 1px solid #e5e7eb; border-radius: 8px; }"
        "QSlider::groove:horizontal { height: 6px; background: #f3f4f6; border-radius: 3px; }"
        "QSlider::sub-page:horizontal { background: #6366f1; border-radius: 3px; }"
        "QSlider::handle:horizontal { background: #ffffff; border: 2px solid #6366f1; width: 14px; height: 14px; margin: -4px 0; border-radius: 7px; }"
        "QPushButton { background-color: #f3f4f6; border: 1px solid #e5e7eb; color: #374151; padding: 6px; border-radius: 6px; min-width: 34px; }"
        "QPushButton#openBtn { background-color: #6366f1; color: white; border: none; padding: 6px 18px; }"
        "QListWidget { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 8px; color: #374151; }"
        "QListWidget::item:selected { background-color: #f3f4f6; color: #6366f1; border-left: 4px solid #6366f1; }"
        "QLabel { color: #4b5563; font-size: 12px; }"
    );

    setWindowTitle("EasyPlayer Pro (White Edition)");
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->videoWidget) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            onFullscreen();
            return true;
        }
        if (event->type() == QEvent::MouseMove) {
            mouseMoveEvent(static_cast<QMouseEvent*>(event));
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    const int step = 5000;
    switch (event->key()) {
        case Qt::Key_Right:
            if (!event->isAutoRepeat()) m_player->setPosition(m_player->position() + step);
            m_player->setPlaybackRate(2.0);
            break;
        case Qt::Key_Left:
            if (!event->isAutoRepeat()) m_player->setPosition(m_player->position() - step);
            break;
        case Qt::Key_Up:
            onVolumeChanged(static_cast<int>(m_audioOutput->volume() * 100) + 5);
            ui->volumeSlider->setValue(static_cast<int>(m_audioOutput->volume() * 100));
            break;
        case Qt::Key_Down:
            onVolumeChanged(static_cast<int>(m_audioOutput->volume() * 100) - 5);
            ui->volumeSlider->setValue(static_cast<int>(m_audioOutput->volume() * 100));
            break;
        case Qt::Key_Escape:
            if (this->isFullScreen()) onFullscreen();
            break;
        default:
            QMainWindow::keyPressEvent(event);
            break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right && !event->isAutoRepeat()) {
        m_player->setPlaybackRate(1.0);
    }
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    ui->controlBarContainer->show();
    this->setCursor(Qt::ArrowCursor);
    if (this->isFullScreen()) m_hideTimer->start();
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::hideControlBar()
{
    if (this->isFullScreen()) {
        ui->controlBarContainer->hide();
        this->setCursor(Qt::BlankCursor);
    }
}

void MainWindow::onOpenFile()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Open Media", 
        QStandardPaths::writableLocation(QStandardPaths::MoviesLocation),
        "Video Files (*.mp4 *.mkv *.avi *.mov);;All Files (*.*)");

    if (!fileNames.isEmpty()) {
        for (const QString &file : fileNames) ui->playlistWidget->addItem(file);
        m_currentFile = fileNames.first();
        m_player->setSource(QUrl::fromLocalFile(m_currentFile));
        m_player->play();
        updatePlayBtnIcon();
    }
}

void MainWindow::onPlayPause()
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState) m_player->pause();
    else if (!m_currentFile.isEmpty()) m_player->play();
    updatePlayBtnIcon();
}

void MainWindow::onStop()
{
    m_player->stop();
    updatePlayBtnIcon();
}

void MainWindow::onFullscreen()
{
    if (this->isFullScreen()) {
        this->showNormal();
        ui->playlistWidget->show();
        ui->label_playlist->show();
        ui->controlBarContainer->show();
        m_hideTimer->stop();
        this->setCursor(Qt::ArrowCursor);
        ui->horizontalLayout_main->setContentsMargins(15, 15, 15, 15);
    } else {
        ui->playlistWidget->hide();
        ui->label_playlist->hide();
        ui->horizontalLayout_main->setContentsMargins(0, 0, 0, 0); 
        this->showFullScreen();
        ui->controlBarContainer->show();
        this->setCursor(Qt::ArrowCursor);
        m_hideTimer->start();
    }
}

void MainWindow::onVolumeChanged(int volume)
{
    float v = qBound(0.0f, static_cast<float>(volume) / 100.0f, 1.0f);
    m_audioOutput->setVolume(v);
}

void MainWindow::onPositionChanged(qint64 position)
{
    if (!ui->progressSlider->isSliderDown()) ui->progressSlider->setValue(static_cast<int>(position));
    qint64 duration = m_player->duration();
    QTime currentTime = QTime::fromMSecsSinceStartOfDay(qBound(0LL, position, duration));
    QTime totalTime = QTime::fromMSecsSinceStartOfDay(duration);
    QString format = duration >= 3600000 ? "hh:mm:ss" : "mm:ss";
    ui->timeLabel->setText(currentTime.toString(format) + " / " + totalTime.toString(format));
}

void MainWindow::onDurationChanged(qint64 duration)
{
    ui->progressSlider->setRange(0, static_cast<int>(duration));
}

void MainWindow::onSliderMoved(int position)
{
    m_player->setPosition(position);
}

void MainWindow::onPlaylistDoubleClicked(QListWidgetItem *item)
{
    m_currentFile = item->text();
    m_player->setSource(QUrl::fromLocalFile(m_currentFile));
    m_player->play();
    updatePlayBtnIcon();
}

void MainWindow::updatePlayBtnIcon()
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState) ui->playBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    else ui->playBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}
