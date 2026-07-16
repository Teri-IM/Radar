
#include "paramdialog.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QMessageBox>

ParamDialog::ParamDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Редактор параметров имитатора");
    resize(600, 450);
    setWindowModality(Qt::WindowModal);

    setStyleSheet("QDialog { background-color: #2c3e50; color: #ffffff; }"
                  "QMessageBox { background-color: #1a1a2e; color: #ffffff; }"
                  "QMessageBox QLabel { color: #ffffff; }"
                  "QMessageBox QPushButton { background-color: #3498db; color: white; padding: 6px 15px; border-radius: 4px; }"
                  "QMessageBox QPushButton:hover { background-color: #2980b9; }");

    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setSpacing(10);
    horizontalLayout->setContentsMargins(10, 10, 10, 10);

    // === Левая панель параметров ===
    QWidget *leftWidget = new QWidget(this);
    leftWidget->setStyleSheet("QWidget { background-color: #34495e; border-radius: 5px; }");
    leftWidget->setFixedWidth(200);

    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(10, 10, 10, 10);
    leftLayout->setSpacing(8);

    QLabel *label1 = new QLabel("Основные параметры:", leftWidget);
    label1->setStyleSheet("font-weight: bold; color: #ffffff;");

    comboBox = new QComboBox(leftWidget);
    comboBox->setStyleSheet("QComboBox { background-color: #2c3e50; color: #ffffff; border: 1px solid #5d6d7e; padding: 5px; border-radius: 3px; }"
                            "QComboBox::drop-down { background-color: #2c3e50; }"
                            "QComboBox QAbstractItemView { background-color: #2c3e50; color: #ffffff; selection-background-color: #3498db; }");
    comboBox->addItem("uTime");
    comboBox->addItem("imitator");
    comboBox->addItem("frequencyConverter");
    comboBox->addItem("azimuth");
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ParamDialog::onComboBoxChanged);

    QLabel *label2 = new QLabel("Параметры целей:", leftWidget);
    label2->setStyleSheet("font-weight: bold; color: #ffffff;");

    comboBox_2 = new QComboBox(leftWidget);
    comboBox_2->setStyleSheet("QComboBox { background-color: #2c3e50; color: #ffffff; border: 1px solid #5d6d7e; padding: 5px; border-radius: 3px; }"
                              "QComboBox::drop-down { background-color: #2c3e50; }"
                              "QComboBox QAbstractItemView { background-color: #2c3e50; color: #ffffff; selection-background-color: #3498db; }");
    comboBox_2->addItem("targetResponse");
    comboBox_2->addItem("targetPosition");
    comboBox_2->addItem("targetFormation");
    connect(comboBox_2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ParamDialog::onComboBox2Changed);

    QLabel *label3 = new QLabel("Параметры помех:", leftWidget);
    label3->setStyleSheet("font-weight: bold; color: #ffffff;");

    comboBox_3 = new QComboBox(leftWidget);
    comboBox_3->setStyleSheet("QComboBox { background-color: #2c3e50; color: #ffffff; border: 1px solid #5d6d7e; padding: 5px; border-radius: 3px; }"
                              "QComboBox::drop-down { background-color: #2c3e50; }"
                              "QComboBox QAbstractItemView { background-color: #2c3e50; color: #ffffff; selection-background-color: #3498db; }");
    comboBox_3->addItem("noise");
    comboBox_3->addItem("ppPosition");
    comboBox_3->addItem("nipPosition");
    comboBox_3->addItem("nipLevel");
    comboBox_3->addItem("nipFormation");
    comboBox_3->addItem("summator");
    comboBox_3->addItem("clutterResponse");
    comboBox_3->addItem("clutterFormation");
    connect(comboBox_3, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ParamDialog::onComboBox3Changed);

    leftLayout->addWidget(label1);
    leftLayout->addWidget(comboBox);
    leftLayout->addWidget(label2);
    leftLayout->addWidget(comboBox_2);
    leftLayout->addWidget(label3);
    leftLayout->addWidget(comboBox_3);
    leftLayout->addStretch();

    horizontalLayout->addWidget(leftWidget);

    // === Правая панель с прокруткой ===
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background-color: #2c3e50; border: none; }"
                              "QScrollBar:vertical { background-color: #34495e; width: 12px; }"
                              "QScrollBar::handle:vertical { background-color: #5d6d7e; border-radius: 6px; min-height: 20px; }"
                              "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }");

    rightContentWidget = new QWidget();
    rightContentWidget->setStyleSheet("QWidget { background-color: #2c3e50; }");
    rightLayout = new QVBoxLayout(rightContentWidget);
    rightLayout->setSpacing(10);
    rightLayout->setContentsMargins(15, 15, 15, 15);
    rightLayout->setAlignment(Qt::AlignTop);

    scrollArea->setWidget(rightContentWidget);
    horizontalLayout->addWidget(scrollArea);

    createStructFields();
    displayStructFields("uTime");
}

ParamDialog::~ParamDialog() {}

void ParamDialog::createStructFields()
{
    structFields["uTime"] = {
        {"pulse_time", "long long", nullptr, "uTime"},
        {"probing_time", "long long", nullptr, "uTime"},
        {"sampling_rate", "long long", nullptr, "uTime"},
        {"max_sampling_cnt", "long long", nullptr, "uTime"}
    };
    structFields["targetResponse"] = {{"enable", "bool", nullptr, "targetResponse"}};
    structFields["targetPosition"] = {
        {"enable", "bool", nullptr, "targetPosition"},
        {"cntTarget", "int", nullptr, "targetPosition"},
        {"Targetamplitude", "float", nullptr, "targetPosition"}
    };
    structFields["targetFormation"] = {{"enable", "bool", nullptr, "targetFormation"}};
    structFields["ppPosition"] = {
        {"enable", "bool", nullptr, "ppPosition"},
        {"cntPP", "int", nullptr, "ppPosition"},
        {"PPamplitude", "float", nullptr, "ppPosition"}
    };
    structFields["noise"] = {
        {"enable", "bool", nullptr, "noise"},
        {"mean", "double", nullptr, "noise"},
        {"sigma", "double", nullptr, "noise"}
    };
    structFields["nipPosition"] = {
        {"enable", "bool", nullptr, "nipPosition"},
        {"cntNIP", "int", nullptr, "nipPosition"},
        {"NIPamplitude", "float", nullptr, "nipPosition"},
        {"MaxNIP_probingTime", "long long", nullptr, "nipPosition"}
    };
    structFields["nipLevel"] = {
        {"enable", "bool", nullptr, "nipLevel"},
        {"amplitudeDecrease", "float", nullptr, "nipLevel"}
    };
    structFields["nipFormation"] = {{"enable", "bool", nullptr, "nipFormation"}};
    structFields["summator"] = {{"enable", "bool", nullptr, "summator"}};
    structFields["imitator"] = {{"maxDistance", "float", nullptr, "imitator"}};
    structFields["frequencyConverter"] = {{"enable", "bool", nullptr, "frequencyConverter"}};
    structFields["clutterResponse"] = {{"enable", "bool", nullptr, "clutterResponse"}};
    structFields["clutterFormation"] = {{"enable", "bool", nullptr, "clutterFormation"}};
    structFields["azimuth"] = {
        {"enable", "bool", nullptr, "azimuth"},
        {"startAngle", "float", nullptr, "azimuth"},
        {"angularVelocity", "float", nullptr, "azimuth"}
    };
}

void ParamDialog::clearRightPanel()
{
    QLayoutItem *item;
    while ((item = rightLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void ParamDialog::displayStructFields(const QString &structName)
{
    clearRightPanel();
    currentStructName = structName;

    if (!structFields.contains(structName)) {
        QLabel *label = new QLabel("Структура не найдена");
        label->setStyleSheet("color: #ff6b6b;");
        rightLayout->addWidget(label);
        return;
    }

    auto &fields = structFields[structName];

    QLabel *titleLabel = new QLabel(QString("<b>%1</b>").arg(structName));
    titleLabel->setStyleSheet("font-size: 14px; color: #ffd93d;");
    rightLayout->addWidget(titleLabel);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("QFrame { color: #5d6d7e; background-color: #5d6d7e; max-height: 2px; }");
    rightLayout->addWidget(line);

    for (auto &field : fields) {
        QHBoxLayout *fieldLayout = new QHBoxLayout();
        fieldLayout->setSpacing(10);

        QLabel *label = new QLabel(field.fieldName + ":");
        label->setMinimumWidth(150);
        label->setStyleSheet("font-weight: bold; color: #ecf0f1;");
        fieldLayout->addWidget(label);

        // Проверяем сохраненное значение
        QString savedValue = "";
        if (allParameters.contains(structName) && allParameters[structName].contains(field.fieldName)) {
            savedValue = allParameters[structName][field.fieldName];
        }

        if (field.fieldType == "bool") {
            QComboBox *combo = new QComboBox();
            combo->addItem("Выключен");
            combo->addItem("Включен");
            combo->setObjectName(field.fieldName);
            combo->setStyleSheet("QComboBox { background-color: #34495e; color: #ffffff; border: 1px solid #5d6d7e; padding: 5px; border-radius: 3px; }"
                                 "QComboBox::drop-down { background-color: #34495e; }"
                                 "QComboBox QAbstractItemView { background-color: #34495e; color: #ffffff; selection-background-color: #3498db; }");

            if (!savedValue.isEmpty()) {
                combo->setCurrentIndex(savedValue.toInt());
            }

            field.widget = combo;
            fieldLayout->addWidget(combo);
        } else {
            QLineEdit *lineEdit = new QLineEdit();
            lineEdit->setObjectName(field.fieldName);
            lineEdit->setPlaceholderText(field.fieldType == "int" || field.fieldType == "long long" ? "Введите целое число" : "Введите число");
            lineEdit->setStyleSheet("QLineEdit { "
                                   "background-color: #34495e; "
                                   "color: #ffffff; "
                                   "border: 1px solid #5d6d7e; "
                                   "border-radius: 4px; "
                                   "padding: 6px; "
                                   "} "
                                   "QLineEdit:focus { border: 1px solid #3498db; }"
                                   "QLineEdit::placeholder { color: #95a5a6; }");

            if (!savedValue.isEmpty()) {
                lineEdit->setText(savedValue);
            }

            field.widget = lineEdit;
            fieldLayout->addWidget(lineEdit);
        }

        fieldLayout->addStretch();
        QWidget *fieldWidget = new QWidget();
        fieldWidget->setStyleSheet("QWidget { background-color: transparent; }");
        fieldWidget->setLayout(fieldLayout);
        rightLayout->addWidget(fieldWidget);
    }

    QPushButton *applyButton = new QPushButton("Сохранить параметры");
    applyButton->setStyleSheet("QPushButton { "
                               "background-color: #3498db; "
                               "color: white; "
                               "padding: 10px; "
                               "border-radius: 5px; "
                               "font-weight: bold; "
                               "} "
                               "QPushButton:hover { background-color: #2980b9; }");
    connect(applyButton, &QPushButton::clicked, this, &ParamDialog::onApplyButtonClicked);
    rightLayout->addWidget(applyButton);

    QPushButton *showButton = new QPushButton("Показать все параметры");
    showButton->setStyleSheet("QPushButton { "
                              "background-color: #27ae60; "
                              "color: white; "
                              "padding: 10px; "
                              "border-radius: 5px; "
                              "font-weight: bold; "
                              "} "
                              "QPushButton:hover { background-color: #229954; }");
    connect(showButton, &QPushButton::clicked, this, &ParamDialog::onShowParametersClicked);
    rightLayout->addWidget(showButton);

    rightLayout->addStretch();
}

void ParamDialog::collectAndSaveValues()
{
    if (!structFields.contains(currentStructName)) return;
    auto &fields = structFields[currentStructName];

    if (!allParameters.contains(currentStructName)) {
        allParameters[currentStructName] = QMap<QString, QString>();
    }

    QString message = QString("Сохранены параметры структуры %1:\n\n").arg(currentStructName);

    for (auto &field : fields) {
        if (field.widget) {
            QString value;

            if (field.fieldType == "bool") {
                QComboBox *combo = qobject_cast<QComboBox*>(field.widget);
                if (combo) {
                    value = QString::number(combo->currentIndex());
                    message += QString("%1: %2\n").arg(field.fieldName, combo->currentText());
                }
            } else {
                QLineEdit *lineEdit = qobject_cast<QLineEdit*>(field.widget);
                if (lineEdit) {
                    value = lineEdit->text();
                    if (value.isEmpty()) {
                        message += QString("%1: (не задано)\n").arg(field.fieldName);
                    } else {
                        message += QString("%1: %2\n").arg(field.fieldName, value);
                    }
                }
            }

            allParameters[currentStructName][field.fieldName] = value;
        }
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Параметры сохранены");
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet("QMessageBox { background-color: #1a1a2e; color: #ffffff; }"
                         "QMessageBox QLabel { color: #ffffff; }"
                         "QMessageBox QPushButton { background-color: #3498db; color: white; padding: 6px 15px; border-radius: 4px; }"
                         "QMessageBox QPushButton:hover { background-color: #2980b9; }");
    msgBox.exec();
}

void ParamDialog::showAllParameters()
{
    if (allParameters.isEmpty()) {
        QMessageBox::information(this, "Параметры", "Нет сохраненных параметров!");
        return;
    }

    QString message = "=== ВСЕ СОХРАНЕННЫЕ ПАРАМЕТРЫ ===\n\n";

    for (auto structIt = allParameters.begin(); structIt != allParameters.end(); ++structIt) {
        message += QString("--- %1 ---\n").arg(structIt.key());
        for (auto fieldIt = structIt.value().begin(); fieldIt != structIt.value().end(); ++fieldIt) {
            QString value = fieldIt.value().isEmpty() ? "(не задано)" : fieldIt.value();
            message += QString("  %1: %2\n").arg(fieldIt.key(), value);
        }
        message += "\n";
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Все параметры");
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet("QMessageBox { background-color: #1a1a2e; color: #ffffff; min-width: 400px; }"
                         "QMessageBox QLabel { color: #ffffff; font-family: monospace; }"
                         "QMessageBox QPushButton { background-color: #3498db; color: white; padding: 6px 15px; border-radius: 4px; }"
                         "QMessageBox QPushButton:hover { background-color: #2980b9; }");
    msgBox.exec();
}

void ParamDialog::onApplyButtonClicked()
{
    collectAndSaveValues();
}

void ParamDialog::onShowParametersClicked()
{
    showAllParameters();
}

void ParamDialog::onComboBoxChanged(int index) {
    if (index >= 0) {
        comboBox_2->blockSignals(true); comboBox_2->setCurrentIndex(-1); comboBox_2->blockSignals(false);
        comboBox_3->blockSignals(true); comboBox_3->setCurrentIndex(-1); comboBox_3->blockSignals(false);
        displayStructFields(comboBox->currentText());
    }
}

void ParamDialog::onComboBox2Changed(int index) {
    if (index >= 0) {
        comboBox->blockSignals(true); comboBox->setCurrentIndex(-1); comboBox->blockSignals(false);
        comboBox_3->blockSignals(true); comboBox_3->setCurrentIndex(-1); comboBox_3->blockSignals(false);
        displayStructFields(comboBox_2->currentText());
    }
}

void ParamDialog::onComboBox3Changed(int index) {
    if (index >= 0) {
        comboBox->blockSignals(true); comboBox->setCurrentIndex(-1); comboBox->blockSignals(false);
        comboBox_2->blockSignals(true); comboBox_2->setCurrentIndex(-1); comboBox_2->blockSignals(false);
        displayStructFields(comboBox_3->currentText());
    }
}

QMap<QString, QMap<QString, QString>> ParamDialog::getParameters() const
{
    return allParameters;
}
