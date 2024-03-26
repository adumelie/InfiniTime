#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"
#include "components/motor/MotorController.h"

namespace Pinetime {
    namespace Applications {
        namespace Screens {
            class REM : public Screen {
            public:
                REM(Controllers::MotorController& motorController);
                ~REM() override;

                static void btnEventHandler(lv_obj_t* obj, lv_event_t event);
                void OnButtonEvent(lv_event_t event);
                void startDelayToSequence();


            private:
                Pinetime::Controllers::MotorController& motorController;
                static void periodicVibrationSequence(TimerHandle_t xTimer);
                void vibrationSequence();

                TimerHandle_t delayTimerHandle;

            };
        }

        template <>
        struct AppTraits<Apps::REM> {
            static constexpr Apps app = Apps::REM;
            static constexpr const char* icon = Screens::Symbols::eye;
            static Screens::Screen* Create(AppControllers& controllers) {
                return new Screens::REM(controllers.motorController);
            }
        };
    }
}