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
#include "displayapp/LittleVgl.h"

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
  constexpr ble_uuid128_t msAlbumArtFrameEntryCharUuid {CharUuid(0x0d, 0x00)};

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
  characteristicDefinition[13] = {.uuid = &msAlbumArtFrameEntryCharUuid.u,
                                  .access_cb = MusicCallback,
                                  .arg = this,
                                  .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[14] = {0};

  serviceDefinition[0] = {.type = BLE_GATT_SVC_TYPE_PRIMARY, .uuid = &msUuid.u, .characteristics = characteristicDefinition};
  serviceDefinition[1] = {0};
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
    if (notifSize > MaxStringSize) {
      bufferSize = MaxStringSize;
    }

    char data[bufferSize + 1];
    os_mbuf_copydata(ctxt->om, 0, bufferSize, data);

    if (ble_uuid_cmp(ctxt->chr->uuid, &msAlbumArtFrameEntryCharUuid.u) == 0) {
      if (musicStatus.musicAppOpen && lvgl != nullptr) {
        musicStatus.receivedAlbumArt = 1;
        for (uint8_t i = 0; i < MaxStringSize; i += 5) {
          lv_area_t area;
          area.x1 = data[i + 0] + ((DISPLAY_WIDTH - ALBUM_ART_WIDTH - 15));
          area.x2 = area.x1;
          area.y1 = data[i + 1] + 15;
          area.y2 = area.y1;

          lv_color16_t color;
          color.ch.red = (data[i + 2] & 0b11111000) >> 3;
          color.ch.green_l = (data[i + 3] & 0b11100000) >> 5;
          color.ch.green_h = (data[i + 2] & 0b00000111) << 3;
          color.ch.blue = data[i + 3] & 0b00011111;

          lvgl->SetFullRefresh(Components::LittleVgl::FullRefreshDirections::None);
          lvgl->FlushDisplay(&area, &color);
        }
      }
      return 0;
    }

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
      musicStatus.playing = s[0];
      // These variables need to be updated, because the progress may not be updated immediately,
      // leading to getProgress() returning an incorrect position.
      if (musicStatus.playing) {
        trackProgressUpdateTime = xTaskGetTickCount();
      } else {
        trackProgress +=
          static_cast<uint32_t>((static_cast<float>(xTaskGetTickCount() - trackProgressUpdateTime) / 1024.0f) * getPlaybackSpeed());
      }
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &msRepeatCharUuid.u) == 0) {
      musicStatus.repeat = s[0];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &msShuffleCharUuid.u) == 0) {
      musicStatus.shuffle = s[0];
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
  return musicStatus.playing;
}

void Pinetime::Controllers::MusicService::setLvglPtr(Pinetime::Components::LittleVgl& newLvgl) {
  lvgl = &newLvgl;
}

float Pinetime::Controllers::MusicService::getPlaybackSpeed() const {
  return playbackSpeed;
}

uint32_t Pinetime::Controllers::MusicService::getProgress() const {
  if (musicStatus.playing) {
    return trackProgress +
           static_cast<uint32_t>((static_cast<float>(xTaskGetTickCount() - trackProgressUpdateTime) / 1024.0f) * getPlaybackSpeed());
  }
  return trackProgress;
}

uint32_t Pinetime::Controllers::MusicService::getTrackLength() const {
  return trackLength;
}

bool Pinetime::Controllers::MusicService::didReceiveAlbumArtData() const {
  return musicStatus.receivedAlbumArt;
}

void Pinetime::Controllers::MusicService::musicAppClosed() {
  musicStatus.receivedAlbumArt = 0;
  musicStatus.musicAppOpen = 0;
  lvgl = nullptr;
}

void Pinetime::Controllers::MusicService::event(char event) {
  switch (event) {
    case EVENT_MUSIC_OPEN: {
      musicStatus.musicAppOpen = 1;
    } break;
    case EVENT_MUSIC_PREV:
    case EVENT_MUSIC_NEXT: {
      musicStatus.receivedAlbumArt = 0;
    } break;
  }

  auto* om = ble_hs_mbuf_from_flat(&event, 1);

  uint16_t connectionHandle = nimble.connHandle();

  if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE) {
    return;
  }

  ble_gattc_notify_custom(connectionHandle, eventHandle, om);
}
