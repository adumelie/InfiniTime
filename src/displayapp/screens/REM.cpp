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

    timeLeftLabel = lv_label_create(lv_scr_act(), nullptr);
    lv_obj_align(timeLeftLabel, nullptr, LV_ALIGN_CENTER, 0, -100);


    lv_obj_t* startBtn = lv_label_create(btn, nullptr);
    lv_label_set_text(startBtn, "NRRR");
    lv_obj_set_user_data(btn, this); // Set REM instance as user data
    lv_obj_set_event_cb(btn, btnEventHandler);
    motorController.Init();

    taskRefresh = lv_task_create(RefreshTaskCallback, 100, LV_TASK_PRIO_MID, this);
}

REM::~REM() {
    motorController.StopRinging(); // Make sure to stop any ongoing vibrations
    lv_obj_clean(lv_scr_act());
}

void REM::Refresh() {
    TickType_t remainingTimeInTicks = motorController.GetRemainingREMHeuristicTime();
    const uint16_t remainingTimeInSeconds = remainingTimeInTicks / (configTICK_RATE_HZ); // Truncation towards 0

    uint8_t minutes  = remainingTimeInSeconds / 60;
    uint8_t seconds = remainingTimeInSeconds % 60;

    char timeLeftStr[6];
    snprintf(timeLeftStr, sizeof(timeLeftStr), "%02d:%02d", minutes, seconds);

    lv_label_set_text(timeLeftLabel, timeLeftStr);
}

void REM::OnButtonEvent(lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        motorController.hapticFeedback();
        motorController.hapticFeedback();   // Two pulse haptic feedback

        motorController.StartStimulationTask();
    }
}
