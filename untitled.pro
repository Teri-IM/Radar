#-------------------------------------------------
#
# Project created by QtCreator 2026-04-15T18:38:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        Handler/src/ADC.c \
        Handler/src/DDC.c \
        Handler/src/codogramm.c \
        Handler/src/nn.c \
        Handler/src/processing_module.c \
        Handler/src/suppression_NIP.c \
        Handler/src/threshold_generator.c \
        Handler/src/threshold_device.c \
        Imitator/AzimuthSensor.c \
        Imitator/ClutterFormationSignals.c \
        Imitator/ClutterResponse.c \
        Imitator/FrequencyConverter.c \
        Imitator/ImitSummator.c \
        Imitator/Imitator.c \
        Imitator/NIPFormationSignals.c \
        Imitator/NIPLevel.c \
        Imitator/NIPPosition.c \
        Imitator/NoiseGenerator.c \
        Imitator/PPPosition.c \
        Imitator/TargetFormationSignals.c \
        Imitator/TargetPosition.c \
        Imitator/TargetResponse.c \
        Imitator/UnifedTime.c \
        widget.cpp

HEADERS  += widget.h \
    Handler/include/ADC.h \
    Handler/include/DDC.h \
    Handler/include/codogramm.h \
    Handler/include/nn.h \
    Handler/include/processing_module.h \
    Handler/include/processing_module_param.h \
    Handler/include/suppression_NIP.h \
    Handler/include/threshold.h \
    Imitator/AzimuthSensor.h \
    Imitator/AzimuthSensorParam.h \
    Imitator/ClutterFormationSignals.h \
    Imitator/ClutterFormationSignalsParam.h \
    Imitator/ClutterResponse.h \
    Imitator/ClutterResponseParams.h \
    Imitator/FrequencyConverter.h \
    Imitator/FrequencyConverterParam.h \
    Imitator/ImitSummator.h \
    Imitator/ImitSummatorParam.h \
    Imitator/Imitator.h \
    Imitator/ImitatorParam.h \
    Imitator/NIPFormationSignals.h \
    Imitator/NIPFormationSignalsParam.h \
    Imitator/NIPLevel.h \
    Imitator/NIPLevelParam.h \
    Imitator/NIPPosition.h \
    Imitator/NIPPositionParam.h \
    Imitator/NoiseGenerator.h \
    Imitator/NoiseGeneratorParam.h \
    Imitator/PPPosition.h \
    Imitator/PPPositionParam.h \
    Imitator/Points.h \
    Imitator/TargetFormationSignals.h \
    Imitator/TargetFormationSignalsParam.h \
    Imitator/TargetPosition.h \
    Imitator/TargetPositionParam.h \
    Imitator/TargetResponse.h \
    Imitator/TargetResponseParam.h \
    Imitator/UnifedTime.h \
    Imitator/UnifiedImitatorParam.h \
    Imitator/UnifiedTimeParam.h \
    UnifiedImitatorParam.h

FORMS    += widget.ui
