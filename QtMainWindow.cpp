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
    this->setFixedSize(600, 400); // ���ڳߴ������
    this->setWindowTitle("Backup & Restore");

    RadioButton_flag = 0;

    // �����������
    QGroupBox* group = new QGroupBox("Choose an action:", this);
    QRadioButton* backup_b = new QRadioButton("Backup");
    QRadioButton* restore_b = new QRadioButton("Restore");
    backup_b->setChecked(true);

    // ��ȡRadioButtonֵ
    connect(backup_b, &QRadioButton::toggled, [=](bool isChecked) {
        RadioButton_flag = isChecked ? 0 : 1;
        });

    // �����ڲ����֣���ֱ��
    QVBoxLayout* groupLayout = new QVBoxLayout;
    groupLayout->addWidget(backup_b);
    groupLayout->addWidget(restore_b);
    group->setLayout(groupLayout);

    // ������ť
    QPushButton* next_b = new QPushButton("Next");
    QPushButton* close_b = new QPushButton("Close");

    connect(next_b, &QPushButton::clicked, this, &QtMainWindow::next);
    connect(close_b, &QPushButton::clicked, this, &QtMainWindow::close);

    // ��ťˮƽ����
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();        // �ð�ť����
    buttonLayout->addWidget(next_b);
    buttonLayout->addWidget(close_b);

    // ������
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(group);
    mainLayout->addStretch();          // ����հ�����
    mainLayout->addLayout(buttonLayout);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    this->setLayout(mainLayout);

    // ��ʽ�Ż�
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
