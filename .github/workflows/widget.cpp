#include "widget.h"
#include "ui_widget.h"

#define TIMEOUT 2000

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    ,m_listModel(new QStandardItemModel(this))
    ,m_downloadModel(new QStandardItemModel(this))
    ,m_mediaPlayer(new QMediaPlayer(this))

{
    ui->setupUi(this);
    //m_listModel = new QStandardItemModel;
    ui->musicListView->setModel(m_listModel);
    ui->downloadedMusicListView->setModel(m_downloadModel);
    t_timePicture = new QTimer;
    connect(m_mediaPlayer, &QMediaPlayer::stateChanged, this, &Widget::onMediaStateChanged);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &Widget::updatePosition);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &Widget::updateDuration);
    connect(ui->progressSlider, &QSlider::sliderMoved, this, &Widget::onProgressSliderMoved);
    connect(t_timePicture, &QTimer::timeout, this, &Widget::timePictureHandler);

//    picID = 2;
//    QImage img;
//    img.load("C:\\Users\\29311\\Desktop\\pht\\1.jpg");
//    ui->label_5->setPixmap(QPixmap::fromImage(img));

    initVolumeTimer();
    initVolumeSlider();
    initTimer();
    styleButton_set();
    ui->volumeSlider->setVisible(false);
    ui->musicListView->setVisible(false);
    t_timePicture->start(TIMEOUT);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_openButton_clicked()
{
    auto path = QFileDialog::getExistingDirectory(this,"选择文件夹","D:/NiuGensheng/Cmusicdownload");
        if(path.isEmpty()) return;

        QDirIterator it(path,{"*.mp3","*.wav","*.ogg"});
        while(it.hasNext())
        {
           it.next();
           auto info = it.fileInfo();
           auto item = new QStandardItem(info.fileName());
           // ========== 核心修复2：统一用Qt::UserRole存储路径（避免Role不匹配） ==========
           item->setData(info.canonicalFilePath(), Qt::UserRole);
           m_listModel->appendRow(item);
        }
}

void Widget::on_musicListView_doubleClicked(const QModelIndex &index)
{
    // ========== 核心修复3：用Qt::UserRole读取路径（和存储时一致） ==========
        QString musicPath = index.data(Qt::UserRole).toString();
        if (musicPath.isEmpty()) {
            qDebug() << "读取音乐路径失败！";
            return;
        }
        // 正确设置本地文件路径（必须用QUrl::fromLocalFile，否则路径解析失败）
        m_mediaPlayer->setMedia(QUrl::fromLocalFile(musicPath));
        m_mediaPlayer->play();
        // ========== 核心修复4：双击时更新m_currentIndex（确保索引同步） ==========
        m_currentIndex = index.row();
        qDebug() << "当前播放索引：" << m_currentIndex << "，路径：" << musicPath;
}

void Widget::onMediaStateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::PlayingState) {
           qDebug() << "开始播放";
       } else if (state == QMediaPlayer::PausedState) {
           qDebug() << "暂停播放";
       } else if (state == QMediaPlayer::StoppedState) {
           qDebug() << "停止播放";
       }
}

void Widget::on_pauseButton_clicked()
{
        m_mediaPlayer->pause();
}

void Widget::on_playButton_clicked()
{
    m_mediaPlayer->play();
}

void Widget::updatePosition(qint64 position)
{
    if (!ui->progressSlider->isSliderDown()) {  // 避免拖动冲突
            ui->progressSlider->setValue(position);
        }

        // 更新时间显示
        QTime currentTime(0, 0, 0);
        currentTime = currentTime.addMSecs(position);
        ui->label_2->setText(currentTime.toString("mm:ss"));
}

void Widget::updateDuration(qint64 duration)
{
    ui->progressSlider->setRange(0, duration);

        // 更新总时长显示
        QTime totalTime(0, 0, 0);
        totalTime = totalTime.addMSecs(duration);
        ui->label_3->setText(totalTime.toString("mm:ss"));
}

void Widget::playByIndex(int index)
{
    if (index < 0 || index >= m_listModel->rowCount()) {
        return;
    }

    m_currentIndex = index;
    QModelIndex modelIndex = m_listModel->index(index, 0);
    on_musicListView_doubleClicked(modelIndex);

    // 高亮当前播放的歌曲
    ui->musicListView->setCurrentIndex(modelIndex);
}
void Widget::onProgressSliderMoved(int position)
{
     m_mediaPlayer->setPosition(position);
}


void Widget::on_previousButton_clicked()
{
    if (m_listModel->rowCount() == 0) return;

       int prevIndex = m_currentIndex - 1;
       if (prevIndex < 0) {
           prevIndex = m_listModel->rowCount() - 1; // 循环播放
       }
       playByIndex(prevIndex);
}

void Widget::on_nextButton_clicked()
{
    if (m_listModel->rowCount() == 0) return;

        int nextIndex = m_currentIndex + 1;
        if (nextIndex >= m_listModel->rowCount()) {
            nextIndex = 0; // 循环播放
        }
        playByIndex(nextIndex);
}

void Widget::on_volumeSlider_valueChanged(int value)
{
    m_mediaPlayer->setVolume(value);


}

void Widget::on_volumeButton_clicked()
{
//    flag_volumeVisible = !flag_volumeVisible;
//    if(true == flag_volumeVisible)
//    {ui->volumeSlider->setVisible(true);}

//    else
//    {ui->volumeSlider->setVisible(false);}

}

void Widget::styleButton_set()
{
    ui->previousButton->setStyleSheet("QPushButton{image: url(:/last.png);background-color: rgb(153, 186, 255);border:2px groove gray;border-radius:10px;padding:2px 4px;border-style: outset;}"
                                               "QPushButton:hover{background-color:rgb(229, 241, 251); color: black;}"
                                               "QPushButton:pressed{background-color:rgb(204, 228, 247);border-style: inset;}");
    ui->playButton->setStyleSheet("QPushButton{image: url(:/play.png);background-color: rgb(153, 186, 255);border:2px groove gray;border-radius:10px;padding:2px 4px;border-style: outset;}"
                                               "QPushButton:hover{background-color:rgb(229, 241, 251); color: black;}"
                                               "QPushButton:pressed{background-color:rgb(204, 228, 247);border-style: inset;}");
    ui->nextButton->setStyleSheet("QPushButton{image: url(:/next.png);background-color: rgb(153, 186, 255);border:2px groove gray;border-radius:10px;padding:2px 4px;border-style: outset;}"
                                               "QPushButton:hover{background-color:rgb(229, 241, 251); color: black;}"
                                               "QPushButton:pressed{background-color:rgb(204, 228, 247);border-style: inset;}");
    ui->pauseButton->setStyleSheet("QPushButton{image: url(:/pause.png);background-color: rgb(153, 186, 255);border:2px groove gray;border-radius:10px;padding:2px 4px;border-style: outset;}"
                                               "QPushButton:hover{background-color:rgb(229, 241, 251); color: black;}"
                                               "QPushButton:pressed{background-color:rgb(204, 228, 247);border-style: inset;}");
    ui->openButton->setStyleSheet("QPushButton{image: url(:/Sequential.png);background-color: rgb(153, 186, 255);border:2px groove gray;border-radius:10px;padding:2px 4px;border-style: outset;}"
                                               "QPushButton:hover{background-color:rgb(229, 241, 251); color: black;}"
                                               "QPushButton:pressed{background-color:rgb(204, 228, 247);border-style: inset;}");
    ui->volumeButton->setStyleSheet("QPushButton{image: url(:/volume.png);background-color: rgb(153, 186, 255);border:2px groove gray;border-radius:10px;padding:2px 4px;border-style: outset;}"
                                               "QPushButton:hover{background-color:rgb(229, 241, 251); color: black;}"
                                    "QPushButton:pressed{background-color:rgb(204, 228, 247);border-style: inset;}");
}

void Widget::initTimer()
{
    QTimer *hoverCheckTimer = new QTimer(this);
    hoverCheckTimer->setInterval(50); // 50ms检测一次（按需调整）
    connect(hoverCheckTimer, &QTimer::timeout, this, [=]() {
        // 1. 获取鼠标位置，分别判断按钮和列表的悬浮状态
        QPoint globalPos = QCursor::pos();
        // 按钮悬浮判断（仅这一步能触发列表显示）
        QPoint btnLocalPos = ui->openButton->mapFromGlobal(globalPos);
        bool isBtnHovered = ui->openButton->rect().contains(btnLocalPos);
        // 列表悬浮判断（仅用于保持显示，不触发显示）
        QPoint listLocalPos = ui->musicListView->mapFromGlobal(globalPos);
        bool isListHovered = ui->musicListView->rect().contains(listLocalPos);

        // ========== 核心逻辑 ==========
        if (!m_isListBtnHovered) {
            // 列表当前隐藏：仅鼠标在按钮上时，才显示列表
            if (isBtnHovered) {
                m_isListBtnHovered = true;
                ui->musicListView->show();
                qDebug() << "鼠标进入按钮 → 显示列表";
            }
            // 列表隐藏时，鼠标在列表区域 → 不处理（列表仍隐藏）
        } else {
            // 列表当前显示：只要鼠标在按钮 或 列表上 → 保持显示；否则隐藏
            bool isMouseOnTarget = isBtnHovered || isListHovered;
            if (!isMouseOnTarget) {
                m_isListBtnHovered = false;
                ui->musicListView->hide();
                qDebug() << "鼠标离开按钮和列表 → 隐藏列表";
            }
            // 鼠标在按钮/列表上 → 不处理（列表保持显示）
        }
    });
    hoverCheckTimer->start(); // 必须启动定时器！
}
void Widget::initVolumeTimer()
{
    QTimer *hoverCheckTimer1 = new QTimer(this);
    hoverCheckTimer1->setInterval(50); // 50ms检测一次（按需调整）
    connect(hoverCheckTimer1, &QTimer::timeout, this, [=]() {
        // 1. 获取鼠标位置，分别判断按钮和列表的悬浮状态
        QPoint globalPos = QCursor::pos();
        // 按钮悬浮判断（仅这一步能触发列表显示）
        QPoint btnLocalPos1 = ui->volumeButton->mapFromGlobal(globalPos);
        bool isBtnHovered1 = ui->volumeButton->rect().contains(btnLocalPos1);
        // 列表悬浮判断（仅用于保持显示，不触发显示）
        QPoint listLocalPos1 = ui->volumeSlider->mapFromGlobal(globalPos);
        bool isListHovered1 = ui->volumeSlider->rect().contains(listLocalPos1);

        // ========== 核心逻辑 ==========
        if (!m_isListBtnHovered1) {
            // 列表当前隐藏：仅鼠标在按钮上时，才显示列表
            if (isBtnHovered1) {
                m_isListBtnHovered1 = true;
                ui->volumeSlider->show();
                qDebug() << "鼠标进入按钮 → 显示列表";
            }
            // 列表隐藏时，鼠标在列表区域 → 不处理（列表仍隐藏）
        } else {
            // 列表当前显示：只要鼠标在按钮 或 列表上 → 保持显示；否则隐藏
            bool isMouseOnTarget1 = isBtnHovered1 || isListHovered1;
            if (!isMouseOnTarget1) {
                m_isListBtnHovered1 = false;
                ui->volumeSlider->hide();
                qDebug() << "鼠标离开按钮和列表 → 隐藏列表";
            }
            // 鼠标在按钮/列表上 → 不处理（列表保持显示）
        }
    });
    hoverCheckTimer1->start(); // 必须启动定时器！
}

void Widget::initVolumeSlider()
{
    //滑块操作
    ui->volumeSlider->setRange(0, 100);          // 音量范围0-100
    ui->volumeSlider->setValue(50);              // 默认音量50%
    m_mediaPlayer->setVolume(50);                // 设置播放器初始音量
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &Widget::on_volumeSlider_valueChanged);
    ui->progressSlider->setRange(0, 0);
}

void Widget::timePictureHandler()
{


//    QString path("C:\\Users\\29311\\Desktop\\pht\\");
//    path += QString::number(picID);
//    path += ".jpg";

//    QImage img;
//    img.load(path);
//    ui->label_5->setPixmap(QPixmap::fromImage(img));
//     picID++;
//    if(5 == picID)
//        picID = 1;
}



void Widget::on_downloadButton_clicked()
{

    // 1. 检查当前索引是否有效
        if (m_currentIndex == -1 || m_listModel->rowCount() == 0) {
            qDebug() << "错误：无当前播放的音乐（索引无效）";
            return;
        }

        // 2. 获取当前播放项的索引和数据（用统一的Qt::UserRole）
        QModelIndex currentIndex = m_listModel->index(m_currentIndex, 0);
        QString musicName = currentIndex.data(Qt::DisplayRole).toString(); // 音乐名称
        QString musicPath = currentIndex.data(Qt::UserRole).toString();    // 音乐路径

        // 3. 校验数据是否为空
        if (musicName.isEmpty() || musicPath.isEmpty()) {
            qDebug() << "错误：音乐名称/路径为空";
            return;
        }

        // 4. 检查是否已添加（避免重复）
        bool isExist = false;
        for (int i = 0; i < m_downloadModel->rowCount(); ++i) {
            QString existPath = m_downloadModel->item(i)->data(Qt::UserRole).toString();
            if (existPath == musicPath) {
                isExist = true;
                break;
            }
        }
        if (isExist) {
            qDebug() << "该音乐已添加：" << musicName;
            return;
        }

        // 5. 添加到新列表（同样用Qt::UserRole存储路径）
        QStandardItem *newItem = new QStandardItem(musicName);
        newItem->setData(musicPath, Qt::UserRole);
        m_downloadModel->appendRow(newItem);

        qDebug() << "成功添加到新列表：" << musicName << " | 路径：" << musicPath;
    }
void Widget::on_downloadedMusicListView_doubleClicked(const QModelIndex &index)
{
    QString musicPath = index.data(Qt::UserRole).toString();
    if (musicPath.isEmpty()) return;

    m_mediaPlayer->setMedia(QUrl::fromLocalFile(musicPath));
    m_mediaPlayer->play();

    // 同步更新原列表的当前索引（可选）
    for (int i = 0; i < m_listModel->rowCount(); ++i) {
        QModelIndex modelIndex = m_listModel->index(i, 0);
        if (modelIndex.data(Qt::UserRole).toString() == musicPath) {
            m_currentIndex = i;
            ui->musicListView->setCurrentIndex(modelIndex);
            break;
        }
    }
}
