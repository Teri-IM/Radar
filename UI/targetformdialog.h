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
    int number;        // номер цели
    QString name;       // название/метка цели
    double range;       // дальность, м
    double azimuth;     // азимут, град
    double speed;       // скорость, м/с
    double amplitude;   // амплитуда отклика
    bool active;        // активна ли цель

    TargetInfo(int num = 0, const QString &n = QString(), double r = 0.0,
               double az = 0.0, double sp = 0.0, double amp = 0.0, bool act = true)
        : number(num), name(n), range(r), azimuth(az), speed(sp), amplitude(amp), active(act) {}
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
