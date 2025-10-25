#include "QtMainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>

int RadioButton_flag = 0;

QtMainWindow::QtMainWindow(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->setFixedSize(600, 400); // 窗口尺寸更紧凑
    this->setWindowTitle("Backup & Restore");

    RadioButton_flag = 0;

    // 创建分组对象
    QGroupBox* group = new QGroupBox("Choose an action:", this);
    QRadioButton* backup_b = new QRadioButton("Backup");
    QRadioButton* restore_b = new QRadioButton("Restore");
    backup_b->setChecked(true);

    // 获取RadioButton值
    connect(backup_b, &QRadioButton::toggled, [=](bool isChecked) {
        RadioButton_flag = isChecked ? 0 : 1;
        });

    // 分组内部布局（垂直）
    QVBoxLayout* groupLayout = new QVBoxLayout;
    groupLayout->addWidget(backup_b);
    groupLayout->addWidget(restore_b);
    group->setLayout(groupLayout);

    // 创建按钮
    QPushButton* next_b = new QPushButton("Next");
    QPushButton* close_b = new QPushButton("Close");

    connect(next_b, &QPushButton::clicked, this, &QtMainWindow::next);
    connect(close_b, &QPushButton::clicked, this, &QtMainWindow::close);

    // 按钮水平布局
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();        // 让按钮靠右
    buttonLayout->addWidget(next_b);
    buttonLayout->addWidget(close_b);

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(group);
    mainLayout->addStretch();          // 拉伸空白区域
    mainLayout->addLayout(buttonLayout);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    this->setLayout(mainLayout);

    // 样式优化
    this->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 2px solid #6CA6CD;
            border-radius: 8px;
            margin-top: 10px;
            padding: 10px;
        }
        QPushButton {
            background-color: #4A90E2;
            color: white;
            font-size: 14px;
            padding: 6px 16px;
            border-radius: 6px;
        }
        QPushButton:hover {
            background-color: #357ABD;
        }
        QPushButton:pressed {
            background-color: #2E5E9E;
        }
    )");
}

QtMainWindow::~QtMainWindow() {}

void QtMainWindow::next()
{
    if (RadioButton_flag == 0) {
        qDebug() << "backup";
        this->close();
        QtBackupWindow* pic = new QtBackupWindow();
        pic->show();
    }
    else if (RadioButton_flag == 1) {
        qDebug() << "restore";
        this->close();
        QtRestoreWindow* pic = new QtRestoreWindow();
        pic->show();
    }
    else {
        qDebug() << "error";
    }
}
