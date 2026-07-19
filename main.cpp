

#include <QApplication>
#include <QLabel>
#include <QTimer>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QWidget>
#include <QFont>
#include <cmath>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QElapsedTimer>
#include <QMap>
#include <atomic>
#include <vector>
#include "UI/paramdialog.h"
// Подключаем Си-интерфейсы
extern "C" {
#include "Imitator/Imitator.h"
#include "Imitator/UnifiedImitatorParam.h"

// Предполагаемый прототип вашего обработчика (подставьте ваш реальный, если имена отличаются)
// void Handler(struct HandlerParametrs *params, struct ImitOutData *inData, struct HandlerOutData *outData);
}

// =========================================================================
// ГЛОБАЛЬНЫЕ ДАННЫЕ СИНХРОНИЗАЦИИ ПОТОКОВ (Для разделения GUI и математики)
// =========================================================================
QMutex g_dataMutex;          // Мьютекс для защиты общих данных РЛС
double g_sharedAngle = 0.0;   // Актуальный угол, передаваемый из Си-модели в GUI

struct SharedTargetPoint {
    double angle;
    double distance;
    double amplitude;
};

std::vector<SharedTargetPoint> g_sharedTargets;

// Флаги управления (атомарные, так как читаются из разных потоков)
std::atomic<bool> g_isSimulationRunning{false};
std::atomic<bool> g_isRotating{true};
std::atomic<bool> g_isRadiationOn{false};
std::atomic<bool> g_threadShouldStop{false};

// =========================================================================
// ПОТОК ВЫЧИСЛЕНИЙ РЛС (Выполняет тяжелую Си-математику)
// =========================================================================
class RadarComputeWorker : public QThread {
public:
    struct ImitatorParametrs *simParams;
    struct ImitOutData *simOutput;
    int tickRate;

    RadarComputeWorker(struct ImitatorParametrs *p, struct ImitOutData *out, int rate)
        : simParams(p), simOutput(out), tickRate(rate) {}

protected:
    void run() override {
        while (!g_threadShouldStop.load()) {
            if (!g_isSimulationRunning.load()) {
                // Если симуляция на паузе, спим 5 мс, чтобы не грузить ядро процессора пустым циклом
                msleep(5);
                continue;
            }

            // Синхронизируем интерактивные флаги управления перед шагом модели
            simParams->azimuth->angularVelocity = g_isRotating.load() ? 10.0 : 0.0;
            simParams->targetFormation->enable = g_isRadiationOn.load() ? 1 : 0;
            simParams->clutterFormation->enable = g_isRadiationOn.load() ? 1 : 0;

            // Вызов Си-имитатора (точка долгой задержки вычислений)
            // Теперь выполняется в отдельном системном потоке!
            Imitator(simParams, simOutput);

            // Защищаем критическую секцию обновления данных для графики
            g_dataMutex.lock();
            if (simOutput->AzimuthData != nullptr) {
                g_sharedAngle = simOutput->AzimuthData->azimuth_new;
                simParams->azimuth->startAngle = g_sharedAngle;
            }

            g_sharedTargets.clear();
            if (simOutput->TargetPositionData != nullptr && simOutput->TargetPositionData->Target_map != nullptr) {
                for (int i = 0; i < simOutput->TargetPositionData->cntTarget; ++i) {
                    const struct Point &target = simOutput->TargetPositionData->Target_map[i];
                    if (target.amplitude > 0.0f) {
                        g_sharedTargets.push_back({target.angle, target.distance, target.amplitude});
                    }
                }
            }
            g_dataMutex.unlock();

            // Небольшой сон после шага, чтобы не полностью блокировать планировщик
            // и не "съедать" весь доступный ресурс у GUI-потока.
            msleep(1);
        }
    }
};

// =========================================================================
// ОСНОВНАЯ ФУНКЦИЯ ПРИЛОЖЕНИЯ
// =========================================================================
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Индикатор кругового обзора (Многопоточный режим)");
    window.setMinimumSize(500, 300);
    window.setStyleSheet("background-color: black;");

    QHBoxLayout *mainLayout = new QHBoxLayout(&window);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);
    QLabel *radarLabel = new QLabel(&window);
    radarLabel->setStyleSheet("background-color: black;");
    radarLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Панель управления
    QVBoxLayout *controlLayout = new QVBoxLayout;
    QLabel *controlLabel = new QLabel("Управление", &window);
    controlLabel->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    controlLabel->setAlignment(Qt::AlignHCenter);

    QString buttonStyle = "font-size: 14px; padding: 8px; color: white; background-color: #303030; border: 1px solid #4a4a4a;";

    QPushButton *simButton = new QPushButton("Запуск моделирования", &window);
    simButton->setMinimumSize(160, 44);
    simButton->setStyleSheet(buttonStyle);

    QPushButton *rotationButton = new QPushButton("Остановить вращение", &window);
    rotationButton->setMinimumSize(160, 44);
    rotationButton->setStyleSheet(buttonStyle);

    QPushButton *radButton = new QPushButton("Включить излучение", &window);
    radButton->setMinimumSize(160, 44);
    radButton->setStyleSheet(buttonStyle);

    QPushButton *paramButton = new QPushButton("Настройка параметров", &window);
    paramButton->setMinimumSize(160, 44);
    paramButton->setStyleSheet("font-size: 14px; padding: 8px; color: white; background-color: #2980b9; border: 1px solid #3498db;");

    controlLayout->addWidget(controlLabel);
    controlLayout->addWidget(simButton);
    controlLayout->addWidget(rotationButton);
    controlLayout->addWidget(radButton);
    controlLayout->addWidget(paramButton);
    controlLayout->addStretch(1);

    QWidget *controlWidget = new QWidget(&window);
    controlWidget->setLayout(controlLayout);
    controlWidget->setMinimumWidth(200);
    controlWidget->setStyleSheet("background-color: #101010; border: 1px solid #2a2a2a;");

    mainLayout->addWidget(radarLabel, 3);
    mainLayout->addWidget(controlWidget, 1);

    // Выделение памяти под Си-структуры параметров
    struct ImitatorParametrs *simParams = (struct ImitatorParametrs *)malloc(sizeof(struct ImitatorParametrs));
    simParams->imitator = (struct ImitParam *)malloc(sizeof(struct ImitParam));
    simParams->imitator->maxDistance = 100000.0;
    simParams->uTime = (struct UTimeParam *)malloc(sizeof(struct UTimeParam));
    simParams->uTime->probing_time = 10000;
    simParams->uTime->pulse_time = 6000;
    simParams->uTime->max_sampling_cnt = 10000000;
    simParams->uTime->sampling_rate = 2000;
    simParams->azimuth = (struct AzimutParam *)malloc(sizeof(struct AzimutParam));
    simParams->azimuth->startAngle = 0.0;
    simParams->azimuth->angularVelocity = 10.0;
    simParams->ppPosition = (struct PPPosParam *)malloc(sizeof(struct PPPosParam));
    simParams->ppPosition->cntPP = 3;
    simParams->ppPosition->enable = 1;
    simParams->ppPosition->PPamplitude = 500.0;
    simParams->clutterResponse = (struct ClutterResponseParams *)malloc(sizeof(struct ClutterResponseParams));
    simParams->clutterResponse->enable = 1;
    simParams->clutterFormation = (struct ClutterFormationParam *)malloc(sizeof(struct ClutterFormationParam));
    simParams->clutterFormation->enable = 1;
    simParams->targetPosition = (struct TargetPosParam *)malloc(sizeof(struct TargetPosParam));
    simParams->targetPosition->cntTarget = 3;
    simParams->targetPosition->enable = 1;
    simParams->targetPosition->Targetamplitude = 1500.0;
    simParams->targetFormation = (struct TargetFormationParam *)malloc(sizeof(struct TargetFormationParam));
    simParams->targetFormation->enable = 1;
    simParams->targetResponse = (struct TargetResponseParams *)malloc(sizeof(struct TargetResponseParams));
    simParams->targetResponse->enable = 1;
    simParams->nipPosition = (struct NIPPosParam *)malloc(sizeof(struct NIPPosParam));
    simParams->nipPosition->cntNIP = 3;
    simParams->nipPosition->enable = 1;
    simParams->nipPosition->NIPamplitude = 600.0;
    simParams->nipLevel = (struct NIPLvlParam *)malloc(sizeof(struct NIPLvlParam));
    simParams->nipLevel->enable = 1;
    simParams->nipLevel->amplitudeDecrease = 10.0;
    simParams->nipFormation = (struct NIPFormationParam *)malloc(sizeof(struct NIPFormationParam));
    simParams->nipFormation->enable = 1;
    simParams->summator = (struct SummatorParam *)malloc(sizeof(struct SummatorParam));
    simParams->summator->enable = 1;
    simParams->frequencyConverter = (struct FreqConvertorParam *)malloc(sizeof(struct FreqConvertorParam));
    simParams->frequencyConverter->enable = 1;
    simParams->noise = (struct NoiseParam *)malloc(sizeof(struct NoiseParam));
    simParams->noise->enable = 0;
    simParams->noise->mean = 20;
    simParams->noise->sigma = 15;

    struct ImitOutData *simOutput = (struct ImitOutData *)calloc(1, sizeof(struct ImitOutData));
    simOutput->TimeData = (struct UnifedTimeOut *)calloc(1, sizeof(struct UnifedTimeOut));
    simOutput->AzimuthData = (struct AzimutSensorOut *)calloc(1, sizeof(struct AzimutSensorOut));
    simOutput->SummatorData = (struct ImitSummatorOut *)calloc(1, sizeof(struct ImitSummatorOut));
    simOutput->SummatorData->sum_signals = (float *)calloc(simParams->uTime->max_sampling_cnt, sizeof(float));
    simOutput->TargetPositionData = (struct TargetPositionOut *)calloc(1, sizeof(struct TargetPositionOut));
    simOutput->TargetPositionData->Target_map = (struct Point *)calloc(simParams->targetPosition->cntTarget, sizeof(struct Point));
    simOutput->TargetResponseData = (struct TargetResponseOut *)calloc(1, sizeof(struct TargetResponseOut));
    simOutput->TargetResponseData->target_map_find = (struct PointWith_discredNum *)calloc(simParams->targetPosition->cntTarget, sizeof(struct PointWith_discredNum));

    // Инициализация и запуск вычислительного потока
    int tickRate = 10;
    RadarComputeWorker *workerThread = new RadarComputeWorker(simParams, simOutput, tickRate);
    workerThread->start();

    const double PI = 3.141592653589793;
    QTimer timer;

    // Слой отрисовки интерфейса (Поток GUI) - срабатывает с разумной частотой, чтобы
    // не перегружать старый Qt и не терять отзывчивость интерфейса.
    QElapsedTimer frameTimer;
    bool frameTimerInitialized = false;
    auto redraw = [&]() {
        if (!frameTimerInitialized) {
            frameTimer.start();
            frameTimerInitialized = true;
        }

        if (frameTimer.elapsed() < 16) {
            return;
        }
        frameTimer.restart();

        QSize size = radarLabel->size();
        if (size.width() < 4 || size.height() < 4) return;

        QPixmap pixmap(size.width(), size.height());
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        int cx = size.width() / 2;
        int cy = size.height() / 2;
        int radius = std::min(cx, cy) - 25;

        // Статическая сетка
        painter.setPen(QPen(QColor(0, 100, 0), 1));
        painter.drawEllipse(QPoint(cx, cy), radius, radius);
        painter.drawEllipse(QPoint(cx, cy), radius * 2 / 3, radius * 2 / 3);
        painter.drawEllipse(QPoint(cx, cy), radius / 3, radius / 3);

        painter.setPen(QPen(QColor(0, 80, 0), 1, Qt::DashLine));
        for (int i = 0; i < 360; i += 30) {
            double rad = (i - 90) * PI / 180.0;
            painter.drawLine(cx, cy, cx + radius * std::cos(rad), cy + radius * std::sin(rad));
        }

        // КРИТИЧЕСКАЯ СЕКЦИЯ: Быстро забираем данные из вычислительного потока
        std::vector<SharedTargetPoint> drawTargets;
        g_dataMutex.lock();
        double drawAngle = g_sharedAngle;
        drawTargets = g_sharedTargets;
        g_dataMutex.unlock();

        double radians = (drawAngle - 90) * PI / 180.0;
        if (g_isSimulationRunning.load()) {
            painter.setPen(g_isRadiationOn.load() ? QPen(Qt::green, 3) : QPen(QColor(40, 180, 40), 3, Qt::DashLine));
        } else {
            painter.setPen(QPen(QColor(110, 110, 110), 3));
        }
        painter.drawLine(cx, cy, cx + radius * std::cos(radians), cy + radius * std::sin(radians));

        painter.setPen(QPen(QColor(255, 80, 80), 2));
        painter.setBrush(QBrush(QColor(255, 80, 80)));
        for (const auto &target : drawTargets) {
            double targetAngleRad = (target.angle - 90.0) * PI / 180.0;
            double normalizedDistance = target.distance / std::max(1.0f, simParams->imitator->maxDistance);
            normalizedDistance = std::max(0.0, std::min(1.0, normalizedDistance));
            int tx = cx + static_cast<int>(radius * normalizedDistance * std::cos(targetAngleRad));
            int ty = cy + static_cast<int>(radius * normalizedDistance * std::sin(targetAngleRad));
            painter.drawEllipse(QPoint(tx, ty), 4, 4);
        }

        painter.end();
        radarLabel->setPixmap(pixmap);

        // Фиксируем затраченное время
        //qint64 elapsedNs = frameTimer.nsecsElapsed(); // Время в наносекундах
        //double elapsedMs = elapsedNs / 1000000.0;     // Переводим в миллисекунды с точностью до плавающей точки

        // Вывод в консоль отладки Qt («Вывод приложения» / «Application Output»)
        //qDebug() << "Время генерации кадра:" << QString::number(elapsedMs, 'f', 2) << "мс";
    };

    QObject::connect(&timer, &QTimer::timeout, redraw);
    timer.start(16);
    redraw();

    // Интерактивная привязка кнопок управления (Пишут в атомарные переменные)
    QObject::connect(simButton, &QPushButton::clicked, [&]() {
        bool nextState = !g_isSimulationRunning.load();
        g_isSimulationRunning.store(nextState);
        simButton->setText(nextState ? "Стоп моделирования" : "Запуск моделирования");
    });
    QObject::connect(rotationButton, &QPushButton::clicked, [&]() {
        bool nextState = !g_isRotating.load();
        g_isRotating.store(nextState);
        rotationButton->setText(nextState ? "Остановить вращение" : "Запустить вращение");
    });
    QObject::connect(radButton, &QPushButton::clicked, [&]() {
        bool nextState = !g_isRadiationOn.load();
        g_isRadiationOn.store(nextState);
        radButton->setText(nextState ? "Выключить излучение" : "Включить излучение");
    });
    //QObject::connect(paramButton, &QPushButton::clicked, [&]() {
    //    ParamDialog dialog(&window);
    //    dialog.exec();
    //});

    QObject::connect(paramButton, &QPushButton::clicked, [&]() {
        ParamDialog dialog(&window);
        dialog.exec();

        // Получаем параметры из диалога
        QMap<QString, QMap<QString, QString>> params = dialog.getParameters();

        // Здесь можно применить параметры к структурам имитатора
        if (!params.isEmpty()) {
            qDebug() << "Параметры обновлены!";

            // Пример применения параметров (раскомментируйте при необходимости):
            /*
            if (params.contains("uTime")) {
                auto uTimeParams = params["uTime"];
                if (uTimeParams.contains("pulse_time")) {
                    simParams->uTime->pulse_time = uTimeParams["pulse_time"].toLongLong();
                }
                // ... и так далее для всех полей
            }
            */
        }
    });

    window.showMaximized();
    int result = app.exec();

    // Корректная остановка фонового вычислительного потока
    g_threadShouldStop.store(true);
    workerThread->quit();
    workerThread->wait(); // Ждем завершения текущего такта Си-модели
    delete workerThread;

    // Освобождение динамической памяти Си-структур
    free(simOutput->SummatorData->sum_signals);
    free(simOutput->SummatorData);
    free(simOutput->AzimuthData);
    free(simOutput->TimeData);
    free(simOutput->TargetPositionData->Target_map);
    free(simOutput->TargetPositionData);
    free(simOutput->TargetResponseData->target_map_find);
    free(simOutput->TargetResponseData);
    free(simOutput);
    free(simParams->imitator);
    free(simParams->uTime);
    free(simParams->azimuth);
    free(simParams->ppPosition);
    free(simParams->clutterResponse);
    free(simParams->clutterFormation);
    free(simParams->targetPosition);
    free(simParams->targetFormation);
    free(simParams->targetResponse);
    free(simParams->nipPosition);
    free(simParams->nipLevel);
    free(simParams->nipFormation);
    free(simParams->summator);
    free(simParams->frequencyConverter);
    free(simParams->noise);
    free(simParams);

    return result;
}
