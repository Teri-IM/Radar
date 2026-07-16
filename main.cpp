

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

#include "UI/paramdialog.h"
// Подключаем Си-интерфейсы
extern "C" {
#include "Imitator/Imitator.h"
#include "Imitator/UnifiedImitatorParam.h"

int Imitator(struct ImitatorParametrs *parametrs, struct ImitOutData *outData);

// Предполагаемый прототип вашего обработчика (подставьте ваш реальный, если имена отличаются)
// void Handler(struct HandlerParametrs *params, struct ImitOutData *inData, struct HandlerOutData *outData);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Индикатор кругового обзора");
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

    // =========================================================================
    // 1. ВЫДЕЛЕНИЕ И НАСТРОЙКА ВХОДНЫХ ПАРАМЕТРОВ ИМИТАТОРА
    // =========================================================================
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

    // =========================================================================
    // 2. СОЗДАНИЕ ВЫХОДНЫХ СТРУКТУР (ДЛЯ ИМИТАТОРА И ОБРАБОТЧИКА)
    // =========================================================================
    struct ImitOutData *simOutput = (struct ImitOutData *)malloc(sizeof(struct ImitOutData));

    // Дополнительно выделяем память под вложенные структуры данных
    simOutput->TimeData = (struct UnifedTimeOut *)calloc(1, sizeof(struct UnifedTimeOut));
    simOutput->AzimuthData = (struct AzimutSensorOut *)calloc(1, sizeof(struct AzimutSensorOut));
    simOutput->SummatorData = (struct ImitSummatorOut *)calloc(1, sizeof(struct ImitSummatorOut));
    // Выделяем буфер под сигналы сумматора
    simOutput->SummatorData->sum_signals = (float *)calloc(simParams->uTime->max_sampling_cnt, sizeof(float));

    // Выходные данные ОБРАБОТЧИКА (закомментировано, но оставлено для будущего использования)
    // struct HandlerOutData *handlerOutput = (struct HandlerOutData *)malloc(sizeof(struct HandlerOutData));
    // struct HandlerParametrs *handlerParams = (struct HandlerParametrs *)malloc(sizeof(struct HandlerParametrs));

    // Состояния GUI
    double currentAngle = 0.0;
    bool isSimulationRunning = false;
    bool isRotating = true;
    bool isRadiationOn = false;

    const double PI = 3.141592653589793;
    QTimer timer;

    auto redraw = [&]() {
        QSize size = radarLabel->size();
        if (size.width() < 4 || size.height() < 4) return;

        int w = size.width();
        int h = size.height();
        QPixmap pixmap(w, h);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        int cx = w / 2;
        int cy = h / 2;
        int radius = std::min(cx, cy) - 25;

        // Отрисовка статической сетки РЛС
        painter.setPen(QPen(QColor(0, 100, 0), 1));
        painter.drawEllipse(QPoint(cx, cy), radius, radius);
        painter.drawEllipse(QPoint(cx, cy), radius * 2 / 3, radius * 2 / 3);
        painter.drawEllipse(QPoint(cx, cy), radius / 3, radius / 3);

        painter.setPen(QPen(QColor(0, 80, 0), 1, Qt::DashLine));
        QFont font = painter.font();
        font.setPointSize(9);
        painter.setFont(font);

        for (int i = 0; i < 360; i += 30) {
            double rad = (i - 90) * PI / 180.0;
            painter.drawLine(cx, cy, cx + radius * std::cos(rad), cy + radius * std::sin(rad));
            painter.setPen(QPen(QColor(0, 150, 0)));
            painter.drawText(QRect(cx + (radius + 15) * std::cos(rad) - 15, cy + (radius + 15) * std::sin(rad) - 10, 30, 20), Qt::AlignCenter, QString::number(i) + "°");
            painter.setPen(QPen(QColor(0, 80, 0), 1, Qt::DashLine));
        }

        // РАБОТА С ИМИТАТОРОМ И ОБРАБОТЧИКОМ
        if (isSimulationRunning) {
            // Обновляем параметры перед вызовом имитатора
            simParams->azimuth->angularVelocity = isRotating ? 10.0 : 0.0;
            simParams->targetFormation->enable = isRadiationOn ? 1 : 0;
            simParams->clutterFormation->enable = isRadiationOn ? 1 : 0;

            // 1. Вызываем Имитатор
            Imitator(simParams, simOutput);

            // 2. Обновляем угол из выходных данных
            if (simOutput->AzimuthData != nullptr) {
                currentAngle = simOutput->AzimuthData->azimuth_new;
                simParams->azimuth->startAngle = currentAngle;
            }

            // 3. Вызываем Обработчик (закомментирован, но оставлен для будущего использования)
            // Handler(handlerParams, simOutput, handlerOutput);

            // Отрисовка луча в зависимости от состояния излучения
            if (isRadiationOn) {
                painter.setPen(QPen(Qt::green, 3));
            } else {
                painter.setPen(QPen(QColor(40, 180, 40), 3, Qt::DashLine));
            }
        } else {
            painter.setPen(QPen(QColor(110, 110, 110), 3));
        }

        // Отрисовка луча
        double radians = (currentAngle - 90) * PI / 180.0;
        painter.drawLine(cx, cy, cx + radius * std::cos(radians), cy + radius * std::sin(radians));

        painter.end();
        radarLabel->setPixmap(pixmap);
    };

    QObject::connect(&timer, &QTimer::timeout, redraw);
    timer.start(3);
    redraw();

    // Логика кнопок
    QObject::connect(simButton, &QPushButton::clicked, [&]() {
        isSimulationRunning = !isSimulationRunning;
        simButton->setText(isSimulationRunning ? "Стоп моделирования" : "Запуск моделирования");
    });
    QObject::connect(rotationButton, &QPushButton::clicked, [&]() {
        isRotating = !isRotating;
        rotationButton->setText(isRotating ? "Остановить вращение" : "Запустить вращение");
    });
    QObject::connect(radButton, &QPushButton::clicked, [&]() {
        isRadiationOn = !isRadiationOn;
        radButton->setText(isRadiationOn ? "Выключить излучение" : "Включить излучение");
    });

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

    // Освобождение памяти при закрытии
    free(simOutput->SummatorData->sum_signals);
    free(simOutput->SummatorData);
    free(simOutput->AzimuthData);
    free(simOutput->TimeData);
    free(simOutput);

    // free(handlerOutput);
    // free(handlerParams);

    free(simParams->imitator); free(simParams->uTime); free(simParams->azimuth);
    free(simParams->ppPosition); free(simParams->clutterResponse); free(simParams->clutterFormation);
    free(simParams->targetPosition); free(simParams->targetFormation); free(simParams->targetResponse);
    free(simParams->nipPosition); free(simParams->nipLevel); free(simParams->nipFormation);
    free(simParams->summator); free(simParams->frequencyConverter); free(simParams->noise);
    free(simParams);

    return result;
}
