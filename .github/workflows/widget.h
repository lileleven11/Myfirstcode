#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QDirIterator>
#include <QDebug>
#include <QStandardItemModel>
#include <QMediaPlayer> //媒体播放器
#include <QTimer>
#include <QTime>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_openButton_clicked();

    void on_musicListView_doubleClicked(const QModelIndex &index);

     void onMediaStateChanged(QMediaPlayer::State state);  // 新增槽函数

     void on_pauseButton_clicked();

     void on_playButton_clicked();
     void updatePosition(qint64 position);
     void updateDuration(qint64 duration);
     void onProgressSliderMoved(int position);
     void playByIndex(int index); // 按索引播放
     void on_previousButton_clicked();
     void on_nextButton_clicked();
      void on_volumeSlider_valueChanged(int value); // 音量滑块调节

      void on_volumeButton_clicked();
      void styleButton_set();
      void initTimer();
      void initVolumeTimer();
      void initVolumeSlider();
      void timePictureHandler();


      void on_downloadButton_clicked();
      void on_downloadedMusicListView_doubleClicked(const QModelIndex &index);

private:
    Ui::Widget *ui;
    QStandardItemModel *m_listModel;
    QStandardItemModel *m_downloadModel;   // 新增：已下载/收藏列表模型
    QMediaPlayer *m_mediaPlayer{};
    QTimer *t_timePicture;
    int m_currentIndex = -1; // 当前播放的歌曲索引
    int picID;
    bool flag_volumeVisible = false;
    bool m_isListBtnHovered = false;
    bool m_isListBtnHovered1 = false;

};
#endif // WIDGET_H
