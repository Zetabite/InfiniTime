/*  Copyright (C) 2020-2021 JF, Adam Pigg, Avamander

    This file is part of InfiniTime.

    InfiniTime is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    InfiniTime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "lvgl/lvgl.h"
#include <cstdint>
#include <string>
#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#include <host/ble_uuid.h>
#undef max
#undef min

static constexpr const uint16_t ALBUM_ART_WIDTH = 64;
// static constexpr const uint16_t ALBUM_ART_HEIGHT = 64;
static constexpr const uint16_t DISPLAY_WIDTH = 240;
// static constexpr const uint16_t DISPLAY_HEIGHT = 240;

namespace Pinetime {
  namespace Components {
    class LittleVgl;
  }

  namespace Controllers {
    class NimbleController;

    class MusicService {
    public:
      explicit MusicService(NimbleController& nimble);

      void Init();

      int OnCommand(struct ble_gatt_access_ctxt* ctxt);

      void event(char event);

      std::string getArtist() const;

      std::string getTrack() const;

      std::string getAlbum() const;

      uint32_t getProgress() const;

      uint32_t getTrackLength() const;

      float getPlaybackSpeed() const;

      bool isPlaying() const;

      bool didReceiveAlbumArtData() const;

      void musicAppClosed();

      void setLvglPtr(Pinetime::Components::LittleVgl& newLvgl);

      static constexpr const char EVENT_MUSIC_OPEN = 0xe0;
      static constexpr const char EVENT_MUSIC_PLAY = 0x00;
      static constexpr const char EVENT_MUSIC_PAUSE = 0x01;
      static constexpr const char EVENT_MUSIC_NEXT = 0x03;
      static constexpr const char EVENT_MUSIC_PREV = 0x04;
      static constexpr const char EVENT_MUSIC_VOLUP = 0x05;
      static constexpr const char EVENT_MUSIC_VOLDOWN = 0x06;

    private:
      typedef struct {
        uint32_t playing : 1;
        uint32_t repeat : 1;
        uint32_t shuffle : 1;
        uint32_t receivedAlbumArt : 1;
        uint32_t musicAppOpen : 1;
      } MusicStatus;

      struct ble_gatt_chr_def characteristicDefinition[15];
      struct ble_gatt_svc_def serviceDefinition[2];

      uint16_t eventHandle {};

      std::string artistName {"Waiting for"};
      std::string albumName {};
      std::string trackName {"track information.."};

      // uint8_t musicStringsUpdateStatus = MusicStringMask::Artist | MusicStringMask::Album | MusicStringMask::Track;

      MusicStatus musicStatus = {
        .playing = 0,
        .repeat = 0,
        .shuffle = 0,
        .receivedAlbumArt = 0,
        .musicAppOpen = 0
      };

      Pinetime::Components::LittleVgl* lvgl = nullptr;

      uint32_t trackProgress {0};
      uint32_t trackLength {0};
      uint32_t trackNumber {};
      uint32_t tracksTotal {};

      TickType_t trackProgressUpdateTime {0};

      float playbackSpeed {1.0f};

      NimbleController& nimble;
    };
  }
}
