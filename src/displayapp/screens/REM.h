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

                static void btnStartEventHandler(lv_obj_t*, lv_event_t);
                static void btnStopEventHandler(lv_obj_t*, lv_event_t);
                static void btnToggleEventHandler(lv_obj_t*, lv_event_t);
                void Refresh() override;

                bool isNight = true;

                void setNight();
                void setDay();

                const uint8_t DAY_TIME_PULSE_STRENGTH = 50;
                const uint8_t DAY_TIME_MAX_PERIOD_COUNT_IN_REM = 3;
                const uint8_t NIGHT_TIME_PULSE_TIME = 5;
                const uint8_t NIGHT_TIME_MAX_PERIOD_COUNT_IN_REM = 24;

            private:
                Pinetime::Controllers::MotorController& motorController;

                lv_obj_t* timeLeftLabel;
                lv_task_t* taskRefresh;
                lv_obj_t* btnStart;
                lv_obj_t* btnStop;
                lv_obj_t* pulseStrengthLabel;
                lv_obj_t* maxREMPeriodCountLabel;
                lv_obj_t* btnToggle;
                lv_obj_t* btnToggleText;
                lv_obj_t* cycleCountLabel;

                void updateInfoLabels() const;
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