#include "displayapp/screens/REM.h"
#include "components/motor/MotorController.h"

using namespace Pinetime::Applications::Screens;

void REM::btnEventHandler(lv_obj_t* obj, lv_event_t event) {
    auto* screen = static_cast<REM*>(obj->user_data);
    screen->OnButtonEvent(event);
}

void REM::test(TimerHandle_t xTimer) {
    printf("Callback\n");
    (void)xTimer;
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
    xTimerDelete(delayTimerHandle, 0); // Make sure to stop timer
    lv_obj_clean(lv_scr_act());
}

void REM::OnButtonEvent(lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        startDelayToSequence();
    }
}

void REM::OnButtonEventSTOP(lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        motorController.StopRinging();
        xTimerStop(delayTimerHandle, 0);
    }
}

void REM::btnEventHandlerSTOP(lv_obj_t* obj, lv_event_t event) {
    auto* screen = static_cast<REM*>(obj->user_data);
    screen->OnButtonEventSTOP(event);
}

void REM::startDelayToSequence() {
    motorController.hapticFeedback();
    motorController.hapticFeedback();

    // Timer REM heuristic (70 + 5 min SOP), repeat after 70 + 5 min also
    // auto delay = pdMS_TO_TICKS(5 * 60 * 1000); // 5 min (uint32)

    auto delay = pdMS_TO_TICKS( 60 * 1000); // TMP for testing 1 min

    delayTimerHandle = xTimerCreate("DelayTimer", delay, pdTRUE, this, periodicVibrationSequence);
    xTimerStart(delayTimerHandle, 0);
}

void REM::periodicVibrationSequence(TimerHandle_t xTimer) {
    static uint8_t count = 0; // Breaking 75 min into chunks that fit into uint32_t, not neat but OK for personal build
    REM *remInstance = static_cast<REM *>(pvTimerGetTimerID(xTimer));

    count++;
    if (count >= 1) {  // 15 * 5 min = 75 min
        count = 0;     // Reset for another 75 min

        TimerHandle_t repeatPulseTimer = xTimerCreate("repeatPulseTimer", pdMS_TO_TICKS(2000), pdTRUE, remInstance, repeatPulse);
        xTimerStart(repeatPulseTimer, 0);
    }
}

void REM::repeatPulse(TimerHandle_t xTimer) {
    /*
     * Repeat the pulse 5 times
     * The function is called every X seconds by the timer
     * and will stop after 5 times.
     */
    static uint8_t count = 0;
    REM *remInstance = static_cast<REM *>(pvTimerGetTimerID(xTimer));
    remInstance->motorController.pulse();

    count++;
    if (count >= 5) {
        xTimerStop(xTimer, 0);
        count = 0;
    }
}
