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
}

void MotorController::Ring(TimerHandle_t xTimer) {
  MotorController* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
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

void MotorController::StopMotor(TimerHandle_t /*xTimer*/) {
    nrf_gpio_pin_set(PinMap::Motor);
}

void MotorController::pulse(TimerHandle_t xTimer) {
    MotorController* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
    motorController->RunForDuration(pdMS_TO_TICKS(motorController->PULSE_TIME));

    motorController->pulseCount++;
    if (motorController->pulseCount >= 3) {
        xTimerStop(xTimer, 0);
        motorController->pulseCount = 0;
    }
}

void MotorController::hapticFeedback() {
    xTimerStart(longVib, 0); // Start the vibration
    vTaskDelay(pdMS_TO_TICKS(1000));
    xTimerStop(longVib, 0); // Stop the vibration
}

void MotorController::StopStimulationTask() {
    if (stimulationTaskState == StimulationTaskState::running
        || stimulationTaskState == StimulationTaskState::waiting) {

        xTimerStop(REM_HeuristicTimer, 0);
        xTimerStop(repeatSequenceTimer, 0);
        pulseCount = 3; // Max count to stop if currently pulsing
        stimulationTaskState = StimulationTaskState::stopped;
    }
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


void MotorController::StartStimulationTask() {
    StopStimulationTask();

    REM_HeuristicTimer = xTimerCreate("r", REM_HeuristicDelay, pdTRUE, this, periodicVibrationSequence);
    xTimerStart(REM_HeuristicTimer, 0);
    stimulationTaskState = StimulationTaskState::waiting;
}

void MotorController::periodicVibrationSequence(TimerHandle_t xTimer) {
    // Called either after heuristic of 75 min or with REM detection
    // Sequence repeated for 12 minutes of REM
    MotorController* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
    motorController->stimulationTaskState = StimulationTaskState::running;

    TickType_t repeatSequenceDelay = pdMS_TO_TICKS(30 * 1000);
    motorController->repeatSequenceTimer = xTimerCreate("vP", repeatSequenceDelay, pdTRUE, motorController, repeatSequence);
    xTimerStart(motorController->repeatSequenceTimer, 0);
}

void MotorController::repeatSequence(TimerHandle_t xTimer) {
    static uint8_t count = 0;

    MotorController* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
    count++;
    // Every 30 sec period this is called
    if (count >= 21) { // 12 min in periods of 30 sec (Counts 21-24 are do nothing)
        xTimerStop(xTimer, 0);
        motorController->stimulationTaskState = StimulationTaskState::waiting;
    }
    else if (count % 6 < 3) { // Pulse the first 3 cycles, do nothing the next three, repeat
        motorController->majorPulsePeriod();
    }
}

void MotorController::majorPulsePeriod() {
    TimerHandle_t pulseAndStopTimer = xTimerCreate("mP", pdMS_TO_TICKS(1500), pdTRUE, this, pulse);
    xTimerStart(pulseAndStopTimer, 0);
}
