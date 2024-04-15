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

                static void btnEventHandler(lv_obj_t*, lv_event_t);
                void OnButtonEvent(lv_event_t);
                void Refresh() override;

            private:
                Pinetime::Controllers::MotorController& motorController;

                lv_obj_t* timeLeftLabel;
                lv_task_t* taskRefresh;
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