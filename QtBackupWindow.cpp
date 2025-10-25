#include "QtBackupWindow.h"
#include "QtMainWindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QScrollArea>
#include <QIntValidator>
#include <QDateTimeEdit>
#include <QTimeEdit>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>

// 全局变量
std::string target_folder;
std::vector<std::string> cho;
int mode_flag = 1;
int aes_flag = 1;
int is_scheduled = 0;
int scheduled_mode = 0;

QtBackupWindow::QtBackupWindow(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->setFixedSize(600, 600);
    this->setWindowTitle("Backup Settings");

    // ================= 文件夹选择区域 =================
    QLabel* folderLabel = new QLabel("Save to folder:");
    folderLabel->setStyleSheet("font-weight: bold;");

    QLabel* folderPath = new QLabel("");
    folderPath->setFrameShape(QFrame::Box);
    folderPath->setMinimumHeight(25);
    QPushButton* folderBtn = new QPushButton("Choose Folder");
    connect(folderBtn, &QPushButton::clicked, this, [=]() {
        target_folder = CSelectFolderDlg::Show();
        folderPath->setText(QString::fromStdString(target_folder));
        });

    QHBoxLayout* folderLayout = new QHBoxLayout;
    folderLayout->addWidget(folderLabel);
    folderLayout->addWidget(folderPath);
    folderLayout->addWidget(folderBtn);

    // ================= 文件选择区域 =================
    QLabel* filesLabel = new QLabel("Files selected:");
    filesLabel->setStyleSheet("font-weight: bold;");

    QLabel* filesShow = new QLabel("");
    filesShow->setWordWrap(true);
    filesShow->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QScrollArea* filesScroll = new QScrollArea;
    filesScroll->setWidgetResizable(true);
    filesScroll->setWidget(filesShow);
    filesScroll->setMinimumHeight(80);

    QPushButton* filesBtn = new QPushButton("Choose Files");
    connect(filesBtn, &QPushButton::clicked, this, [=]() {
        cho = chooseFiles();
        QString all;
        for (auto& s : cho)
            all += QString::fromStdString(s) + "\n";
        filesShow->setText(all);
        });

    QVBoxLayout* filesLayout = new QVBoxLayout;
    filesLayout->addWidget(filesLabel);
    filesLayout->addWidget(filesScroll);
    filesLayout->addWidget(filesBtn, 0, Qt::AlignRight);

    // ================= 打包模式选择 =================
    QGroupBox* packGroup = new QGroupBox("Backup Mode");
    QVBoxLayout* packLayout = new QVBoxLayout;

    QRadioButton* mode1 = new QRadioButton("Only back up");
    QRadioButton* mode2 = new QRadioButton(".tar");
    QRadioButton* mode3 = new QRadioButton(".tar.lz");
    QRadioButton* mode4 = new QRadioButton(".tar.aes");
    QRadioButton* mode5 = new QRadioButton(".tar.lz.aes");
    mode1->setChecked(true);

    packLayout->addWidget(mode1);
    packLayout->addWidget(mode2);
    packLayout->addWidget(mode3);
    packLayout->addWidget(mode4);
    packLayout->addWidget(mode5);
    packGroup->setLayout(packLayout);

    // ================= AES 密钥输入区 =================
    QLabel* aesKeyLabel = new QLabel("Please set AES key:");
    aesKeyLabel->setVisible(false);
    QLineEdit* aesKeyEdit = new QLineEdit;
    aesKeyEdit->setEchoMode(QLineEdit::Password);
    aesKeyEdit->setVisible(false);

    QGroupBox* aesModeGroup = new QGroupBox("AES Key Mode");
    QHBoxLayout* aesModeLayout = new QHBoxLayout;
    QRadioButton* aes128 = new QRadioButton("AES128");
    QRadioButton* aes192 = new QRadioButton("AES192");
    QRadioButton* aes256 = new QRadioButton("AES256");
    aes128->setChecked(true);
    aesModeLayout->addWidget(aes128);
    aesModeLayout->addWidget(aes192);
    aesModeLayout->addWidget(aes256);
    aesModeGroup->setLayout(aesModeLayout);
    aesModeGroup->setVisible(false);

    // 模式按钮联动逻辑
    auto hideAES = [=]() {
        aesKeyLabel->setVisible(false);
        aesKeyEdit->setVisible(false);
        aesModeGroup->setVisible(false);
        };
    auto showAES = [=]() {
        aesKeyLabel->setVisible(true);
        aesKeyEdit->setVisible(true);
        aesModeGroup->setVisible(true);
        };
    connect(mode1, &QRadioButton::toggled, this, [=](bool on) { if (on) { mode_flag = 1; hideAES(); } });
    connect(mode2, &QRadioButton::toggled, this, [=](bool on) { if (on) { mode_flag = 2; hideAES(); } });
    connect(mode3, &QRadioButton::toggled, this, [=](bool on) { if (on) { mode_flag = 3; hideAES(); } });
    connect(mode4, &QRadioButton::toggled, this, [=](bool on) { if (on) { mode_flag = 4; showAES(); } });
    connect(mode5, &QRadioButton::toggled, this, [=](bool on) { if (on) { mode_flag = 5; showAES(); } });

    connect(aes128, &QRadioButton::toggled, [=](bool on) { if (on) aes_flag = 1; });
    connect(aes192, &QRadioButton::toggled, [=](bool on) { if (on) aes_flag = 2; });
    connect(aes256, &QRadioButton::toggled, [=](bool on) { if (on) aes_flag = 3; });

    // ================= 定时任务 =================
    QGroupBox* scheduleGroup = new QGroupBox("Scheduled Backup");
    scheduleGroup->setCheckable(true);
    scheduleGroup->setChecked(false);
    QRadioButton* onceBtn = new QRadioButton("Only Once");
    QRadioButton* repeatBtn = new QRadioButton("Repeat");
    QVBoxLayout* scheduleLayout = new QVBoxLayout;
    scheduleLayout->addWidget(onceBtn);
    scheduleLayout->addWidget(repeatBtn);
    scheduleGroup->setLayout(scheduleLayout);

    QLabel* onceLabel = new QLabel("Please select date/time:");
    QDateTimeEdit* datetimeEdit = new QDateTimeEdit(QDateTime::currentDateTime());
    datetimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    onceLabel->setVisible(false);
    datetimeEdit->setVisible(false);

    QLabel* repeatLabel = new QLabel("Repeat every:");
    QLineEdit* dayEdit = new QLineEdit("0");
    dayEdit->setValidator(new QIntValidator);
    QLabel* dayLabel = new QLabel("days");
    QTimeEdit* timeEdit = new QTimeEdit;
    QLabel* timeLabel = new QLabel("hh:mm:ss");
    repeatLabel->setVisible(false);
    dayEdit->setVisible(false);
    dayLabel->setVisible(false);
    timeEdit->setVisible(false);
    timeLabel->setVisible(false);

    // 逻辑控制
    connect(scheduleGroup, &QGroupBox::toggled, this, [=](bool on) {
        is_scheduled = on ? 1 : 0;
        if (on) {
            onceBtn->setChecked(true);
            onceLabel->setVisible(true);
            datetimeEdit->setVisible(true);
        }
        else {
            onceLabel->setVisible(false);
            datetimeEdit->setVisible(false);
            repeatLabel->setVisible(false);
            dayEdit->setVisible(false);
            dayLabel->setVisible(false);
            timeEdit->setVisible(false);
            timeLabel->setVisible(false);
        }
        });

    connect(onceBtn, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            scheduled_mode = 0;
            onceLabel->setVisible(true);
            datetimeEdit->setVisible(true);
            repeatLabel->setVisible(false);
            dayEdit->setVisible(false);
            dayLabel->setVisible(false);
            timeEdit->setVisible(false);
            timeLabel->setVisible(false);
        }
        });

    connect(repeatBtn, &QRadioButton::toggled, this, [=](bool on) {
        if (on) {
            scheduled_mode = 1;
            onceLabel->setVisible(false);
            datetimeEdit->setVisible(false);
            repeatLabel->setVisible(true);
            dayEdit->setVisible(true);
            dayLabel->setVisible(true);
            timeEdit->setVisible(true);
            timeLabel->setVisible(true);
        }
        });

    // ================= 底部按钮 =================
    QPushButton* packBtn = new QPushButton("Pack Up");
    QPushButton* backBtn = new QPushButton("Back");
    QPushButton* closeBtn = new QPushButton("Close");

    QHBoxLayout* bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->addWidget(packBtn);
    bottomLayout->addWidget(backBtn);
    bottomLayout->addWidget(closeBtn);

    // ================= 主布局 =================
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(folderLayout);
    mainLayout->addLayout(filesLayout);
    mainLayout->addWidget(packGroup);
    mainLayout->addWidget(aesKeyLabel);
    mainLayout->addWidget(aesKeyEdit);
    mainLayout->addWidget(aesModeGroup);
    mainLayout->addWidget(scheduleGroup);
    mainLayout->addWidget(onceLabel);
    mainLayout->addWidget(datetimeEdit);
    mainLayout->addWidget(repeatLabel);
    QHBoxLayout* repeatLine = new QHBoxLayout;
    repeatLine->addWidget(dayEdit);
    repeatLine->addWidget(dayLabel);
    repeatLine->addWidget(timeEdit);
    repeatLine->addWidget(timeLabel);
    mainLayout->addLayout(repeatLine);
    mainLayout->addStretch();
    mainLayout->addLayout(bottomLayout);

    this->setLayout(mainLayout);

    // ================= 按钮逻辑 =================
    connect(packBtn, &QPushButton::clicked, this, &QtBackupWindow::excute);
    connect(backBtn, &QPushButton::clicked, this, &QtBackupWindow::back);
    connect(closeBtn, &QPushButton::clicked, this, &QtBackupWindow::close);
}


void QtBackupWindow::excute() {
    // void setEchoMode(EchoMode)
   

    QString name = key_txt->text();
    std::string s = name.toStdString();
    const char *key = s.c_str();
    // qtimer = new QTimerEvent();


    if (is_scheduled == 0) {
        my_packup(target_folder, cho, key, aes_flag, mode_flag);
    }
    else
    {
        if (scheduled_mode == 0)
        {
            QDateTime dateTime1 = QDateTime::currentDateTime(); 
            QDateTime dateTime2 = datetimeEdit->dateTime();
            int seconds = dateTime1.secsTo(dateTime2);
            seconds += 1;
            qDebug() << dateTime1<<" "<<dateTime2 <<" "<<seconds<<"\n";
            if (seconds > 0) {
                qDebug() << target_folder << " " << key << " " << aes_flag << " " << mode_flag << "\n";
                qtimer = new QTimer();
                qtimer->singleShot(seconds*1000, this, [=] {
                    //想要执行的代码
                    my_packup(target_folder, cho, key, aes_flag, mode_flag);
                    });
            }
        }
        else
        {
            
            QString dTime_QS = dayEdit->text();
            std::string dTime_s = dTime_QS.toStdString();
            int dTime = atoi(dTime_s.c_str());
            int htime = timeEdit->time().hour(), mtime = timeEdit->time().minute(), stime = timeEdit->time().second();
            int seconds = (int)(86400*dTime * +3600 * htime + 60 * mtime + stime);
            qDebug() <<dTime << " " << htime << " " << mtime<<" "<<stime<<" "<<seconds << "\n";
            if (seconds > 0) {
                // qDebug() << target_folder << " " << key << " " << aes_flag << " " << mode_flag << "\n";
                timer_id = startTimer(seconds*1000);
            }
        }
    }
 
 

}

void QtBackupWindow::back() {
    this->close();
    QtMainWindow* pic = new QtMainWindow();
    pic->show();
}

void QtBackupWindow::timerEvent(QTimerEvent *event) {
    if (event->timerId() == timer_id) {
        QString name = key_txt->text();
        std::string s = name.toStdString();
        const char* key = s.c_str();
        my_packup(target_folder, cho, key, aes_flag, mode_flag);
    }
}

QtBackupWindow::~QtBackupWindow()
{}

