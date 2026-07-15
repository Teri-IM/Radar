/*
* Автор: Авдеев Матвей
*
* Наименование модуля: Основное окно радара
*
* Назначение: Объединение и вызов модулей, отрисовка индикатора кругового обзора,
* контроль функций радара
*/
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

#include <QThread>
#include <QMutex>
#include <QElapsedTimer>
#include <QDebug>
#include <atomic>

//#include "UI/paramdialog.h"

// Подключаем Си-интерфейсы
extern "C" {
#include "Imitator/Imitator.h"
#include "Imitator/UnifiedImitatorParam.h"
}

// =========================================================================
// ГЛОБАЛЬНЫЕ ДАННЫЕ СИНХРОНИЗАЦИИ ПОТОКОВ (Для разделения GUI и математики)
// =========================================================================
QMutex g_dataMutex;          // Мьютекс для защиты общих данных РЛС
double g_sharedAngle = 0.0;   // Актуальный угол, передаваемый из Си-модели в GUI

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
            if (g_isSimulationRunning.load()) {

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
                // [ЗДЕСЬ БУДЕТ ВЫЗОВ ОБРАБОТЧИКА]:
                // Handler(..., simOutput, ...);
                // Копируем обнаруженные точки в g_sharedTargets под мьютексом
                g_dataMutex.unlock();

            } else {
                // Если симуляция на паузе, спим 5 мс, чтобы не грузить ядро процессора пустым циклом
                msleep(5);
            }
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

    //QPushButton *paramButton = new QPushButton("Настройка параметров", &window);
    //paramButton->setMinimumSize(160, 44);
    //paramButton->setStyleSheet("font-size: 14px; padding: 8px; color: white; background-color: #2980b9; border: 1px solid #3498db;");

    controlLayout->addWidget(controlLabel);
    controlLayout->addWidget(simButton);
    controlLayout->addWidget(rotationButton);
    controlLayout->addWidget(radButton);
    //controlLayout->addWidget(paramButton);
    controlLayout->addStretch(1);

    QWidget *controlWidget = new QWidget(&window);
    controlWidget->setLayout(controlLayout);
    controlWidget->setMinimumWidth(200);
    controlWidget->setStyleSheet("background-color: #101010; border: 1px solid #2a2a2a;");

    mainLayout->addWidget(radarLabel, 3);
    mainLayout->addWidget(controlWidget, 1);

    // Выделение памяти под Си-структуры параметров
    struct ImitatorParametrs *simParams = (struct ImitatorParametrs *)calloc(1, sizeof(struct ImitatorParametrs));
    if (simParams == nullptr) {
        qCritical() << "Не удалось выделить память для параметров симуляции";
        return 1;
    }

    simParams->imitator = (struct ImitParam *)calloc(1, sizeof(struct ImitParam));
    simParams->uTime = (struct UTimeParam *)calloc(1, sizeof(struct UTimeParam));
    simParams->azimuth = (struct AzimutParam *)calloc(1, sizeof(struct AzimutParam));
    simParams->ppPosition = (struct PPPosParam *)calloc(1, sizeof(struct PPPosParam));
    simParams->clutterResponse = (struct ClutterResponseParams *)calloc(1, sizeof(struct ClutterResponseParams));
    simParams->clutterFormation = (struct ClutterFormationParam *)calloc(1, sizeof(struct ClutterFormationParam));
    simParams->targetPosition = (struct TargetPosParam *)calloc(1, sizeof(struct TargetPosParam));
    simParams->targetFormation = (struct TargetFormationParam *)calloc(1, sizeof(struct TargetFormationParam));
    simParams->targetResponse = (struct TargetResponseParams *)calloc(1, sizeof(struct TargetResponseParams));
    simParams->nipPosition = (struct NIPPosParam *)calloc(1, sizeof(struct NIPPosParam));
    simParams->nipLevel = (struct NIPLvlParam *)calloc(1, sizeof(struct NIPLvlParam));
    simParams->nipFormation = (struct NIPFormationParam *)calloc(1, sizeof(struct NIPFormationParam));
    simParams->summator = (struct SummatorParam *)calloc(1, sizeof(struct SummatorParam));
    simParams->frequencyConverter = (struct FreqConvertorParam *)calloc(1, sizeof(struct FreqConvertorParam));
    simParams->noise = (struct NoiseParam *)calloc(1, sizeof(struct NoiseParam));

    if (simParams->imitator == nullptr || simParams->uTime == nullptr || simParams->azimuth == nullptr ||
        simParams->ppPosition == nullptr || simParams->clutterResponse == nullptr || simParams->clutterFormation == nullptr ||
        simParams->targetPosition == nullptr || simParams->targetFormation == nullptr || simParams->targetResponse == nullptr ||
        simParams->nipPosition == nullptr || simParams->nipLevel == nullptr || simParams->nipFormation == nullptr ||
        simParams->summator == nullptr || simParams->frequencyConverter == nullptr || simParams->noise == nullptr) {
        qCritical() << "Не удалось выделить память для одной из структур параметров";
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
        return 1;
    }

    simParams->imitator->maxDistance = 100000.0;
    simParams->uTime->probing_time = 10000;
    simParams->uTime->pulse_time = 6000;
    simParams->uTime->max_sampling_cnt = 10000000;
    simParams->uTime->sampling_rate = 2000;
    simParams->azimuth->startAngle = 0.0;
    simParams->azimuth->angularVelocity = 10.0;
    simParams->ppPosition->cntPP = 3;
    simParams->ppPosition->enable = 1;
    simParams->ppPosition->PPamplitude = 500.0;
    simParams->clutterResponse->enable = 1;
    simParams->clutterFormation->enable = 1;
    simParams->targetPosition->cntTarget = 3;
    simParams->targetPosition->enable = 1;
    simParams->targetPosition->Targetamplitude = 1500.0;
    simParams->targetFormation->enable = 1;
    simParams->targetResponse->enable = 1;
    simParams->nipPosition->cntNIP = 3;
    simParams->nipPosition->enable = 1;
    simParams->nipPosition->NIPamplitude = 600.0;
    simParams->nipLevel->enable = 1;
    simParams->nipLevel->amplitudeDecrease = 10.0;
    simParams->nipFormation->enable = 1;
    simParams->summator->enable = 1;
    simParams->frequencyConverter->enable = 1;
    simParams->noise->enable = 0;
    simParams->noise->mean = 20;
    simParams->noise->sigma = 15;

    struct ImitOutData *simOutput = (struct ImitOutData *)calloc(1, sizeof(struct ImitOutData));
    if (simOutput == nullptr) {
        qCritical() << "Не удалось выделить память для выходных данных симуляции";
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
        return 1;
    }

    if (initImitOutData(simOutput, simParams->uTime) != 0) {
        qCritical() << "Не удалось инициализировать выходные структуры имитатора";
        freeImitOutData(simOutput);
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
        return 1;
    }

    // Инициализация и запуск вычислительного потока
    int tickRate = 10;
    RadarComputeWorker *workerThread = new RadarComputeWorker(simParams, simOutput, tickRate);
    workerThread->start();

    const double PI = 3.141592653589793;
    QTimer timer;

    // Слой отрисовки интерфейса (Поток GUI) - срабатывает строго каждые 30 мс
    auto redraw = [&]() {
        QElapsedTimer frameTimer;
        frameTimer.start();

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
        g_dataMutex.lock();
        double drawAngle = g_sharedAngle;
        // Тут забираем локальную копию карты целей для отрисовки от обработчика
        g_dataMutex.unlock();

        // Отрисовка луча антенны
        double radians = (drawAngle - 90) * PI / 180.0;
        if (g_isSimulationRunning.load()) {
            painter.setPen(g_isRadiationOn.load() ? QPen(Qt::green, 3) : QPen(QColor(40, 180, 40), 3, Qt::DashLine));
        } else {
            painter.setPen(QPen(QColor(110, 110, 110), 3));
        }
        painter.drawLine(cx, cy, cx + radius * std::cos(radians), cy + radius * std::sin(radians));

        // [ТУТ ОТРИСОВКА ТОЧЕК ЦЕЛЕЙ НА ОСНОВЕ СКОПИРОВАННЫХ ДАННЫХ]

        painter.end();
        radarLabel->setPixmap(pixmap);

        // Фиксируем затраченное время
        qint64 elapsedNs = frameTimer.nsecsElapsed(); // Время в наносекундах
        double elapsedMs = elapsedNs / 1000000.0;     // Переводим в миллисекунды с точностью до плавающей точки

        // Вывод в консоль отладки Qt («Вывод приложения» / «Application Output»)
#if defined(DEBUG) || defined(_DEBUG) || defined(QT_DEBUG)
        qDebug() << "Время генерации кадра:" << QString::number(elapsedMs, 'f', 2) << "мс";
#endif
    };

    QObject::connect(&timer, &QTimer::timeout, redraw);
    timer.start(30);
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

    window.showMaximized();
    int result = app.exec();

    // Корректная остановка фонового вычислительного потока
    g_threadShouldStop.store(true);
    workerThread->quit();
    workerThread->wait(); // Ждем завершения текущего такта Си-модели
    delete workerThread;

    // Освобождение динамической памяти Си-структур
    freeImitOutData(simOutput);
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