#include "displayapp/screens/REM.h"
#include "components/motor/MotorController.h"

using namespace Pinetime::Applications::Screens;

void REM::btnEventHandler(lv_obj_t* obj, lv_event_t event) {
    auto* screen = static_cast<REM*>(obj->user_data);
    screen->OnButtonEvent(event);
}

REM::REM(Controllers::MotorController& motorController):
    motorController{motorController}
{

    lv_obj_t* btn = lv_btn_create(lv_scr_act(), nullptr);
    lv_obj_set_size(btn, 100, 50);
    lv_obj_align(btn, nullptr, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* label = lv_label_create(btn, nullptr);
    lv_label_set_text(label, "NRRR");

    lv_obj_set_user_data(btn, this); // Set REM instance as user data
    lv_obj_set_event_cb(btn, btnEventHandler);
    motorController.Init();

    // 2nd button
    lv_obj_t* btn2 = lv_btn_create(lv_scr_act(), nullptr);
    lv_obj_set_size(btn2, 100, 50);
    lv_obj_align(btn2, nullptr, LV_ALIGN_CENTER, 0, 100);
    // set text and user data
    lv_obj_t* label2 = lv_label_create(btn2, nullptr);
    lv_obj_set_style_local_bg_color(btn2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_label_set_text(label2, "STOP");
    lv_obj_set_user_data(btn2, this);
    lv_obj_set_event_cb(btn2, btnEventHandlerSTOP);
}

REM::~REM() {
    motorController.StopRinging(); // Make sure to stop any ongoing vibrations
    lv_obj_clean(lv_scr_act());
}

void REM::OnButtonEvent(lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        motorController.hapticFeedback();
        motorController.hapticFeedback();   // Two pulse haptic feedback

        // Calculate delay in ticks manually, without using ms nor pdMS_To_Ticks to avoid integer overflow
        auto REM_HeuristicDelay = 75 * configTICK_RATE_HZ * 60;

        REM_HeuristicTimer = xTimerCreate("DelayTimer", REM_HeuristicDelay, pdTRUE, &motorController, Controllers::MotorController::periodicVibrationSequence);
        xTimerStart(REM_HeuristicTimer, 0);

    }
}

void REM::OnButtonEventSTOP(lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        motorController.StopRinging();
        xTimerStop(REM_HeuristicTimer, 0);
    }
}

void REM::btnEventHandlerSTOP(lv_obj_t* obj, lv_event_t event) {
    auto* screen = static_cast<REM*>(obj->user_data);
    screen->OnButtonEventSTOP(event);
}