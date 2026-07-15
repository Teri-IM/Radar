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
        Handler/ADC.c \
        Handler/DDC.c \
        Handler/codogramm.c \
        Handler/nn.c \
        Handler/processing_module.c \
        Handler/suppression_NIP.c \
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
    Handler/ADC.h \
    Handler/DDC.h \
    Handler/DDC_param.h \
    Handler/codogramm.h \
    Handler/nn.h \
    Handler/processing_module.h \
    Handler/processing_module_param.h \
    Handler/suppression_NIP.h \
    Handler/suppression_NIP_Param.h \
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


QMAKE_CFLAGS_RELEASE += -O3 -ftree-vectorize
QMAKE_CXXFLAGS_RELEASE += -O3 -ftree-vectorize

QMAKE_CFLAGS_DEBUG += -O3 -march=native -ftree-vectorize
QMAKE_CXXFLAGS_DEBUG += -O3 -march=native -ftree-vectorize
