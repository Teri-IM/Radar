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
#include <cstdlib>
#include "UI/paramdialog.h"

extern "C" {
#include "Imitator/Imitator.h"
#include "Imitator/UnifiedImitatorParam.h"
#include "Handler/include/processing_module.h"
#include "Handler/include/processing_module_param.h"
}

namespace RADAR {
constexpr double kPi = 3.141592653589793;
constexpr int kFrameIntervalMs = 16;
constexpr int kWorkerPauseMs = 5;
constexpr int kControlPanelWidth = 200;
constexpr int kRadarPadding = 25;

struct SharedTargetPoint {
    double angle;
    double distance;
    double amplitude;
};

struct SharedRuntimeState {
    QMutex mutex;
    double sharedAngle = 0.0;
    double maxRadarDistance = 100000.0; // Храним копию тут, чтобы GUI не лез в ресурсы Си!
    std::vector<SharedTargetPoint> sharedTargets;
    std::atomic<bool> isSimulationRunning{false};
    std::atomic<bool> isRotating{true};
    std::atomic<bool> isRadiationOn{false};
    std::atomic<bool> threadShouldStop{false};
    std::atomic<bool> blockComputation{false};
};

struct SimulationResources {
    struct ImitatorParametrs *simParams = nullptr;
    struct ImitOutData *simOutput = nullptr;
    struct GlobalProcessingParam *handlerParams = nullptr;
    struct Codogramm *handlerOutput = nullptr;
};

void cleanupSimulationResources(SimulationResources &resources);

void initializeSimulatorParameters(struct ImitatorParametrs *simParams) {
    if (simParams == nullptr) return;

    simParams->imitator = static_cast<struct ImitParam *>(calloc(1, sizeof(struct ImitParam)));
    if (simParams->imitator != nullptr) {
        simParams->imitator->maxDistance = 100000.0;
    }

    simParams->uTime = static_cast<struct UTimeParam *>(calloc(1, sizeof(struct UTimeParam)));
    if (simParams->uTime != nullptr) {
        simParams->uTime->probing_time = 10000;
        simParams->uTime->pulse_time = 6000;
        simParams->uTime->max_sampling_cnt = 10000000;
        simParams->uTime->sampling_rate = 2000;
    }

    simParams->azimuth = static_cast<struct AzimutParam *>(calloc(1, sizeof(struct AzimutParam)));
    if (simParams->azimuth != nullptr) {
        simParams->azimuth->startAngle = 0.0;
        simParams->azimuth->angularVelocity = 10.0;
    }

    simParams->ppPosition = static_cast<struct PPPosParam *>(calloc(1, sizeof(struct PPPosParam)));
    if (simParams->ppPosition != nullptr) {
        simParams->ppPosition->cntPP = 3;
        simParams->ppPosition->enable = 1;
        simParams->ppPosition->PPamplitude = 500.0;
    }

    simParams->clutterResponse = static_cast<struct ClutterResponseParams *>(calloc(1, sizeof(struct ClutterResponseParams)));
    if (simParams->clutterResponse != nullptr) {
        simParams->clutterResponse->enable = 1;
    }

    simParams->clutterFormation = static_cast<struct ClutterFormationParam *>(calloc(1, sizeof(struct ClutterFormationParam)));
    if (simParams->clutterFormation != nullptr) {
        simParams->clutterFormation->enable = 1;
    }

    simParams->targetPosition = static_cast<struct TargetPosParam *>(calloc(1, sizeof(struct TargetPosParam)));
    if (simParams->targetPosition != nullptr) {
        simParams->targetPosition->cntTarget = 3;
        simParams->targetPosition->enable = 1;
        simParams->targetPosition->Targetamplitude = 1500.0;
    }

    simParams->targetFormation = static_cast<struct TargetFormationParam *>(calloc(1, sizeof(struct TargetFormationParam)));
    if (simParams->targetFormation != nullptr) {
        simParams->targetFormation->enable = 1;
    }

    simParams->targetResponse = static_cast<struct TargetResponseParams *>(calloc(1, sizeof(struct TargetResponseParams)));
    if (simParams->targetResponse != nullptr) {
        simParams->targetResponse->enable = 1;
    }

    simParams->nipPosition = static_cast<struct NIPPosParam *>(calloc(1, sizeof(struct NIPPosParam)));
    if (simParams->nipPosition != nullptr) {
        simParams->nipPosition->cntNIP = 3;
        simParams->nipPosition->enable = 1;
        simParams->nipPosition->NIPamplitude = 600.0;
    }

    simParams->nipLevel = static_cast<struct NIPLvlParam *>(calloc(1, sizeof(struct NIPLvlParam)));
    if (simParams->nipLevel != nullptr) {
        simParams->nipLevel->enable = 1;
        simParams->nipLevel->amplitudeDecrease = 10.0;
    }

    simParams->nipFormation = static_cast<struct NIPFormationParam *>(calloc(1, sizeof(struct NIPFormationParam)));
    if (simParams->nipFormation != nullptr) {
        simParams->nipFormation->enable = 1;
    }

    simParams->summator = static_cast<struct SummatorParam *>(calloc(1, sizeof(struct SummatorParam)));
    if (simParams->summator != nullptr) {
        simParams->summator->enable = 1;
    }

    simParams->frequencyConverter = static_cast<struct FreqConvertorParam *>(calloc(1, sizeof(struct FreqConvertorParam)));
    if (simParams->frequencyConverter != nullptr) {
        simParams->frequencyConverter->enable = 1;
    }

    simParams->noise = static_cast<struct NoiseParam *>(calloc(1, sizeof(struct NoiseParam)));
    if (simParams->noise != nullptr) {
        simParams->noise->enable = 0;
        simParams->noise->mean = 20;
        simParams->noise->sigma = 15;
    }
}

bool initializeSimulationResources(SimulationResources &resources) {
    resources.simParams = static_cast<struct ImitatorParametrs *>(malloc(sizeof(struct ImitatorParametrs)));
    if (resources.simParams == nullptr) return false;

    initializeSimulatorParameters(resources.simParams);

    resources.simOutput = static_cast<struct ImitOutData *>(calloc(1, sizeof(struct ImitOutData)));
    if (resources.simOutput == nullptr) {
        cleanupSimulationResources(resources);
        return false;
    }

    resources.simOutput->TimeData = static_cast<struct UnifedTimeOut *>(calloc(1, sizeof(struct UnifedTimeOut)));
    resources.simOutput->AzimuthData = static_cast<struct AzimutSensorOut *>(calloc(1, sizeof(struct AzimutSensorOut)));
    resources.simOutput->SummatorData = static_cast<struct ImitSummatorOut *>(calloc(1, sizeof(struct ImitSummatorOut)));
    resources.simOutput->TargetPositionData = static_cast<struct TargetPositionOut *>(calloc(1, sizeof(struct TargetPositionOut)));
    resources.simOutput->TargetResponseData = static_cast<struct TargetResponseOut *>(calloc(1, sizeof(struct TargetResponseOut)));

    if (resources.simOutput->TimeData == nullptr || resources.simOutput->AzimuthData == nullptr ||
        resources.simOutput->SummatorData == nullptr || resources.simOutput->TargetPositionData == nullptr ||
        resources.simOutput->TargetResponseData == nullptr) {
        cleanupSimulationResources(resources);
        return false;
    }

    resources.simOutput->SummatorData->sum_signals = static_cast<float *>(calloc(resources.simParams->uTime->max_sampling_cnt, sizeof(float)));
    resources.simOutput->TargetPositionData->Target_map = static_cast<struct Point *>(calloc(resources.simParams->targetPosition->cntTarget, sizeof(struct Point)));
    resources.simOutput->TargetResponseData->target_map_find = static_cast<struct PointWith_discredNum *>(calloc(resources.simParams->targetPosition->cntTarget, sizeof(struct PointWith_discredNum)));

    if (resources.simOutput->SummatorData->sum_signals == nullptr || resources.simOutput->TargetPositionData->Target_map == nullptr ||
        resources.simOutput->TargetResponseData->target_map_find == nullptr) {
        cleanupSimulationResources(resources);
        return false;
    }

    resources.handlerOutput = static_cast<struct Codogramm *>(calloc(1, sizeof(struct Codogramm)));
    resources.handlerParams = static_cast<struct GlobalProcessingParam *>(malloc(sizeof(struct GlobalProcessingParam)));
    if (resources.handlerOutput == nullptr || resources.handlerParams == nullptr) {
        cleanupSimulationResources(resources);
        return false;
    }

    resources.handlerParams->threshold.threshold = 100;
    resources.handlerParams->threshold.enable = 1;
    resources.handlerParams->ADC.enable = 1;
    resources.handlerParams->DDC.enable = 1;
    resources.handlerParams->NN.enable = 1;
    resources.handlerParams->suppression_NIP.enable = 1;

    return true;
}

void cleanupSimulationResources(SimulationResources &resources) {
    if (resources.simOutput != nullptr) {
        if (resources.simOutput->SummatorData != nullptr) {
            free(resources.simOutput->SummatorData->sum_signals);
            free(resources.simOutput->SummatorData);
        }
        free(resources.simOutput->AzimuthData);
        free(resources.simOutput->TimeData);
        if (resources.simOutput->TargetPositionData != nullptr) {
            free(resources.simOutput->TargetPositionData->Target_map);
            free(resources.simOutput->TargetPositionData);
        }
        if (resources.simOutput->TargetResponseData != nullptr) {
            free(resources.simOutput->TargetResponseData->target_map_find);
            free(resources.simOutput->TargetResponseData);
        }
        free(resources.simOutput);
    }

    if (resources.simParams != nullptr) {
        free(resources.simParams->imitator);
        free(resources.simParams->uTime);
        free(resources.simParams->azimuth);
        free(resources.simParams->ppPosition);
        free(resources.simParams->clutterResponse);
        free(resources.simParams->clutterFormation);
        free(resources.simParams->targetPosition);
        free(resources.simParams->targetFormation);
        free(resources.simParams->targetResponse);
        free(resources.simParams->nipPosition);
        free(resources.simParams->nipLevel);
        free(resources.simParams->nipFormation);
        free(resources.simParams->summator);
        free(resources.simParams->frequencyConverter);
        free(resources.simParams->noise);
        free(resources.simParams);
    }

    free(resources.handlerOutput);
    free(resources.handlerParams);

    resources.simParams = nullptr;
    resources.simOutput = nullptr;
    resources.handlerParams = nullptr;
    resources.handlerOutput = nullptr;
}

// Теперь функция принимает только state_ и не трогает ресурсы Си-модели в GUI потоке!
void drawRadarScene(QPainter &painter, const QSize &size, const SharedRuntimeState &state) {
    if (size.width() < 4 || size.height() < 4) return;

    int cx = size.width() / 2;
    int cy = size.height() / 2;
    int radius = std::min(cx, cy) - kRadarPadding;

    painter.setPen(QPen(QColor(0, 100, 0), 1));
    painter.drawEllipse(QPoint(cx, cy), radius, radius);
    painter.drawEllipse(QPoint(cx, cy), radius * 2 / 3, radius * 2 / 3);
    painter.drawEllipse(QPoint(cx, cy), radius / 3, radius / 3);

    painter.setPen(QPen(QColor(0, 80, 0), 1, Qt::DashLine));
    for (int i = 0; i < 360; i += 30) {
        double rad = (i - 90) * kPi / 180.0;
        painter.drawLine(cx, cy, cx + radius * std::cos(rad), cy + radius * std::sin(rad));
    }

    double radians = (state.sharedAngle - 90.0) * kPi / 180.0;
    if (state.isSimulationRunning.load()) {
        painter.setPen(state.isRadiationOn.load() ? QPen(Qt::green, 3) : QPen(QColor(40, 180, 40), 3, Qt::DashLine));
    } else {
        painter.setPen(QPen(QColor(110, 110, 110), 3));
    }
    painter.drawLine(cx, cy, cx + radius * std::cos(radians), cy + radius * std::sin(radians));

    painter.setPen(QPen(QColor(255, 80, 80), 2));
    painter.setBrush(QBrush(QColor(255, 80, 80)));
    for (const auto &target : state.sharedTargets) {
        double targetAngleRad = (target.angle - 90.0) * kPi / 180.0;
        double normalizedDistance = target.distance / std::max(1.0, state.maxRadarDistance);
        normalizedDistance = std::max(0.0, std::min(1.0, normalizedDistance));
        int tx = cx + static_cast<int>(radius * normalizedDistance * std::cos(targetAngleRad));
        int ty = cy + static_cast<int>(radius * normalizedDistance * std::sin(targetAngleRad));
        painter.drawEllipse(QPoint(tx, ty), 4, 4);
    }
}

class RadarComputeWorker : public QThread {
public:
    RadarComputeWorker(SimulationResources &resources, SharedRuntimeState &state)
        : resources_(resources), state_(state) {}

protected:
    void run() override {
        while (!state_.threadShouldStop.load()) {
            if (!state_.isSimulationRunning.load() || state_.blockComputation.load()) {
                msleep(10);
                continue;
            }

            auto *simParams = resources_.simParams;
            auto *simOutput = resources_.simOutput;
            if (simParams == nullptr || simOutput == nullptr || simParams->azimuth == nullptr ||
                simOutput->AzimuthData == nullptr || simOutput->TimeData == nullptr ||
                simOutput->SummatorData == nullptr) {
                msleep(10);
                continue;
            }

            {
                QMutexLocker locker(&state_.mutex);
                simParams->azimuth->angularVelocity = state_.isRotating.load() ? 10.0 : 0.0;
                if (simParams->targetFormation != nullptr) {
                    simParams->targetFormation->enable = state_.isRadiationOn.load() ? 1 : 0;
                }
                if (simParams->clutterFormation != nullptr) {
                    simParams->clutterFormation->enable = state_.isRadiationOn.load() ? 1 : 0;
                }
                // На всякий случай синхронизируем актуальную дальность, если её поменяют
                if (simParams->imitator != nullptr) {
                    state_.maxRadarDistance = simParams->imitator->maxDistance;
                }
            }

            if (Imitator(simParams, simOutput) != 0) {
                msleep(kWorkerPauseMs);
                continue;
            }

            if (resources_.handlerParams != nullptr && resources_.handlerOutput != nullptr) {
                ProcessingModule(resources_.handlerParams, simOutput, resources_.handlerOutput);
            }

            {
                QMutexLocker locker(&state_.mutex);
                state_.sharedAngle = simOutput->AzimuthData->azimuth_new;
                simParams->azimuth->startAngle = state_.sharedAngle;

                state_.sharedTargets.clear();
                if (resources_.handlerOutput != nullptr && resources_.handlerOutput->number_of_objects > 0) {
                    for (int i = 0; i < resources_.handlerOutput->number_of_objects; ++i) {
                        const struct threshold_device_out &target = resources_.handlerOutput->sign[i];
                        if (target.amplitude > 0 && target.AzimuthData != nullptr) {
                            state_.sharedTargets.push_back({
                                target.AzimuthData->azimuth_new,
                                static_cast<double>(target.distance),
                                static_cast<double>(target.amplitude)
                            });
                        }
                    }
                }
            }

            msleep(kWorkerPauseMs);
        }
    }

private:
    SimulationResources &resources_;
    SharedRuntimeState &state_;
};
}

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
    controlWidget->setMinimumWidth(RADAR::kControlPanelWidth);
    controlWidget->setStyleSheet("background-color: #101010; border: 1px solid #2a2a2a;");

    mainLayout->addWidget(radarLabel, 3);
    mainLayout->addWidget(controlWidget, 1);

    RADAR::SharedRuntimeState runtimeState;
    RADAR::SimulationResources resources;
    if (!initializeSimulationResources(resources)) {
        qDebug() << "Не удалось инициализировать ресурсы моделирования";
        return 1;
    }
    
    // Передаем начальное значение дальности Си-модели в состояние синхронизации
    if (resources.simParams != nullptr && resources.simParams->imitator != nullptr) {
        runtimeState.maxRadarDistance = resources.simParams->imitator->maxDistance;
    }

    RADAR::RadarComputeWorker workerThread(resources, runtimeState);
    workerThread.start();

    QTimer timer;
    QElapsedTimer frameTimer;
    bool frameTimerInitialized = false;

    auto redraw = [&]() {
        if (!frameTimerInitialized) {
            frameTimer.start();
            frameTimerInitialized = true;
        }

        if (frameTimer.elapsed() < RADAR::kFrameIntervalMs) {
            return;
        }
        frameTimer.restart();

        QSize size = radarLabel->size();
        if (size.width() < 4 || size.height() < 4) return;

        QPixmap pixmap(size.width(), size.height());
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        {
            QMutexLocker locker(&runtimeState.mutex);
            // drawRadarScene теперь абсолютно изолирована от структур ресурсов Си-модели!
            drawRadarScene(painter, size, runtimeState);
        }

        painter.end();
        radarLabel->setPixmap(pixmap);
    };

    QObject::connect(&timer, &QTimer::timeout, redraw);
    timer.start(RADAR::kFrameIntervalMs);
    redraw();

    QObject::connect(simButton, &QPushButton::clicked, [&]() {
        bool nextState = !runtimeState.isSimulationRunning.load();
        runtimeState.isSimulationRunning.store(nextState);
        simButton->setText(nextState ? "Стоп моделирования" : "Запуск моделирования");
    });

    QObject::connect(rotationButton, &QPushButton::clicked, [&]() {
        bool nextState = !runtimeState.isRotating.load();
        runtimeState.isRotating.store(nextState);
        rotationButton->setText(nextState ? "Запустить вращение" : "Остановить вращение");
    });

    QObject::connect(radButton, &QPushButton::clicked, [&]() {
        bool nextState = !runtimeState.isRadiationOn.load();
        runtimeState.isRadiationOn.store(nextState);
        radButton->setText(nextState ? "Выключить излучение" : "Включить излучение");
    });

    QObject::connect(paramButton, &QPushButton::clicked, [&]() {
        ParamDialog dialog(&window);
        dialog.exec();

        QMap<QString, QMap<QString, QString>> params = dialog.getParameters();
        if (!params.isEmpty()) {
            qDebug() << "Параметры обновлены!";
            
            runtimeState.blockComputation.store(true);
            {
                QMutexLocker locker(&runtimeState.mutex);
                if (params.contains("uTime")) {
                    auto uTimeParams = params["uTime"];
                    if (uTimeParams.contains("pulse_time") && resources.simParams != nullptr && resources.simParams->uTime != nullptr) {
                        resources.simParams->uTime->pulse_time = uTimeParams["pulse_time"].toLongLong();
                    }
                }
                // Синхронизируем maxDistance, если он менялся в диалоге
                if (resources.simParams != nullptr && resources.simParams->imitator != nullptr) {
                    runtimeState.maxRadarDistance = resources.simParams->imitator->maxDistance;
                }
            }
            runtimeState.blockComputation.store(false);
        }
    });

    window.showMaximized();
    int result = app.exec();

    // --- НАДЕЖНЫЙ ПОРЯДОК ОСТАНОВКИ ПОТОКА ---
    // Сначала жестко выставляем флаги остановки
    runtimeState.threadShouldStop.store(true);
    runtimeState.blockComputation.store(false); // Выводим из возможного лока блока
    
    // Будим поток, если он застрял, и корректно дожидаемся выхода из run()
    workerThread.quit();
    workerThread.wait(); // Стек main() гарантированно не разрушится, пока поток активен

    cleanupSimulationResources(resources);
    return result;
}