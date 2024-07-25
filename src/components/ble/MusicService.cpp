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
#include "components/ble/MusicService.h"
#include "components/ble/NimbleController.h"
#include <cstring>
#include "MusicService.h"

namespace {
  // 0000yyxx-78fc-48fe-8e23-433b3a1942d0
  constexpr ble_uuid128_t CharUuid(uint8_t x, uint8_t y) {
    return ble_uuid128_t {.u = {.type = BLE_UUID_TYPE_128},
                          .value = {0xd0, 0x42, 0x19, 0x3a, 0x3b, 0x43, 0x23, 0x8e, 0xfe, 0x48, 0xfc, 0x78, x, y, 0x00, 0x00}};
  }

  // 00000000-78fc-48fe-8e23-433b3a1942d0
  constexpr ble_uuid128_t BaseUuid() {
    return CharUuid(0x00, 0x00);
  }

  constexpr ble_uuid128_t msUuid {BaseUuid()};

  constexpr ble_uuid128_t msEventCharUuid {CharUuid(0x01, 0x00)};
  constexpr ble_uuid128_t msStatusCharUuid {CharUuid(0x02, 0x00)};
  constexpr ble_uuid128_t msArtistCharUuid {CharUuid(0x03, 0x00)};
  constexpr ble_uuid128_t msTrackCharUuid {CharUuid(0x04, 0x00)};
  constexpr ble_uuid128_t msAlbumCharUuid {CharUuid(0x05, 0x00)};
  constexpr ble_uuid128_t msPositionCharUuid {CharUuid(0x06, 0x00)};
  constexpr ble_uuid128_t msTotalLengthCharUuid {CharUuid(0x07, 0x00)};
  constexpr ble_uuid128_t msTrackNumberCharUuid {CharUuid(0x08, 0x00)};
  constexpr ble_uuid128_t msTrackTotalCharUuid {CharUuid(0x09, 0x00)};
  constexpr ble_uuid128_t msPlaybackSpeedCharUuid {CharUuid(0x0a, 0x00)};
  constexpr ble_uuid128_t msRepeatCharUuid {CharUuid(0x0b, 0x00)};
  constexpr ble_uuid128_t msShuffleCharUuid {CharUuid(0x0c, 0x00)};
  constexpr ble_uuid128_t msAlbumCoverCharUuid {CharUuid(0x0d, 0x00)};
  #if NUM_INDEXED_COLORS_ALBUM_COVER
  constexpr ble_uuid128_t msAlbumCoverPaletteCharUuid {CharUuid(0x0d, 0x00)};
  #endif

  constexpr uint8_t MaxStringSize {40};

  int MusicCallback(uint16_t /*conn_handle*/, uint16_t /*attr_handle*/, struct ble_gatt_access_ctxt* ctxt, void* arg) {
    return static_cast<Pinetime::Controllers::MusicService*>(arg)->OnCommand(ctxt);
  }
}

Pinetime::Controllers::MusicService::MusicService(Pinetime::Controllers::NimbleController& nimble) : nimble(nimble) {
  characteristicDefinition[0] = {.uuid = &msEventCharUuid.u,
                                 .access_cb = MusicCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_NOTIFY,
                                 .val_handle = &eventHandle};
  characteristicDefinition[1] = {.uuid = &msStatusCharUuid.u,
                                 .access_cb = MusicCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[2] = {.uuid = &msTrackCharUuid.u,
                                 .access_cb = MusicCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[3] = {.uuid = &msArtistCharUuid.u,
                                 .access_cb = MusicCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[4] = {.uuid = &msAlbumCharUuid.u,
                                 .access_cb = MusicCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[5] = {.uuid = &msPositionCharUuid.u,
                                 .access_cb = MusicCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[6] = {.uuid = &msTotalLengthCharUuid.u,
                                 .access_cb = MusicCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[7] = {.uuid = &msTotalLengthCharUuid.u,
                                 .access_cb = MusicCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[8] = {.uuid = &msTrackNumberCharUuid.u,
                                 .access_cb = MusicCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[9] = {.uuid = &msTrackTotalCharUuid.u,
                                 .access_cb = MusicCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[10] = {.uuid = &msPlaybackSpeedCharUuid.u,
                                  .access_cb = MusicCallback,
                                  .arg = this,
                                  .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[11] = {.uuid = &msRepeatCharUuid.u,
                                  .access_cb = MusicCallback,
                                  .arg = this,
                                  .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[12] = {.uuid = &msShuffleCharUuid.u,
                                  .access_cb = MusicCallback,
                                  .arg = this,
                                  .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[13] = {.uuid = &msAlbumCoverCharUuid.u,
                                  .access_cb = MusicCallback,
                                  .arg = this,
                                  .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  #if NUM_INDEXED_COLORS_ALBUM_COVER
  characteristicDefinition[14] = {.uuid = &msAlbumCoverPaletteCharUuid.u,
                                  .access_cb = MusicCallback,
                                  .arg = this,
                                  .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[15] = {0};
  #else
  characteristicDefinition[14] = {0};
  #endif

  serviceDefinition[0] = {.type = BLE_GATT_SVC_TYPE_PRIMARY, .uuid = &msUuid.u, .characteristics = characteristicDefinition};
  serviceDefinition[1] = {0};

  /*
  // Test if map is properly created
  // uint16_t pos = 0;
  for (uint8_t y = 0; y < Controllers::MusicService::ALBUM_ART_HEIGHT; y++) {
    for (uint8_t x = 0; x < Controllers::MusicService::ALBUM_ART_WIDTH; x++) {
      lv_color_t color = LV_COLOR_WHITE;
      uint16_t pos = x * 2 + y * (Controllers::MusicService::ALBUM_ART_WIDTH * 2);
      album_art_map[pos] = color.ch.green_l | color.ch.blue << 3;
      album_art_map[pos + 1] = color.ch.green_h << 3 | color.ch.red;
      //pos++2;
    }
  }
  */
  #if NUM_INDEXED_COLORS_ALBUM_COVER
  for (uint16_t i = 0; i < COLOR_PALETTE_SIZE; i++) {
    albumCoverData[i] = 0;
  }
  #endif
}

void Pinetime::Controllers::MusicService::Init() {
  uint8_t res = 0;
  res = ble_gatts_count_cfg(serviceDefinition);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(serviceDefinition);
  ASSERT(res == 0);
}

int Pinetime::Controllers::MusicService::OnCommand(struct ble_gatt_access_ctxt* ctxt) {
  if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
    size_t notifSize = OS_MBUF_PKTLEN(ctxt->om);
    size_t bufferSize = notifSize;

    if (ble_uuid_cmp(ctxt->chr->uuid, &msAlbumCoverCharUuid.u) == 0) {
      if (notifSize > MAX_BYTES_PER_CHUNK) {
        bufferSize = MAX_BYTES_PER_CHUNK;
      }

      if (bufferSize != MAX_BYTES_PER_CHUNK) {
        return 0;
      }

      if (0 != os_mbuf_copydata(ctxt->om, 0, BYTES_FOR_BITMAP_CHUNK_INFO, albumCoverChunkInfo)) {
        return 0;
      }

      const uint8_t albumCoverType = albumCoverChunkInfo[0];

      if (2 > albumCoverType || albumCoverType > 4) {
        return 0;
      }

      const uint8_t col = albumCoverChunkInfo[1];
      const uint8_t row = albumCoverChunkInfo[2];
      uint8_t dataSize = albumCoverChunkInfo[3];

      if (dataSize > MAX_BYTES_PER_BITMAP_DATA) {
        dataSize = MAX_BYTES_PER_BITMAP_DATA;
      }

      os_mbuf_copydata(ctxt->om, BYTES_FOR_BITMAP_CHUNK_INFO, dataSize, albumCoverData + row * BYTES_PER_ROW + col * BYTES_PER_COLOR);

      receivedAlbumCover = (albumCoverType == AlbumCoverType::DONE || MAX_BYTES_PER_BITMAP_DATA > dataSize);
      return 0;
    } 
    #if NUM_INDEXED_COLORS_ALBUM_COVER
    else if (ble_uuid_cmp(ctxt->chr->uuid, &msAlbumCoverPaletteCharUuid.u) == 0) {
      if (notifSize > MAX_BYTES_PER_CHUNK) {
        bufferSize = MAX_BYTES_PER_CHUNK;
      }

      if (bufferSize != MAX_BYTES_PER_CHUNK) {
        return 0;
      }

      if (0 != os_mbuf_copydata(ctxt->om, 0, BYTES_FOR_COLOR_PALETTE_CHUNK_INFO, colorPaletteChunkInfo)) {
        return 0;
      }

      const uint8_t chunkType = colorPaletteChunkInfo[0];

      if (2 > chunkType || chunkType > 4) {
        return 0;
      }

      const uint8_t chunkOffset = colorPaletteChunkInfo[2];

      os_mbuf_copydata(ctxt->om, BYTES_FOR_COLOR_PALETTE_CHUNK_INFO, COLOR_PACKET_SIZE, albumCoverData + chunkOffset * COLOR_PACKET_SIZE);
      return 0;
    }
    #endif

    if (notifSize > MaxStringSize) {
      bufferSize = MaxStringSize;
    }

    char data[bufferSize + 1];
    os_mbuf_copydata(ctxt->om, 0, bufferSize, data);

    if (notifSize > bufferSize) {
      data[bufferSize - 1] = '.';
      data[bufferSize - 2] = '.';
      data[bufferSize - 3] = '.';
    }
    data[bufferSize] = '\0';

    char* s = &data[0];
    if (ble_uuid_cmp(ctxt->chr->uuid, &msArtistCharUuid.u) == 0) {
      artistName = s;
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &msTrackCharUuid.u) == 0) {
      trackName = s;
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &msAlbumCharUuid.u) == 0) {
      albumName = s;
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &msStatusCharUuid.u) == 0) {
      playing = s[0];
      // These variables need to be updated, because the progress may not be updated immediately,
      // leading to getProgress() returning an incorrect position.
      if (playing) {
        trackProgressUpdateTime = xTaskGetTickCount();
      } else {
        trackProgress +=
          static_cast<uint32_t>((static_cast<float>(xTaskGetTickCount() - trackProgressUpdateTime) / 1024.0f) * getPlaybackSpeed());
      }
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &msRepeatCharUuid.u) == 0) {
      repeat = s[0];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &msShuffleCharUuid.u) == 0) {
      shuffle = s[0];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &msPositionCharUuid.u) == 0) {
      trackProgress = (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3];
      trackProgressUpdateTime = xTaskGetTickCount();
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &msTotalLengthCharUuid.u) == 0) {
      trackLength = (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &msTrackNumberCharUuid.u) == 0) {
      trackNumber = (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &msTrackTotalCharUuid.u) == 0) {
      tracksTotal = (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &msPlaybackSpeedCharUuid.u) == 0) {
      playbackSpeed = static_cast<float>(((s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3])) / 100.0f;
    }
  }
  return 0;
}

std::string Pinetime::Controllers::MusicService::getAlbum() const {
  return albumName;
}

std::string Pinetime::Controllers::MusicService::getArtist() const {
  return artistName;
}

std::string Pinetime::Controllers::MusicService::getTrack() const {
  return trackName;
}

bool Pinetime::Controllers::MusicService::isPlaying() const {
  return playing;
}

float Pinetime::Controllers::MusicService::getPlaybackSpeed() const {
  return playbackSpeed;
}

int32_t Pinetime::Controllers::MusicService::getProgress() const {
  if (playing) {
    return trackProgress +
           static_cast<int32_t>((static_cast<float>(xTaskGetTickCount() - trackProgressUpdateTime) / 1024.0f) * getPlaybackSpeed());
  }
  return trackProgress;
}

int32_t Pinetime::Controllers::MusicService::getTrackLength() const {
  return trackLength;
}

bool Pinetime::Controllers::MusicService::hasReceivedAlbumCover() const {
  return receivedAlbumCover;
}

void Pinetime::Controllers::MusicService::event(char event) {
  if (EVENT_MUSIC_PREV == event || EVENT_MUSIC_NEXT == event) {
    receivedAlbumCover = false;
    trackProgress = 0;
    trackProgressUpdateTime = xTaskGetTickCount();
  }

  auto* om = ble_hs_mbuf_from_flat(&event, 1);

  uint16_t connectionHandle = nimble.connHandle();

  if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE) {
    return;
  }

  ble_gattc_notify_custom(connectionHandle, eventHandle, om);
}
