#ifndef TARGETFORMDIALOG_H
#define TARGETFORMDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QString>

// Структура формуляра одной цели
struct TargetInfo {
    int number = 0;          // номер цели
    QString name;             // название/метка цели
    double range = 0.0;       // дальность, м
    double azimuth = 0.0;     // азимут, град
    double speed = 0.0;       // скорость, м/с
    double amplitude = 0.0;   // амплитуда отклика
    bool active = true;       // активна ли цель
};

class TargetFormDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TargetFormDialog(QWidget *parent = nullptr);
    ~TargetFormDialog();

private:
    void initTargets();
    void displayTargets();
    QWidget* createTargetCard(const TargetInfo &target);

    QVBoxLayout *contentLayout;
    QWidget *contentWidget;
    QVector<TargetInfo> targetList;
};

#endif // TARGETFORMDIALOG_H