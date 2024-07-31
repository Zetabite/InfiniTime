#pragma once
#include <cstdint>
#include <drivers/include/nrfx_saadc.h>
#include <systemtask/SystemTask.h>

namespace Pinetime {
  namespace Controllers {
    class Battery {
    public:
      Battery();

      void ReadPowerState();
      void MeasureVoltage();
      void Register(System::SystemTask* systemTask);

      uint8_t PercentRemaining() const;

      uint16_t Voltage() const;

      bool IsCharging() const;
      bool IsPowerPresent() const;

    private:
      void SaadcInit();
      void SaadcEventHandler(nrfx_saadc_evt_t const* p_event);

      static void AdcCallbackStatic(nrfx_saadc_evt_t const* event);

      static Battery* instance;
      nrf_saadc_value_t saadc_value;

      static constexpr nrf_saadc_input_t batteryVoltageAdcInput = NRF_SAADC_INPUT_AIN7;

      typedef struct {
        uint32_t firstMeasurement : 1;
        uint32_t powerPresent : 1;
        uint32_t reading : 1;
        uint32_t charging : 1;
        uint32_t full : 1;
        uint32_t : 3;
        uint32_t percentRemaining : 8;
        uint32_t voltage : 16;
      } BatteryStatus;

      BatteryStatus batteryStatus = {
        .firstMeasurement = 1,
        .powerPresent = 0,
        .reading = 0,
        .charging = 0,
        .full = 0,
        .percentRemaining = 0,
        .voltage = 0
      };

      Pinetime::System::SystemTask* systemTask = nullptr;
    };
  }
}
