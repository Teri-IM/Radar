#include "targetformdialog.h"
#include <QScrollArea>
#include <QFrame>
#include <QHBoxLayout>

TargetFormDialog::TargetFormDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Формуляры целей");
    resize(460, 420);
    setWindowModality(Qt::WindowModal);
    setStyleSheet("QDialog { background-color: #2c3e50; color: #ffffff; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    QLabel *titleLabel = new QLabel("Формуляры целей", this);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffd93d;");
    mainLayout->addWidget(titleLabel);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("QFrame { color: #5d6d7e; background-color: #5d6d7e; max-height: 2px; }");
    mainLayout->addWidget(line);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background-color: #2c3e50; border: none; }"
                              "QScrollBar:vertical { background-color: #34495e; width: 12px; }"
                              "QScrollBar::handle:vertical { background-color: #5d6d7e; border-radius: 6px; min-height: 20px; }");

    contentWidget = new QWidget();
    contentWidget->setStyleSheet("QWidget { background-color: #2c3e50; }");
    contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setAlignment(Qt::AlignTop);
    contentLayout->setSpacing(8);

    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea, 1);

    initTargets();
    displayTargets();

    QPushButton *okButton = new QPushButton("OK", this);
    okButton->setStyleSheet("QPushButton { background-color: #3498db; color: white; padding: 8px; border-radius: 5px; font-weight: bold; }"
                            "QPushButton:hover { background-color: #2980b9; }");
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(okButton);
}

TargetFormDialog::~TargetFormDialog() {}

void TargetFormDialog::initTargets()
{
    targetList.clear();

    // Цели задаются вручную прямо здесь
    targetList.append({1, "Цель 1", 12000.0, 45.0, 250.0, 1500.0, true});
    targetList.append({2, "Цель 2", 8000.0, 120.0, 0.0, 900.0, true});
    targetList.append({3, "Цель 3", 20000.0, 300.0, 400.0, 1500.0, false});
}

QWidget* TargetFormDialog::createTargetCard(const TargetInfo &target)
{
    QWidget *card = new QWidget(contentWidget);
    card->setStyleSheet(QString("background-color: #34495e; border-radius: 6px; %1")
                         .arg(target.active ? "" : "opacity: 0.6;"));

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(10, 8, 10, 8);
    cardLayout->setSpacing(4);

    QLabel *nameLabel = new QLabel(QString("%1 (%2)").arg(target.name, target.active ? "активна" : "неактивна"));
    nameLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #ffffff;");
    cardLayout->addWidget(nameLabel);

    QHBoxLayout *row1 = new QHBoxLayout();
    QLabel *rangeLabel = new QLabel(QString("Дальность: %1 м").arg(target.range));
    QLabel *azimuthLabel = new QLabel(QString("Азимут: %1°").arg(target.azimuth));
    rangeLabel->setStyleSheet("font-size: 12px; color: #ecf0f1;");
    azimuthLabel->setStyleSheet("font-size: 12px; color: #ecf0f1;");
    row1->addWidget(rangeLabel);
    row1->addWidget(azimuthLabel);
    cardLayout->addLayout(row1);

    QHBoxLayout *row2 = new QHBoxLayout();
    QLabel *speedLabel = new QLabel(QString("Скорость: %1 м/с").arg(target.speed));
    QLabel *ampLabel = new QLabel(QString("Амплитуда: %1").arg(target.amplitude));
    speedLabel->setStyleSheet("font-size: 12px; color: #ecf0f1;");
    ampLabel->setStyleSheet("font-size: 12px; color: #ecf0f1;");
    row2->addWidget(speedLabel);
    row2->addWidget(ampLabel);
    cardLayout->addLayout(row2);

    return card;
}

void TargetFormDialog::displayTargets()
{
    for (const TargetInfo &target : targetList) {
        contentLayout->addWidget(createTargetCard(target));
    }
}