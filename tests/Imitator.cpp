#include <QtTest>
#include <QCoreApplication>
#include <vector>
#include <cmath>
#include <chrono>
#include <iostream>

extern "C" {
#include "ADC.h"
#include "UnifedTime.h"
#include "NoiseGenerator.h"
#include "PPPosition.h"
#include "NIPPosition.h"
#include "TargetPosition.h"
#include "FrequencyConverter.h"
#include "AzimuthSensor.h"
}

namespace {
template<typename TestFunc>
void runTimedTest(const char* testName, TestFunc&& testFunc)
{
    auto start = std::chrono::high_resolution_clock::now();
    testFunc();
    auto finish = std::chrono::high_resolution_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
    std::cout << "[timing] " << testName << ": " << elapsedMs << " us" << std::endl;
}
}

class ImitatorTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {
    }

    void test_UnifiedTime_null_parameter() {
        runTimedTest("test_UnifiedTime_null_parameter", [this]() {
            struct UTimeParam params = {};
            struct UnifedTimeOut output = {};

            QVERIFY2(timeTick(nullptr, &output) == 1, "timeTick должен возвращать ошибку для null-параметра");
        });
    }

    void test_UnifiedTime_updates_time_fields() {
        runTimedTest("test_UnifiedTime_updates_time_fields", [this]() {
            struct UTimeParam params = {};
            struct UnifedTimeOut output = {};
            struct timespec past;

            clock_gettime(CLOCK_MONOTONIC, &past);
            past.tv_sec -= 2;
            output.start_time = past;

            int result = timeTick(&params, &output);

            QVERIFY2(result == 0, "timeTick должен успешно обновлять структуру времени");
            QVERIFY2(output.total_ns >= 1'900'000'000ULL, "Общее время должно быть не меньше двух секунд");
            QVERIFY2(output.nanoseconds < 1'000'000'000U, "Наносекунды должны быть в допустимом диапазоне");
            QVERIFY2(output.days == 0, "Для короткого интервала дни должны быть равны 0");
        });
    }

    void test_NoiseGenerator_disabled_module_sets_zero() {
        runTimedTest("test_NoiseGenerator_disabled_module_sets_zero", [this]() {
            struct NoiseParam params = {0, 0.0, 0.0};
            struct UnifedTimeOut input = {};
            input.sampling_cnt = 8;
            std::vector<float> buffer(8, 0.0f);
            struct NoiseGeneratorOut output = {buffer.data()};

            int result = generateNoise(&params, &input, &output);

            QVERIFY2(result == 0, "generateNoise должен успешно обработать отключённый модуль");
            for (int i = 0; i < 8; ++i) {
                QCOMPARE(output.noise_signals[i], 0.0f);
            }
        });
    }

    void test_NoiseGenerator_with_zero_sigma_returns_mean() {
        runTimedTest("test_NoiseGenerator_with_zero_sigma_returns_mean", [this]() {
            struct NoiseParam params = {1, 3.0, 0.0};
            struct UnifedTimeOut input = {};
            input.sampling_cnt = 6;
            std::vector<float> buffer(6, 0.0f);
            struct NoiseGeneratorOut output = {buffer.data()};

            int result = generateNoise(&params, &input, &output);

            QVERIFY2(result == 0, "generateNoise должен корректно обработать параметры шума");
            for (int i = 0; i < 6; ++i) {
                QCOMPARE(output.noise_signals[i], 3.0f);
            }
        });
    }

    void test_PPPositionMap_disables_points_when_module_is_off() {
        runTimedTest("test_PPPositionMap_disables_points_when_module_is_off", [this]() {
            struct PPPosParam params = {0, 3, 2.5f};
            struct ImitParam imitatorParams = {10.0f};
            std::vector<Point> map(3);
            struct PPPositionOut output = {map.data(), 0};

            int result = PPPositionMap(&params, &imitatorParams, &output);

            QVERIFY2(result == 0, "PPPositionMap должен корректно обрабатывать отключённый модуль");
            QCOMPARE(output.cntPP, 0);
            for (int i = 0; i < 3; ++i) {
                QCOMPARE(output.PP_map[i].angle, 0.0f);
                QCOMPARE(output.PP_map[i].distance, 0.0f);
                QCOMPARE(output.PP_map[i].amplitude, 0.0f);
            }
        });
    }

    void test_TargetPositionMap_populates_target_map() {
        runTimedTest("test_TargetPositionMap_populates_target_map", [this]() {
            struct TargetPosParam params = {1, 2, 4.0f};
            struct ImitParam imitatorParams = {12.0f};
            std::vector<Point> map(2);
            struct TargetPositionOut output = {map.data(), 0};

            int result = TargetPositionMap(&params, &imitatorParams, &output);

            QVERIFY2(result == 0, "TargetPositionMap должен успешно сформировать карту целей");
            QCOMPARE(output.cntTarget, 2);
            for (int i = 0; i < 2; ++i) {
                QVERIFY2(output.Target_map[i].amplitude == params.Targetamplitude, "Амплитуда цели должна совпадать с параметрами");
                QVERIFY2(output.Target_map[i].distance >= 1.0f, "Дистанция цели должна быть положительной");
                QVERIFY2(output.Target_map[i].angle >= 0.0f && output.Target_map[i].angle <= 360.0f, "Угол цели должен быть в диапазоне [0, 360)");
            }
        });
    }

    void test_AzimuthSensor_updates_angle() {
        runTimedTest("test_AzimuthSensor_updates_angle", [this]() {
            struct AzimutParam params = {1, 10.0f, 360.0f};
            struct UTimeParam timeParams = {};
            struct UnifedTimeOut timeInput = {};
            struct AzimutSensorOut output = {10.0f, 0.0f, 0.0f};
            timeParams.probing_time = 50000LL;

            int result = AzimuthSensor(&params, &timeParams, &timeInput, &output, 1);

            QVERIFY2(result == 0, "AzimuthSensor должен обновлять азимут без ошибок");
            QVERIFY2(std::fabs(output.azimuth_new - 10.0f) < 1e-5f, "Азимут должен обновляться согласно формуле модуля");
        });
    }

    void test_FrequencyConverter_doubles_signal_values() {
        runTimedTest("test_FrequencyConverter_doubles_signal_values", [this]() {
            struct FreqConvertorParam params = {1};
            struct UnifedTimeOut timeInput = {};
            timeInput.sampling_cnt = 4;
            std::vector<float> signal_values = {1.0f, -2.0f, 3.5f, 0.0f};
            struct ImitSummatorOut output = {signal_values.data()};

            int result = FrequencyConverter(&params, &timeInput, &output);

            QVERIFY2(result == 0, "FrequencyConverter должен удваивать значения сигналов");
            QCOMPARE(output.sum_signals[0], 2.0f);
            QCOMPARE(output.sum_signals[1], -4.0f);
            QCOMPARE(output.sum_signals[2], 7.0f);
            QCOMPARE(output.sum_signals[3], 0.0f);
        });
    }

    void test_CreateNipMap_disables_map_when_module_is_off() {
        runTimedTest("test_CreateNipMap_disables_map_when_module_is_off", [this]() {
            struct NIPPosParam params = {0, 2, 1.5f, 100, nullptr};
            struct ImitParam imitatorParams = {8.0f};
            std::vector<NIPPointParam> generated(2);
            params.NIPMap = generated.data();

            int result = createNipMap(&params, &imitatorParams);

            QVERIFY2(result == 0, "createNipMap должен корректно обрабатывать отключённый модуль");
            for (int i = 0; i < 2; ++i) {
                QCOMPARE(generated[i].angle, 0.0f);
                QCOMPARE(generated[i].distance, 0.0f);
                QCOMPARE(generated[i].NIPprobing_time, 0LL);
            }
        });
    }

    void cleanupTestCase() {
    }
};

QTEST_MAIN(ImitatorTest)
#include "Imitator.moc"