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

// Подключаем наш новый заголовочный файл диалога
#include "paramdialog.h" 

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Индикатор кругового обзора");
    window.setMinimumSize(700, 450);
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

    // === НОВАЯ КНОПКА ДЛЯ ВЫЗОВА ДИАЛОГА ===
    QPushButton *paramButton = new QPushButton("Настройка параметров", &window);
    paramButton->setMinimumSize(160, 44);
    paramButton->setStyleSheet("font-size: 14px; padding: 8px; color: white; background-color: #2980b9; border: 1px solid #3498db;");

    controlLayout->addWidget(controlLabel);
    controlLayout->addWidget(simButton);
    controlLayout->addWidget(rotationButton);
    controlLayout->addWidget(radButton);
    controlLayout->addWidget(paramButton); // Добавляем кнопку в разметку
    controlLayout->addStretch(1);

    QWidget *controlWidget = new QWidget(&window);
    controlWidget->setLayout(controlLayout);
    controlWidget->setMinimumWidth(200);
    controlWidget->setStyleSheet("background-color: #101010; border: 1px solid #2a2a2a;");

    mainLayout->addWidget(radarLabel, 3);
    mainLayout->addWidget(controlWidget, 1);

    // Состояния
    int angle = 0;
    bool isSimulationRunning = false;
    bool isRotating = true;
    bool isRadiationOn = false;

    const double PI = 3.141592653589793;
    QTimer timer;

    auto redraw = [&]() {
        QSize size = radarLabel->size();
        if (size.width() < 4 || size.height() < 4) return;

        QPixmap pixmap(size.width(), size.height());
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        int cx = size.width() / 2;
        int cy = size.height() / 2;
        int radius = std::min(cx, cy) - 25;

        // Сетка
        painter.setPen(QPen(QColor(0, 100, 0), 1));
        painter.drawEllipse(QPoint(cx, cy), radius, radius);
        painter.drawEllipse(QPoint(cx, cy), radius * 2 / 3, radius * 2 / 3);
        painter.drawEllipse(QPoint(cx, cy), radius / 3, radius / 3);

        // Линии направлений
        painter.setPen(QPen(QColor(0, 80, 0), 1, Qt::DashLine));
        for (int i = 0; i < 360; i += 30) {
            double rad = (i - 90) * PI / 180.0;
            painter.drawLine(cx, cy, cx + radius * std::cos(rad), cy + radius * std::sin(rad));
        }
        // Луч антенны
        double radians = (angle - 90) * PI / 180.0;
        if (isSimulationRunning) {
            painter.setPen(isRadiationOn ? QPen(Qt::green, 3) : QPen(QColor(40, 180, 40), 3, Qt::DashLine));
            if (isRotating) angle = (angle + 2) % 360;
        } else {
            painter.setPen(QPen(QColor(110, 110, 110), 3));
        }
        painter.drawLine(cx, cy, cx + radius * std::cos(radians), cy + radius * std::sin(radians));

        painter.end();
        radarLabel->setPixmap(pixmap);
    };

    QObject::connect(&timer, &QTimer::timeout, redraw);
    timer.start(30);

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

    // === ЛОГИКА НАЖАТИЯ НА КНОПКУ НАСТРОЕК ===
    QObject::connect(paramButton, &QPushButton::clicked, [&]() {
        ParamDialog dialog(&window); // Передаем главное окно как родителя
        dialog.exec();               // Вызываем модально (exec заблокирует поток GUI на время настроек)
    });

    window.showMaximized();
    return app.exec();
}