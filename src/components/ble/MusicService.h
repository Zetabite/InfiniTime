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

#define NO_ALBUM_ART_CHECKSUM ((uint64_t) -1)
#define ALBUM_ART_WIDTH 32
#define ALBUM_ART_HEIGHT 32
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240
#define ALBUM_ART_NUM_COLORS 16

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

      uint64_t getAlbumArtChecksum() const;

      void musicAppClosed();

      uint32_t getProgress() const;

      uint32_t getTrackLength() const;

      float getPlaybackSpeed() const;

      bool isPlaying() const;

      void setLvglPtr(Pinetime::Components::LittleVgl& newLvgl);

      static const char EVENT_MUSIC_OPEN = 0xe0;
      static const char EVENT_MUSIC_PLAY = 0x00;
      static const char EVENT_MUSIC_PAUSE = 0x01;
      static const char EVENT_MUSIC_NEXT = 0x03;
      static const char EVENT_MUSIC_PREV = 0x04;
      static const char EVENT_MUSIC_VOLUP = 0x05;
      static const char EVENT_MUSIC_VOLDOWN = 0x06;

      enum MusicStatus { NotPlaying = 0x00, Playing = 0x01 };

    private:
      struct ble_gatt_chr_def characteristicDefinition[17];
      struct ble_gatt_svc_def serviceDefinition[2];

      uint16_t eventHandle {};

      std::string artistName {"Waiting for"};
      std::string albumName {};
      std::string trackName {"track information.."};

      uint64_t albumArtChecksum {0};
      lv_color_t indexedColors[ALBUM_ART_NUM_COLORS];
      bool acceptAlbumArtData = {false};
      bool musicAppOpen = {false};
      Pinetime::Components::LittleVgl* lvgl = nullptr;

      bool playing {false};

      uint32_t trackProgress {0};
      uint32_t trackLength {0};
      uint32_t trackNumber {};
      uint32_t tracksTotal {};
      TickType_t trackProgressUpdateTime {0};

      float playbackSpeed {1.0f};

      bool repeat {false};
      bool shuffle {false};

      NimbleController& nimble;
    };
  }
}
