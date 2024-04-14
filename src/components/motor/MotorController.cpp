#include "components/motor/MotorController.h"
#include <hal/nrf_gpio.h>
#include "systemtask/SystemTask.h"
#include "drivers/PinMap.h"

using namespace Pinetime::Controllers;

void MotorController::Init() {
  nrf_gpio_cfg_output(PinMap::Motor);
  nrf_gpio_pin_set(PinMap::Motor);

  shortVib = xTimerCreate("shortVib", 1, pdFALSE, nullptr, StopMotor);
  longVib = xTimerCreate("longVib", pdMS_TO_TICKS(1000), pdTRUE, this, Ring);
  pulseTimerEnd = xTimerCreate("pulseTimer", pdMS_TO_TICKS(5000), pdFALSE, this, StopPulse);
}

void MotorController::Ring(TimerHandle_t xTimer) {
  auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
  motorController->RunForDuration(50);
}

void MotorController::RunForDuration(uint8_t motorDuration) {
  if (motorDuration > 0 && xTimerChangePeriod(shortVib, pdMS_TO_TICKS(motorDuration), 0) == pdPASS && xTimerStart(shortVib, 0) == pdPASS) {
    nrf_gpio_pin_clear(PinMap::Motor);
  }
}

void MotorController::StartRinging() {
  RunForDuration(50);
  xTimerStart(longVib, 0);
}

void MotorController::StopRinging() {
  xTimerStop(longVib, 0);
  nrf_gpio_pin_set(PinMap::Motor);
}

void MotorController::pulse() {
    RunForDuration(pdMS_TO_TICKS(5));
}

void MotorController::hapticFeedback() {
    xTimerStart(longVib, 0); // Start the vibration
    vTaskDelay(pdMS_TO_TICKS(1000));
    xTimerStop(longVib, 0); // Stop the vibration
}

void MotorController::StopPulse(TimerHandle_t xTimer) {
    auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
    motorController->StopRinging();
}

void MotorController::StopMotor(TimerHandle_t /*xTimer*/) {
  nrf_gpio_pin_set(PinMap::Motor);
}

void MotorController::StartStimulationTask() {
    // Calculate delay in ticks manually, without using ms nor pdMS_To_Ticks to avoid integer overflow
    // auto REM_HeuristicDelay = 75 * configTICK_RATE_HZ * 60;
    auto REM_HeuristicDelay = 0.5 * configTICK_RATE_HZ * 60;

    REM_HeuristicTimer = xTimerCreate("r", REM_HeuristicDelay, pdTRUE, this, periodicVibrationSequence);
    xTimerStart(REM_HeuristicTimer, 0);
    stimulationTaskState = StimulationTaskState::waiting;
}

TickType_t MotorController::GetRemainingREMHeuristicTime() {
    if (stimulationTaskState == StimulationTaskState::stopped) {
        return 0;
    }
    else if (stimulationTaskState == StimulationTaskState::waiting) {
        return xTimerGetExpiryTime(REM_HeuristicTimer) - xTaskGetTickCount();
    }
    else { // Running
        return xTimerGetExpiryTime(repeatSequenceTimer) - xTaskGetTickCount();
    }
}


void MotorController::periodicVibrationSequence(TimerHandle_t xTimer) {
    // Called either after heuristic of 75 min or with REM detection
    // Pulses in REM, while in REM start sequence every 3 minutes for 4 times ( 4 * pulsePeriod calls)
    // Sequence repeated for 3*4 = 12 minutes of REM
    // Sequence: 3 pulses in 3 seconds, 4 times seperated by 30 seconds
    auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
    motorController->stimulationTaskState = StimulationTaskState::running;

    auto repeatSequenceDelay = pdMS_TO_TICKS(3 * 60 * 1000);
    motorController->repeatSequenceTimer = xTimerCreate("vP", repeatSequenceDelay, pdTRUE, motorController, repeatSequence);
    xTimerStart(motorController->repeatSequenceTimer, 0);
}

void MotorController::repeatSequence(TimerHandle_t xTimer) {
    static uint8_t count = 0;
    auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
    auto numberOfPulseCycles = 4;

    count++;
    if (count >= numberOfPulseCycles) {
        xTimerStop(xTimer, 0);  // Stop vibrationPeriod timer after 4 cycles of 3 minutes
        count = 0;

        TimerHandle_t sequenceEvery30Seconds = xTimerCreate("rS", pdMS_TO_TICKS(30 * 1000), pdTRUE, motorController, majorPulsePeriod);
        xTimerStart(sequenceEvery30Seconds, 0);
    }
}

void MotorController::majorPulsePeriod(TimerHandle_t xTimer) {
    static uint8_t count = 0;
    auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));

    count++;
    if (count >= 1) {
        xTimerStop(xTimer, 0);  // Stop vibrationPeriod timer after
        count = 0;

        TimerHandle_t pulseAndStopTimer = xTimerCreate("mP", pdMS_TO_TICKS(1500), pdTRUE, motorController, minorPulsePeriod);
        xTimerStart(pulseAndStopTimer, 0);
    }
}

void MotorController::minorPulsePeriod(TimerHandle_t xTimer) {
    static uint8_t count = 0;
    auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
    motorController->pulse();

    count++;
    if (count >= 3) {
        xTimerStop(xTimer, 0);  // Stop minor (3 pulses) pulse period
        count = 0;
    }

}