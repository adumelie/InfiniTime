#include "displayapp/screens/REM.h"
#include "components/motor/MotorController.h"

using namespace Pinetime::Applications::Screens;

void REM::btnStartEventHandler(lv_obj_t* obj, lv_event_t event) {
    REM* screen = static_cast<REM*>(obj->user_data);
    if (event == LV_EVENT_CLICKED) {
        screen->motorController.hapticFeedback();
        screen->motorController.hapticFeedback();   // Two pulse haptic feedback

        screen->motorController.StartStimulationTask();
    }
}

void REM::btnStopEventHandler(lv_obj_t* obj, lv_event_t event) {
    REM* screen = static_cast<REM*>(obj->user_data);
    if (event == LV_EVENT_CLICKED) {
        screen->motorController.hapticFeedback(); // One pulse haptic feedback

        screen->motorController.StopStimulationTask();
    }
}



REM::REM(Controllers::MotorController& motorController):
    motorController{motorController}
{
    btnStart = lv_btn_create(lv_scr_act(), nullptr);
    lv_obj_set_size(btnStart, 100, 50);
    lv_obj_align(btnStart, nullptr, LV_ALIGN_CENTER, 0, -25);
    lv_obj_t* btnStartText= lv_label_create(btnStart, nullptr);
    lv_label_set_text(btnStartText, "Start");
    lv_obj_set_user_data(btnStart, this); // Set REM instance as user data
    lv_obj_set_event_cb(btnStart, btnStartEventHandler);

    btnStop = lv_btn_create(lv_scr_act(), nullptr);
    lv_obj_set_size(btnStop, 100, 50);
    lv_obj_align(btnStop, nullptr, LV_ALIGN_CENTER, 0, 80);
    lv_obj_t* btnStopText = lv_label_create(btnStop, nullptr);
    lv_label_set_text(btnStopText, "Stop");
    lv_obj_set_user_data(btnStop, this); // Set REM instance as user data
    lv_obj_set_event_cb(btnStop, btnStopEventHandler);


    timeLeftLabel = lv_label_create(lv_scr_act(), nullptr);
    lv_obj_align(timeLeftLabel, nullptr, LV_ALIGN_CENTER, 0, -100);

    motorController.Init();

    taskRefresh = lv_task_create(RefreshTaskCallback, 100, LV_TASK_PRIO_MID, this);
}

REM::~REM() {
    motorController.StopRinging(); // Make sure to stop any ongoing vibrations
    lv_obj_clean(lv_scr_act());
    lv_task_del(taskRefresh);
}

void REM::Refresh() {
    TickType_t remainingTimeInTicks = motorController.GetRemainingREMHeuristicTime();
    const uint16_t remainingTimeInSeconds = remainingTimeInTicks / (configTICK_RATE_HZ); // Truncation towards 0

    uint16_t minutes  = remainingTimeInSeconds / 60;
    uint16_t seconds = remainingTimeInSeconds % 60;

    char timeLeftStr[16];
    snprintf(timeLeftStr, sizeof(timeLeftStr), "%02d:%02d", minutes, seconds);

    lv_label_set_text(timeLeftLabel, timeLeftStr);

    // If state is running, set color to green
    if (motorController.stimulationTaskState == Controllers::StimulationTaskState::running) {
        lv_obj_set_style_local_text_color(timeLeftLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    }
        // If state is waiting, set color to blue
    else if (motorController.stimulationTaskState == Controllers::StimulationTaskState::waiting) {
        lv_obj_set_style_local_text_color(timeLeftLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    }
        // If state is stopped, set color to red
    else {
        lv_obj_set_style_local_text_color(timeLeftLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
    }


    // If state is running or waiting grey out start button
    if (motorController.stimulationTaskState == Controllers::StimulationTaskState::running
        || motorController.stimulationTaskState == Controllers::StimulationTaskState::waiting) {
        lv_obj_set_style_local_bg_color(btnStart, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    } else {
        lv_obj_set_style_local_bg_color(btnStart, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    }
}

