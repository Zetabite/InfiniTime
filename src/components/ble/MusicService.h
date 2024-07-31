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

// 0 - 256, 0 meaning indexed colors aren't used
#define NUM_INDEXED_COLORS_ALBUM_COVER 0 // 240

namespace Pinetime {
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

      int32_t getProgress() const;

      int32_t getTrackLength() const;

      float getPlaybackSpeed() const;

      bool isPlaying() const;

      bool hasReceivedAlbumCover() const;

      static constexpr const char EVENT_MUSIC_OPEN = 0xe0;
      static constexpr const char EVENT_MUSIC_PLAY = 0x00;
      static constexpr const char EVENT_MUSIC_PAUSE = 0x01;
      static constexpr const char EVENT_MUSIC_NEXT = 0x03;
      static constexpr const char EVENT_MUSIC_PREV = 0x04;
      static constexpr const char EVENT_MUSIC_VOLUP = 0x05;
      static constexpr const char EVENT_MUSIC_VOLDOWN = 0x06;
      // static constexpr const char EVENT_MUSIC_REQUEST_ALBUM_ART = 0x07;

      // BLE 4.2 Payload Attribute Data = 244, otherwise 20
      static constexpr uint16_t MAX_BYTES_PER_CHUNK = 244;

      // Album Art Sizes
      static constexpr uint8_t ALB_COV_WIDTH = 64;
      static constexpr uint8_t ALB_COV_HEIGHT = 64;
      static constexpr uint16_t ALB_COV_AREA = ALB_COV_WIDTH * ALB_COV_HEIGHT;

      // Album Art Bytes
      #if NUM_INDEXED_COLORS_ALBUM_COVER
      static constexpr uint8_t BYTES_PER_COLOR = 4;
      static constexpr uint16_t COLOR_PALETTE_SIZE = NUM_INDEXED_COLORS_ALBUM_COVER * BYTES_PER_COLOR;
      static constexpr uint16_t TOTAL_ALB_COV_BYTES = ALB_COV_AREA + COLOR_PALETTE_SIZE;
      static constexpr uint16_t BYTES_PER_ROW = ALB_COV_WIDTH * BYTES_PER_COLOR;
      static constexpr uint8_t COLORS_PER_PACKET = 60;
      static constexpr uint8_t COLOR_PACKET_SIZE = COLORS_PER_PACKET * BYTES_PER_COLOR;
      #else
      static constexpr uint8_t BYTES_PER_COLOR = 2;
      static constexpr uint16_t TOTAL_ALB_COV_BYTES = ALB_COV_AREA * BYTES_PER_COLOR;
      static constexpr uint16_t BYTES_PER_ROW = ALB_COV_WIDTH * BYTES_PER_COLOR;
      #endif
      
      // BLE Relevant
      // Album Art Chunk Info 
      static constexpr uint8_t BYTES_FOR_CHUNK_DATA_SIZE = 1;
      static constexpr uint8_t BYTES_FOR_CHUNK_ID = 2;
      static constexpr uint8_t BYTES_FOR_CHUNK_TYPE = 1;
      static constexpr uint8_t BYTES_FOR_BITMAP_CHUNK_INFO = BYTES_FOR_CHUNK_DATA_SIZE + BYTES_FOR_CHUNK_ID + BYTES_FOR_CHUNK_TYPE;
      #if NUM_INDEXED_COLORS_ALBUM_COVER
      static constexpr uint8_t BYTES_FOR_COLOR_PALETTE_CHUNK_INFO = BYTES_FOR_BITMAP_CHUNK_INFO;
      #endif

      // Bytes transmitted
      static constexpr uint16_t MAX_BYTES_PER_BITMAP_DATA = MAX_BYTES_PER_CHUNK - BYTES_FOR_BITMAP_CHUNK_INFO;

      lv_img_dsc_t albumCoverImg = {
        {
          #if NUM_INDEXED_COLORS_ALBUM_COVER
          LV_IMG_CF_INDEXED_8BIT,
          #else
          LV_IMG_CF_TRUE_COLOR,
          #endif
          0,
          0,
          ALB_COV_WIDTH,
          ALB_COV_HEIGHT
        },
        TOTAL_ALB_COV_BYTES,
        albumCoverData
      };

    private:
      int32_t calcProgress() const;

      enum AlbumCoverType : uint8_t {
        UNKNOWN,
        NO_ALB_COV,
        STARTING,
        DONE
      };

      bool playing = false;
      bool repeat = false;
      bool shuffle = false;
      bool receivedAlbumCover = false;

      #if NUM_INDEXED_COLORS_ALBUM_COVER
      struct ble_gatt_chr_def characteristicDefinition[16];
      #else
      struct ble_gatt_chr_def characteristicDefinition[15];
      #endif
      struct ble_gatt_svc_def serviceDefinition[2];

      uint16_t eventHandle {};

      std::string artistName {"Waiting for"};
      std::string albumName {};
      std::string trackName {"track information.."};

      int32_t trackProgress {0};
      int32_t trackLength {0};
      int32_t trackNumber {};
      int32_t tracksTotal {};

      uint8_t albumCoverData[TOTAL_ALB_COV_BYTES];

      uint8_t albumCoverChunkInfo[BYTES_FOR_BITMAP_CHUNK_INFO];
      #if NUM_INDEXED_COLORS_ALBUM_COVER
      uint8_t colorPaletteChunkInfo[BYTES_FOR_COLOR_PALETTE_CHUNK_INFO];
      #endif

      TickType_t trackProgressUpdateTime {0};

      float playbackSpeed {1.0f};

      NimbleController& nimble;
    };
  }
}
