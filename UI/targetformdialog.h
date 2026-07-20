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

// Единая точка добавления/редактирования целей.
// Чтобы завести новую цель — просто добавьте ещё один TargetInfo(...) в список ниже,
// больше нигде в коде их прописывать не нужно.
inline QVector<TargetInfo> makeDefaultTargets()
{
    return {
        TargetInfo(1,  "target1",12000.0,  45.0, 250.0, 1500.0, true),
        TargetInfo(2,  "target2",8000.0, 120.0,   0.0,  900.0, true),
        TargetInfo(3,  "target3",20000.0,300.0, 400.0, 1500.0, false),
        TargetInfo(4,  "target4",15500.0,  10.0, 220.0, 1100.0, true),
        TargetInfo(5,  "target5",9600.0, 200.0, 900.0, 2000.0, true),
        TargetInfo(6,  "target6",30000.0,  75.0, 230.0,  700.0, true),
        TargetInfo(7,  "target7",6000.0, 330.0, 150.0,  600.0, false),
        TargetInfo(8,  "target8",11000.0,  95.0, 260.0, 1600.0, true),
        TargetInfo(9,  "target9",4000.0, 160.0,  60.0,  400.0, true),
        TargetInfo(10, "target10", 17000.0, 210.0,  25.0,  500.0, false),
        TargetInfo(11, "target11", 28000.0, 55.0, 240.0, 1900.0, true),
        TargetInfo(12, "target12", 9000.0, 280.0, 250.0,  300.0, true)
    };
}

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
