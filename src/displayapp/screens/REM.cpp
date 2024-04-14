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

    statusLabel = lv_label_create(lv_scr_act(), nullptr);
    lv_obj_align(statusLabel, nullptr, LV_ALIGN_CENTER, 0, -50);
    UpdateStatusLabel();

    // Create a new label for the time left
    timeLeftLabel = lv_label_create(lv_scr_act(), nullptr);
    lv_obj_align(timeLeftLabel, nullptr, LV_ALIGN_CENTER, 0, -100);
    lv_label_set_text(timeLeftLabel, "N/A");


    lv_obj_t* label = lv_label_create(btn, nullptr);
    lv_label_set_text(label, "NRRR");

    lv_obj_set_user_data(btn, this); // Set REM instance as user data
    lv_obj_set_event_cb(btn, btnEventHandler);
    motorController.Init();

    taskRefresh = lv_task_create(RefreshTaskCallback, 100, LV_TASK_PRIO_MID, this);
}

REM::~REM() {
    motorController.StopRinging(); // Make sure to stop any ongoing vibrations
    lv_obj_clean(lv_scr_act());
}

void REM::UpdateStatusLabel() {
    auto status = motorController.stimulationTaskState;

    const char* statusStr;
    switch (status) {
        case Controllers::StimulationTaskState::waiting:
            statusStr = "W";
            break;
        case Controllers::StimulationTaskState::running:
            statusStr = "R";
            break;
        case Controllers::StimulationTaskState::stopped:
            statusStr = "S";
            break;

        default :
            statusStr = "NA";
    }
    lv_label_set_text(statusLabel, statusStr);
}

void REM::Refresh() {
    // Get the remaining time on the heuristic timer
    TickType_t remainingTimeInTicks = motorController.GetRemainingREMHeuristicTime();
    double remainingTimeInSeconds = remainingTimeInTicks / static_cast<double>(configTICK_RATE_HZ);

    // Convert the remaining time to minutes and seconds
    int minutes = static_cast<int>(remainingTimeInSeconds) / 60;
    int seconds = static_cast<int>(remainingTimeInSeconds) % 60;

    // Convert the remaining time to a string
    char timeLeftStr[32];
    snprintf(timeLeftStr, sizeof(timeLeftStr), "%02d:%02d", minutes, seconds);

    // Set the label text to the remaining time
    lv_label_set_text(timeLeftLabel, timeLeftStr);
}

void REM::OnButtonEvent(lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        motorController.hapticFeedback();
        motorController.hapticFeedback();   // Two pulse haptic feedback

        motorController.StartStimulationTask();
        UpdateStatusLabel();
    }
}

void REM::OnButtonEventSTOP(lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        motorController.StopRinging();
    }
}

void REM::btnEventHandlerSTOP(lv_obj_t* obj, lv_event_t event) {
    auto* screen = static_cast<REM*>(obj->user_data);
    screen->OnButtonEventSTOP(event);
}