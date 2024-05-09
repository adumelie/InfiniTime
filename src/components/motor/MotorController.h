#pragma once

#include <FreeRTOS.h>
#include <timers.h>
#include <cstdint>

namespace Pinetime {
  namespace Controllers {

      enum class StimulationTaskState {
          waiting,
          running,
          stopped
      };

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
      void StopStimulationTask();
      StimulationTaskState stimulationTaskState = StimulationTaskState::stopped;

      uint8_t getPulseStrength() const { return PULSE_TIME; }
      void setPulseStrength(const uint8_t pulseTime) { PULSE_TIME = pulseTime; }
      uint8_t getMaxPeriodCountInREM() const { return maxPeriodCountInREM; }
      void setMaxPeriodCountInREM(const uint8_t maxPeriodCount) { maxPeriodCountInREM = maxPeriodCount; }
      uint8_t getCycleCount() const { return cycleCount; }
      void resetCycleCount() { cycleCount = 0; }


    private:
      static void Ring(TimerHandle_t xTimer);
      static void StopMotor(TimerHandle_t xTimer);
      TimerHandle_t shortVib;
      TimerHandle_t longVib;

      static void pulse(TimerHandle_t xTimer);
      static void repeatSequence(TimerHandle_t xTimer);
      void majorPulsePeriod();
      TimerHandle_t REM_HeuristicTimer;
      TimerHandle_t repeatSequenceTimer;
      uint8_t pulseCount = 0;
      uint8_t periodCountInREM = 0;
      uint8_t cycleCount = 0;

      TickType_t REM_HeuristicDelay = 0;
      const uint8_t BASE_REM_HeuristicDelay = 75;
      const uint8_t SOP_HeuristicDelay = 15;
      const uint8_t RESOP_HeuristicDelay = 5;

      uint8_t PULSE_TIME = 5;    // Default values
      uint8_t maxPeriodCountInREM = 21;


    };
  }
}
