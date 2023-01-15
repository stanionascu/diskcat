/**
 * Copyright (c) 2023 - Stanislav Ionascu <stanislav.ionascu@gmail.com>
 * This file is part of Diskcat.
 * SPDX-License-Identifier: MIT or GPL-2.0-or-later
 */

#include "dvdparser.h"

#include <absl/log/log.h>
#include <absl/strings/str_format.h>
#include <dvdnav/dvdnav.h>

namespace {

void dvdnav_log_cb(void *, dvdnav_logger_level_t dvd_level, const char *msg, va_list args) {
  auto fmt_str = absl::StrFormat(msg, args);
  if (dvd_level == DVDNAV_LOGGER_LEVEL_DEBUG) {
    LOG(INFO) << fmt_str;
  } else if (dvd_level == DVDNAV_LOGGER_LEVEL_INFO) {
    LOG(INFO) << fmt_str;
  } else if (dvd_level == DVDNAV_LOGGER_LEVEL_WARN) {
    LOG(WARNING) << fmt_str;
  } else if (dvd_level == DVDNAV_LOGGER_LEVEL_ERROR) {
    LOG(ERROR) << fmt_str;
  }
}

} // namespace

namespace diskcat {

struct DvdParser::Context {
  dvdnav_t *dvd{nullptr};
  dvdnav_logger_cb log_cb;
};

DvdParser::DvdParser() : ctx_(std::make_unique<Context>()) {}

DvdParser::~DvdParser() { close(); }

void DvdParser::open(const std::filesystem::path &path, int selected_title) {
  if (ctx_->dvd != nullptr) {
    close();
  }
  ctx_->log_cb.pf_log = &dvdnav_log_cb;

  int longest_title = 1;
  uint64_t longest_title_duration = 0;
  if (dvdnav_open2(&ctx_->dvd, nullptr, &ctx_->log_cb, path.c_str()) != DVDNAV_STATUS_OK) {
    LOG(FATAL) << "Failed to dvdnav_open(" << path << ")";
  }

  int32_t num_titles;
  if (dvdnav_get_number_of_titles(ctx_->dvd, &num_titles) != DVDNAV_STATUS_OK) {
    LOG(FATAL) << "Failed to dvdnav_get_number_of_titles()";
  }

  for (int title = 0; title < num_titles; ++title) {
    uint64_t title_duration = 0;
    uint64_t *chapters;
    if (auto num_parts = dvdnav_describe_title_chapters(ctx_->dvd, title, &chapters, &title_duration)) {
      if (title_duration >= longest_title_duration) {
        longest_title_duration = title_duration;
        longest_title = title;
      }
      LOG(INFO) << "Title: " << title << " with duration: " << title_duration << " and " << num_parts << " chapter(s)";
    }
  }
  LOG(INFO) << "There are " << num_titles << " titles";

  if (selected_title < 0) {
    selected_title = longest_title;
  }

  LOG(INFO) << "Selected title: " << selected_title;
  if (dvdnav_title_play(ctx_->dvd, selected_title) != DVDNAV_STATUS_OK) {
    LOG(INFO) << "Failed to dvdnav_title_play(" << selected_title << ")";
  }

  dvdnav_set_readahead_flag(ctx_->dvd, 1);
  dvdnav_set_PGC_positioning_flag(ctx_->dvd, 1);

  selected_title_ = selected_title;
}

Boxed<std::vector<uint8_t>> DvdParser::next() {
  std::vector<uint8_t> buffer;
  buffer.resize(DVD_VIDEO_LB_LEN);
  int result, event, len = DVD_VIDEO_LB_LEN;

  result = dvdnav_get_next_block(ctx_->dvd, buffer.data(), &event, &len);

  if (result == DVDNAV_STATUS_ERR) {
    LOG(FATAL) << "Error while trying to retrieve next block: " << result;
  }

  switch (event) {
  case DVDNAV_BLOCK_OK: {
    return buffer;
  } break;
  case DVDNAV_NOP:
    break;
  case DVDNAV_STILL_FRAME:
    dvdnav_still_skip(ctx_->dvd);
    break;
  case DVDNAV_WAIT:
    dvdnav_wait_skip(ctx_->dvd);
    break;
  case DVDNAV_CELL_CHANGE: {
    int32_t tt = 0, ptt = 0;
    uint32_t pos, len;

    dvdnav_current_title_info(ctx_->dvd, &tt, &ptt);
    dvdnav_get_position(ctx_->dvd, &pos, &len);

    LOG(INFO) << "Cell change, title: " << tt << ", chapter: " << ptt;
    LOG(INFO) << "Position: " << (100. * (double(pos) / len)) << "%";
    // if title number changes, it's time to quit
    if (tt != selected_title_) {
      return Error{"Title playback finished."};
    }
  } break;
  case DVDNAV_SPU_CLUT_CHANGE:
  case DVDNAV_SPU_STREAM_CHANGE:
  case DVDNAV_AUDIO_STREAM_CHANGE:
  case DVDNAV_HIGHLIGHT:
  case DVDNAV_VTS_CHANGE:
  case DVDNAV_HOP_CHANNEL:
    break;
  case DVDNAV_NAV_PACKET: {
    // just read and skip
    pci_t *pci = dvdnav_get_current_nav_pci(ctx_->dvd);
    dsi_t *dsi = dvdnav_get_current_nav_dsi(ctx_->dvd);
  } break;
  case DVDNAV_STOP:
  default:
    return Error{"Stop requested."};
  }

  return buffer;
}

void DvdParser::close() {
  dvdnav_close(ctx_->dvd);
  ctx_->dvd = nullptr;
}

} // namespace diskcat