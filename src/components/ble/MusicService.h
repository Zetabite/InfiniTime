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

#define USE_ALBUM_ART_INDEXED_COLOR_PALETTE 0
#define USE_ALBUM_ART_CHECKSUM 0

#if USE_ALBUM_ART_CHECKSUM
static constexpr const uint64_t NO_ALBUM_ART_CHECKSUM = ((uint64_t) -1);
#endif

#if USE_ALBUM_ART_INDEXED_COLOR_PALETTE
static constexpr const uint8_t ALBUM_ART_NUM_COLORS = 16;
#endif

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

      #if USE_ALBUM_ART_CHECKSUM
      uint32_t getAlbumArtChecksum() const;
      #endif

      bool didReceiveAlbumArtData() const;

      void musicAppClosed();

      void setLvglPtr(Pinetime::Components::LittleVgl& newLvgl);

      bool isArtistUpdated();

      bool isAlbumUpdated();

      bool isTrackUpdated();

      static constexpr const char EVENT_MUSIC_OPEN = 0xe0;
      static constexpr const char EVENT_MUSIC_PLAY = 0x00;
      static constexpr const char EVENT_MUSIC_PAUSE = 0x01;
      static constexpr const char EVENT_MUSIC_NEXT = 0x03;
      static constexpr const char EVENT_MUSIC_PREV = 0x04;
      static constexpr const char EVENT_MUSIC_VOLUP = 0x05;
      static constexpr const char EVENT_MUSIC_VOLDOWN = 0x06;

      enum MusicStatusMask : uint8_t {
        Playing = 1 << 0,
        Repeat = 1 << 1,
        Shuffle = 1 << 2,
        AcceptAlbumArt = 1 << 3,
        ReceivedAlbumArt = 1 << 4,
        MusicAppOpen = 1 << 5
      };

      /*
      enum MusicStringMask : uint8_t {
        Artist = 1 << 0,
        Album = 1 << 1,
        Track = 1 << 2
      };
      */

    private:
      #if USE_ALBUM_ART_INDEXED_COLOR_PALETTE
      struct ble_gatt_chr_def characteristicDefinition[17];
      #else
      struct ble_gatt_chr_def characteristicDefinition[15];
      #endif
      struct ble_gatt_svc_def serviceDefinition[2];

      uint16_t eventHandle {};

      std::string artistName {"Waiting for"};
      std::string albumName {};
      std::string trackName {"track information.."};

      // uint8_t musicStringsUpdateStatus = MusicStringMask::Artist | MusicStringMask::Album | MusicStringMask::Track;

      #if USE_ALBUM_ART_CHECKSUM
      uint32_t albumArtChecksum {0};
      #endif
      #if USE_ALBUM_ART_INDEXED_COLOR_PALETTE
      lv_color_t indexedColors[ALBUM_ART_NUM_COLORS];
      #endif
      #if USE_ALBUM_ART_CHECKSUM
      uint8_t musicStatus {
        (0 << MusicStatusMask::Playing) | (0 << MusicStatusMask::Repeat) | (0 << MusicStatusMask::Shuffle) | (0 << MusicStatusMask::AcceptAlbumArt) | (0 << MusicStatusMask::ReceivedAlbumArt)
      };
      #else
      uint8_t musicStatus {
        (0 << MusicStatusMask::Playing) | (0 << MusicStatusMask::Repeat) | (0 << MusicStatusMask::Shuffle) | (1 << MusicStatusMask::AcceptAlbumArt) | (0 << MusicStatusMask::ReceivedAlbumArt)
      };
      #endif
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
