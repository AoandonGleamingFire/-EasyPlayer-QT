#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QAudioOutput;
class QListWidgetItem;
class QTimer;
class QKeyEvent;
class QMouseEvent;
class QEvent;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void onOpenFile();
    void onPlayPause();
    void onStop();
    void onFullscreen();
    void onVolumeChanged(int volume);
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onSliderMoved(int position);
    void onPlaylistDoubleClicked(QListWidgetItem *item);
    void hideControlBar();

private:
    void updatePlayBtnIcon();

    Ui::MainWindow *ui;
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    QTimer *m_hideTimer;
    QString m_currentFile;
};

#endif // MAINWINDOW_H
