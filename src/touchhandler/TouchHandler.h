#pragma once
#include "drivers/Cst816s.h"
#include "displayapp/TouchEvents.h"

namespace Pinetime {
  namespace Controllers {
    class TouchHandler {
    public:
      struct TouchPoint {
        uint16_t x;
        uint16_t y;
        bool touching;
      };

      bool ProcessTouchInfo(Drivers::Cst816S::TouchInfos info);

      bool IsTouching() const {
        return currentTouchPoint.touching;
      }

      uint16_t GetX() const {
        return currentTouchPoint.x;
      }

      uint16_t GetY() const {
        return currentTouchPoint.y;
      }

      Pinetime::Applications::TouchEvents GestureGet();

    private:
      Pinetime::Applications::TouchEvents gesture;
      TouchPoint currentTouchPoint = {};
      bool gestureReleased = true;
    };
  }
}
