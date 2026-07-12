QT += core gui widgets testlib
TEMPLATE = app
CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = run_tests

# Прописываем пути к исходникам приложения, чтобы тесты их видели
INCLUDEPATH += ../src \
               ../src/Handler \
               ../src/Imitator \
               ../src/UI

# Подключаем файлы логики, которые хотим протестировать (БЕЗ main.cpp приложения!)
SOURCES += Imitator.cpp \
           ../src/Handler/ADC.c \
           ../src/Imitator/UnifedTime.c \
           ../src/Imitator/NoiseGenerator.c \
           ../src/Imitator/PPPosition.c \
           ../src/Imitator/NIPPosition.c \
           ../src/Imitator/TargetPosition.c \
           ../src/Imitator/FrequencyConverter.c \
           ../src/Imitator/AzimuthSensor.c

HEADERS += ../src/Handler/ADC.h \
           ../src/Imitator/UnifedTime.h \
           ../src/Imitator/NoiseGenerator.h \
           ../src/Imitator/PPPosition.h \
           ../src/Imitator/NIPPosition.h \
           ../src/Imitator/TargetPosition.h \
           ../src/Imitator/FrequencyConverter.h \
           ../src/Imitator/AzimuthSensor.h