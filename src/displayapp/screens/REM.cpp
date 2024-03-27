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
    lv_label_set_text(label, "VRRR");

    lv_obj_set_user_data(btn, this); // Set REM instance as user data
    lv_obj_set_event_cb(btn, btnEventHandler);
    motorController.Init();

}

REM::~REM() {
    lv_obj_clean(lv_scr_act());
    motorController.StopRinging(); // Make sure to stop any ongoing vibrations
}

void REM::OnButtonEvent(lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        startDelayToSequence();
    }
}

void REM::startDelayToSequence() {
    motorController.hapticFeedback();
    motorController.hapticFeedback();

    delayTimerHandle = xTimerCreate("DelayTimer", pdMS_TO_TICKS(5 * 1000), pdFALSE, this, periodicVibrationSequence);
    xTimerStart(delayTimerHandle, 0);
}

void REM::periodicVibrationSequence(TimerHandle_t xTimer) {

    REM *remInstance = static_cast<REM *>(pvTimerGetTimerID(xTimer));
    remInstance->motorController.hapticFeedback();
    remInstance->motorController.hapticFeedback();

    for (int i = 0; i < 4; ++i) {
        remInstance->motorController.pulse();
    }

}

void REM::pulse(TimerHandle_t xTimer) {
    REM *remInstance = static_cast<REM *>(pvTimerGetTimerID(xTimer));
    remInstance->motorController.pulse();
}
