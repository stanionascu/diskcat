/**
 * Copyright (c) 2023 - Stanislav Ionascu <stanislav.ionascu@gmail.com>
 * This file is part of Diskcat.
 * SPDX-License-Identifier: MIT or GPL-2.0-or-later
 */

#include "blurayparser.h"

#include <absl/flags/flag.h>
#include <absl/log/log.h>
#include <absl/strings/str_format.h>
#include <libbluray/bluray.h>

#include "diskcat.h"

namespace fs = std::filesystem;

namespace diskcat {

struct BlurayParser::Context {
  BLURAY *bd{nullptr};
};

BlurayParser::BlurayParser() : ctx_{std::make_unique<Context>()} {}
BlurayParser::~BlurayParser() { close(); }

void BlurayParser::open(const std::filesystem::path &path) {
  auto selected_title = absl::GetFlag(FLAGS_title);
  int longest_title = 1;
  int longest_title_duration;

  if (!(ctx_->bd = bd_open(path.c_str(), nullptr))) {
    LOG(FATAL) << "Failed to " << absl::StrFormat("bd_open_disc(%s)", path.c_str());
  }

  int num_titles = bd_get_titles(ctx_->bd, TITLES_RELEVANT, absl::GetFlag(FLAGS_min_title_length));

  for (int title = 0; title < num_titles; ++title) {
    auto *info = bd_get_title_info(ctx_->bd, title, absl::GetFlag(FLAGS_angle));
    LOG(INFO) << absl::StrFormat("Playlist %05d.mpls, (%02d:%02d:%02d) with %d chapter(s)", info->playlist,
                                 info->duration / 90 / 3600, info->duration / 90 % 3600 / 60, info->duration / 90 % 60,
                                 info->chapter_count);
    if (info->duration >= longest_title_duration) {
      longest_title_duration = info->duration;
      longest_title = info->playlist;
    }
    bd_free_title_info(info);
  }

  if (selected_title < 0) {
    selected_title = bd_get_main_title(ctx_->bd);
    if (selected_title < 0) {
      selected_title = longest_title;
    }
  }

  bd_select_playlist(ctx_->bd, selected_title);
}

Boxed<std::vector<uint8_t>> BlurayParser::next() {
  std::vector<uint8_t> buffer;
  buffer.resize(1024 * 32);

  int read = bd_read(ctx_->bd, buffer.data(), buffer.size());
  if (read == 0) {
    return Error{"Finished reading Bluray."};
  } else if (read == -1) {
    return Error{"Failed to read Bluray."};
  }

  return buffer;
}

void BlurayParser::close() {
  if (ctx_->bd) {
    bd_close(ctx_->bd);
    ctx_->bd = nullptr;
  }
}

} // namespace diskcat