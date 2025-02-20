#pragma once

#include "displayapp/screens/Screen.h"
#include <lvgl/lvgl.h>

#include <FreeRTOS.h>
#include "portmacro_cmsis.h"

#include "systemtask/SystemTask.h"
#include "displayapp/apps/Apps.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      enum class States : uint8_t { Init, Running, Halted };

      struct TimeSeparated_t {
        uint16_t hours;
        uint16_t mins;
        uint16_t secs;
        uint16_t hundredths;
      };

      class StopWatch : public Screen {
      public:
        explicit StopWatch(System::SystemTask& systemTask);
        ~StopWatch() override;
        void Refresh() override;

        void playPauseBtnEventHandler();
        void stopLapBtnEventHandler();
        bool OnButtonPushed() override;

      private:
        void SetInterfacePaused();
        void SetInterfaceRunning();
        void SetInterfaceStopped();

        void Reset();
        void Start();
        void Pause();

        Pinetime::System::SystemTask& systemTask;
        States currentState = States::Init;
        TickType_t startTime;
        TickType_t oldTimeElapsed = 0;
        TickType_t blinkTime = 0;
        static constexpr int maxLapCount = 20;
        TickType_t laps[maxLapCount + 1];
        static constexpr uint8_t displayedLaps = 2;
        bool isHoursLabelUpdated = false;
        int lapsDone = 0;
        lv_obj_t *time, *msecTime, *btnPlayPause, *btnStopLap, *txtPlayPause, *txtStopLap;
        lv_obj_t* lapText;

        lv_task_t* taskRefresh;
      };
    }

    template <>
    struct AppTraits<Apps::StopWatch> {
      static constexpr Apps app = Apps::StopWatch;
      static constexpr const char* icon = Screens::Symbols::stopWatch;

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::StopWatch(*controllers.systemTask);
      };
    };
  }
}
