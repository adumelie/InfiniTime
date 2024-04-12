#pragma once

#include <FreeRTOS.h>
#include <timers.h>
#include <cstdint>

namespace Pinetime {
  namespace Controllers {

    class MotorController {
    public:
      MotorController() = default;

      void Init();
      void RunForDuration(uint8_t motorDuration);
      void StartRinging();
      void StopRinging();
      void pulse();
      void hapticFeedback();

      static void periodicVibrationSequence(TimerHandle_t xTimer);

    private:
      static void Ring(TimerHandle_t xTimer);
      static void StopMotor(TimerHandle_t xTimer);
      static void StopPulse(TimerHandle_t xTimer);
      TimerHandle_t shortVib;
      TimerHandle_t longVib;
      TimerHandle_t pulseTimerEnd;


        static void pulsePeriod(TimerHandle_t xTimer);
      static void majorPulsePeriod(TimerHandle_t xTimer);
      static void minorPulsePeriod(TimerHandle_t xTimer);
    };
  }
}
