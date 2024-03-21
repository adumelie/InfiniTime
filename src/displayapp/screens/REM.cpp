#include "displayapp/screens/REM.h"
#include "displayapp/DisplayApp.h"
#include "components/motor/MotorController.h"

using namespace Pinetime::Applications::Screens;

void REM::btnEventHandler(lv_obj_t* obj, lv_event_t event) {
    auto* screen = static_cast<REM*>(obj->user_data);
    screen->OnButtonEvent(obj, event);
}

REM::REM() {
    lv_obj_t* btn = lv_btn_create(lv_scr_act(), nullptr);
    lv_obj_set_size(btn, 100, 50);
    lv_obj_align(btn, nullptr, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* label = lv_label_create(btn, nullptr);
    lv_label_set_text(label, "VRRR");

    lv_obj_set_user_data(btn, this); // Set REM instance as user data
    lv_obj_set_event_cb(btn, btnEventHandler);
    motorController.Init();
}

REM::~REM() {
    lv_obj_clean(lv_scr_act());
    motorController.StopRinging(); // Make sure to stop any ongoing vibrations
}

void REM::OnButtonEvent(lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        for (int i = 0; i < 3; ++i) {
            motorController.StartRinging();
            vTaskDelay(pdMS_TO_TICKS(200)); // Delay for 200 milliseconds (adjust as needed)
            motorController.StopRinging();
            vTaskDelay(pdMS_TO_TICKS(300)); // Delay for 300 milliseconds between pulses (adjust as needed)
        }
    }
}
