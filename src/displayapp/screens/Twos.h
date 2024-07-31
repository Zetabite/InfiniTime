#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"

namespace Pinetime {
  namespace Applications {
    struct TwosTile {
      bool merged = false;
      unsigned int value = 0;
    };

    namespace Screens {
      class Twos : public Screen {
      public:
        Twos();
        ~Twos() override;

        bool OnTouchEvent(TouchEvents event) override;

      private:
        static constexpr uint8_t nColors = 5;
        lv_style_t cellStyles[nColors];

        lv_obj_t* scoreText;
        lv_obj_t* gridDisplay;
        static constexpr uint8_t nCols = 4;
        static constexpr uint8_t nRows = 4;
        static constexpr uint8_t nCells = nCols * nRows;
        TwosTile grid[nRows][nCols];
        uint32_t score = 0;
        void updateGridDisplay();
        bool tryMerge(int32_t newRow, int32_t newCol, int32_t oldRow, int32_t oldCol);
        bool tryMove(int32_t newRow, int32_t newCol, int32_t oldRow, int32_t oldCol);
        bool placeNewTile();
      };
    }

    template <>
    struct AppTraits<Apps::Twos> {
      static constexpr Apps app = Apps::Twos;
      static constexpr const char* icon = "2";

      static Screens::Screen* Create(AppControllers& /*controllers*/) {
        return new Screens::Twos();
      };
    };
  }
}
