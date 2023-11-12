#include "displayapp/screens/WatchFaceBinaryBCD.h"

#include <lvgl/lvgl.h>
#include <cstdio>
#include "displayapp/screens/NotificationIcon.h"
#include "displayapp/screens/Symbols.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"
#include "components/settings/Settings.h"

using namespace Pinetime::Applications::Screens;

WatchFaceBinaryBCD::WatchFaceBinaryBCD(Controllers::DateTime& dateTimeController,
                                 const Controllers::Battery& batteryController,
                                 const Controllers::Ble& bleController,
                                 Controllers::NotificationManager& notificationManager,
                                 Controllers::Settings& settingsController,
                                 Controllers::HeartRateController& heartRateController,
                                 Controllers::MotionController& motionController)
  : currentDateTime {{}},
    dateTimeController {dateTimeController},
    notificationManager {notificationManager},
    settingsController {settingsController},
    heartRateController {heartRateController},
    motionController {motionController},
    statusIcons(batteryController, bleController) {

  statusIcons.Create();

  notificationIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(notificationIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_LIME);
  lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(false));
  lv_obj_align(notificationIcon, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  lableDate = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_align(lableDate, lv_scr_act(), LV_ALIGN_CENTER, 0, 70);
  lv_obj_set_style_local_text_color(lableDate, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x999999));

  constexpr lv_coord_t pointSize = 28;
#if BINARY_SECONDS_ENABLED
  constexpr lv_coord_t widthSpacer = 8;
  auto innerSpacing = (lv_coord_t) ((LV_HOR_RES - (pointSize * 6 + widthSpacer * 2)) / 7);
  auto outerSpacing = (lv_coord_t) (innerSpacing * 2);
#else
  constexpr lv_coord_t widthSpacer = 42;
  auto innerSpacing = (lv_coord_t) ((LV_HOR_RES - (pointSize * 4 + widthSpacer * 2)) / 6);
  auto outerSpacing = (lv_coord_t) (innerSpacing * 4);
#endif

  constexpr lv_coord_t offsetY = pointSize * 1.25;
  constexpr lv_coord_t bottomY = 32 + offsetY * 3;

  for (uint8_t i = 0; i < 4; i++) {
    const auto currentY = (lv_coord_t) (bottomY - offsetY * i);
    // Hours
    // High
    if (2 > i) {
      hourPoints[i] = lv_obj_create(lv_scr_act(), nullptr);

      lv_obj_set_style_local_bg_color(hourPoints[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
      lv_obj_set_style_local_radius(hourPoints[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
      lv_obj_set_size(hourPoints[i], pointSize, pointSize);
      lv_obj_set_pos(hourPoints[i], widthSpacer, currentY);
    }
    // Low
    hourPoints[i + 2] = lv_obj_create(lv_scr_act(), nullptr);

    lv_obj_set_style_local_bg_color(hourPoints[i + 2], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
    lv_obj_set_style_local_radius(hourPoints[i + 2], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_obj_set_size(hourPoints[i + 2], pointSize, pointSize);
    lv_obj_set_pos(hourPoints[i + 2], (lv_coord_t) (widthSpacer + pointSize + innerSpacing), currentY);

    // Minutes
    // High
    if (3 > i) {
      minutePoints[i] = lv_obj_create(lv_scr_act(), nullptr);
      lv_obj_set_style_local_bg_color(minutePoints[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
      lv_obj_set_style_local_radius(minutePoints[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
      lv_obj_set_size(minutePoints[i], pointSize, pointSize);
      lv_obj_set_pos(minutePoints[i], (lv_coord_t) (widthSpacer + pointSize * 2 + innerSpacing + outerSpacing), currentY);
    }
    // Low
    minutePoints[i + 3] = lv_obj_create(lv_scr_act(), nullptr);
    lv_obj_set_style_local_bg_color(minutePoints[i + 3], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
    lv_obj_set_style_local_radius(minutePoints[i + 3], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_obj_set_size(minutePoints[i + 3], pointSize, pointSize);
    lv_obj_set_pos(minutePoints[i + 3], (lv_coord_t) (widthSpacer + pointSize * 3 + innerSpacing * 2 + outerSpacing), currentY);

// Seconds
#if BINARY_SECONDS_ENABLED
    // High
    if (3 > i) {
      secondPoints[i] = lv_obj_create(lv_scr_act(), nullptr);
      lv_obj_set_style_local_bg_color(secondPoints[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
      lv_obj_set_style_local_radius(secondPoints[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
      lv_obj_set_size(secondPoints[i], pointSize, pointSize);
      lv_obj_set_pos(secondPoints[i], (lv_coord_t) (widthSpacer + pointSize * 4 + innerSpacing * 2 + outerSpacing * 2), currentY);
    }
    // Low
    secondPoints[i + 3] = lv_obj_create(lv_scr_act(), nullptr);
    lv_obj_set_style_local_bg_color(secondPoints[i + 3], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
    lv_obj_set_style_local_radius(secondPoints[i + 3], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_obj_set_size(secondPoints[i + 3], pointSize, pointSize);
    lv_obj_set_pos(secondPoints[i + 3], (lv_coord_t) (widthSpacer + pointSize * 5 + innerSpacing * 3 + outerSpacing * 2), currentY);
#endif
  }

  constexpr lv_coord_t tflOffsetX = 4;
  constexpr lv_coord_t tflSpacing = 4;
  constexpr auto tflHeight = (lv_coord_t) (pointSize / 2 - tflSpacing);

  const lv_coord_t labelAMpmX = widthSpacer + 1;
  constexpr auto labelAMy = (lv_coord_t) (bottomY - offsetY - 4);

  labelAM = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(labelAM, "AM");
  lv_obj_set_pos(labelAM, labelAMpmX, labelAMy);
  lv_obj_set_style_local_text_color(labelAM, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
  lv_obj_set_size(labelAM, pointSize, tflHeight);

  labelPM = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(labelPM, "PM");
  lv_obj_set_pos(labelPM, labelAMpmX, labelAMy + tflHeight + tflSpacing);
  lv_obj_set_style_local_text_color(labelPM, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
  lv_obj_set_size(labelPM, pointSize, tflHeight);

  lastClockType = settingsController.GetClockType();
  is12HourModeSet = (settingsController.GetClockType() == Controllers::Settings::ClockType::H12);
  lv_obj_set_hidden(hourPoints[1], is12HourModeSet);
  lv_obj_set_hidden(labelAM, !is12HourModeSet);
  lv_obj_set_hidden(labelPM, !is12HourModeSet);

  heartbeatIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(heartbeatIcon, Symbols::heartBeat);
  lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
  lv_obj_align(heartbeatIcon, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

  heartbeatValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
  lv_label_set_text_static(heartbeatValue, "");
  lv_obj_align(heartbeatValue, heartbeatIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  stepValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00FFE7));
  lv_label_set_text_static(stepValue, "0");
  lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);

  stepIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00FFE7));
  lv_label_set_text_static(stepIcon, Symbols::shoe);
  lv_obj_align(stepIcon, stepValue, LV_ALIGN_OUT_LEFT_MID, -5, 0);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceBinaryBCD::~WatchFaceBinaryBCD() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void WatchFaceBinaryBCD::Refresh() {
  statusIcons.Update();

  notificationState = notificationManager.AreNewNotificationsAvailable();
  if (notificationState.IsUpdated()) {
    lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(notificationState.Get()));
  }

  currentDateTime = std::chrono::time_point_cast<std::chrono::seconds>(dateTimeController.CurrentDateTime());

  if (currentDateTime.IsUpdated()) {
    uint8_t hours = dateTimeController.Hours();
    bool shouldUpdateHours = (displayedHours != hours);

    uint8_t minutes = dateTimeController.Minutes();
    const bool shouldUpdateMinutes = (displayedMinutes != minutes);

#if BINARY_SECONDS_ENABLED
    uint8_t seconds = dateTimeController.Seconds();
    seconds = (uint8_t) (((seconds - seconds % 10) / 10) << 4 | seconds % 10);
#endif
    const bool clockTypeChanged = (settingsController.GetClockType() != lastClockType);

    if ((settingsController.GetClockType() == Controllers::Settings::ClockType::H12) && (shouldUpdateHours || clockTypeChanged)) {
      lastClockType = Controllers::Settings::ClockType::H12;

      if (!is12HourModeSet) {
        is12HourModeSet = true;
      }

      if ((12 <= hours) && (24 > hours)) {
        if (12 != hours) {
          hours -= 12;
        }
        lv_obj_set_style_local_text_color(labelAM, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
        lv_obj_set_style_local_text_color(labelPM, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, BINARY_ON_COLOR);
      } else {
        if (0 == hours) {
          hours = 12;
        }
        lv_obj_set_style_local_text_color(labelAM, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, BINARY_ON_COLOR);
        lv_obj_set_style_local_text_color(labelPM, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
      }
      shouldUpdateHours = (displayedHours != hours);
    } else if (is12HourModeSet) {
      is12HourModeSet = false;
    }

    if (shouldUpdateHours) {
      displayedHours = hours;
      hours = (uint8_t) (((hours - hours % 10) / 10) << 4 | hours % 10);
    }

    if (shouldUpdateMinutes) {
      displayedMinutes = minutes;
      minutes = (uint8_t) (((minutes - minutes % 10) / 10) << 4 | minutes % 10);
    }

    if (clockTypeChanged) {
      lv_obj_set_hidden(hourPoints[1], is12HourModeSet);
      lv_obj_set_hidden(labelAM, !is12HourModeSet);
      lv_obj_set_hidden(labelPM, !is12HourModeSet);
    }

    if (BINARY_SECONDS_ENABLED || shouldUpdateHours || shouldUpdateMinutes) {
      for (uint8_t i = 0; i < 4; i++) {
        // Hours
        if (shouldUpdateHours) {
          // High
          if (2 > i) {
            switch (hours >> (i + 3) & 0b1) {
              case 1:
                lv_obj_set_style_local_bg_color(hourPoints[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_ON_COLOR);
                break;
              default:
                lv_obj_set_style_local_bg_color(hourPoints[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
                break;
            }
          }
          // Low
          switch (hours >> i & 0b1) {
            case 1:
              lv_obj_set_style_local_bg_color(hourPoints[i + 2], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_ON_COLOR);
              break;
            default:
              lv_obj_set_style_local_bg_color(hourPoints[i + 2], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
              break;
          }
        }
        // Minutes
        if (shouldUpdateMinutes) {
          // High
          if (3 > i) {
            switch (minutes >> (i + 4) & 0b1) {
              case 1:
                lv_obj_set_style_local_bg_color(minutePoints[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_ON_COLOR);
                break;
              default:
                lv_obj_set_style_local_bg_color(minutePoints[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
                break;
            } 
          }
          // Low
          switch (minutes >> i & 0b1) {
            case 1:
              lv_obj_set_style_local_bg_color(minutePoints[i + 3], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_ON_COLOR);
              break;
            default:
              lv_obj_set_style_local_bg_color(minutePoints[i + 3], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
              break;
          }
        }
// Seconds
#if BINARY_SECONDS_ENABLED
        // High
        if (3 > i) {
          switch (seconds >> (i + 4) & 0b1) {
            case 1:
              lv_obj_set_style_local_bg_color(secondPoints[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_ON_COLOR);
              break;
            default:
              lv_obj_set_style_local_bg_color(secondPoints[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
              break;
          }
        }
        switch (seconds >> i & 0b1) {
          case 1:
            lv_obj_set_style_local_bg_color(secondPoints[i + 3], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_ON_COLOR);
            break;
          default:
            lv_obj_set_style_local_bg_color(secondPoints[i + 3], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, BINARY_OFF_COLOR);
            break;
        }
#endif
      }
    }

    currentDate = std::chrono::time_point_cast<days>(currentDateTime.Get());
    if (currentDate.IsUpdated()) {
      const uint16_t year = dateTimeController.Year();
      const uint8_t day = dateTimeController.Day();
      if (settingsController.GetClockType() == Controllers::Settings::ClockType::H24) {
        lv_label_set_text_fmt(lableDate,
                              "%s %d %s %d",
                              dateTimeController.DayOfWeekShortToString(),
                              day,
                              dateTimeController.MonthShortToString(),
                              year);
      } else {
        lv_label_set_text_fmt(lableDate,
                              "%s %s %d %d",
                              dateTimeController.DayOfWeekShortToString(),
                              dateTimeController.MonthShortToString(),
                              day,
                              year);
      }
      lv_obj_realign(lableDate);
    }
  }

  heartbeat = heartRateController.HeartRate();
  heartbeatRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
      lv_label_set_text_fmt(heartbeatValue, "%d", heartbeat.Get());
    } else {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x1B1B1B));
      lv_label_set_text_static(heartbeatValue, "");
    }

    lv_obj_realign(heartbeatIcon);
    lv_obj_realign(heartbeatValue);
  }

  stepCount = motionController.NbSteps();
  if (stepCount.IsUpdated()) {
    lv_label_set_text_fmt(stepValue, "%lu", stepCount.Get());
    lv_obj_realign(stepValue);
    lv_obj_realign(stepIcon);
  }
}
