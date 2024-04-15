#pragma once

#include <FreeRTOS.h>
#include <timers.h>
#include <cstdint>

namespace Pinetime {
  namespace Controllers {

/*
      enum class StimulationTaskState {
          waiting,
          running,
          stopped
      };
*/

    class MotorController {
    public:
      MotorController() = default;

      void Init();
      void RunForDuration(uint8_t motorDuration);
      void StartRinging();
      void StopRinging();

      void hapticFeedback();
      void StartStimulationTask();
      TickType_t GetRemainingREMHeuristicTime();

      static void periodicVibrationSequence(TimerHandle_t xTimer);
      //StimulationTaskState stimulationTaskState = StimulationTaskState::stopped;

    private:
      static void Ring(TimerHandle_t xTimer);
      static void StopMotor(TimerHandle_t xTimer);
      static void StopPulse(TimerHandle_t xTimer);
      TimerHandle_t shortVib;
      TimerHandle_t longVib;

      static void pulse(TimerHandle_t xTimer);
      static void repeatSequence(TimerHandle_t xTimer);
      static void majorPulsePeriod(TimerHandle_t xTimer);
      TimerHandle_t pulseTimerEnd;
      TimerHandle_t REM_HeuristicTimer;
      TimerHandle_t repeatSequenceTimer;
      uint8_t pulseCount = 0;

    };
  }
}
