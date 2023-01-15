/**
 * Copyright (c) 2023 - Stanislav Ionascu <stanislav.ionascu@gmail.com>
 * This file is part of Diskcat.
 * SPDX-License-Identifier: MIT or GPL-2.0-or-later
 */

#include "probe.h"

#include <absl/log/log.h>
#include <udfread/udfread.h>

#include "blurayparser.h"
#include "dvdparser.h"

namespace fs = std::filesystem;

namespace diskcat {

std::unique_ptr<Parser> probe(const std::filesystem::path &path) {
  // check for VIDEO_TS folder
  LOG(INFO) << "Probing " << path;
  if (fs::is_directory(path)) {
    LOG(INFO) << path << " is a directory.";
    if (fs::exists(path / "VIDEO_TS")) {
      return std::make_unique<DvdParser>();
    } else if (fs::exists(path / "BDMV")) {
      return std::make_unique<BlurayParser>();
    }
  } else {
    if (udfread *udf = udfread_init()) {
      if (udfread_open(udf, path.c_str()) == 0) {
        if (UDFDIR *dir = udfread_opendir(udf, "VIDEO_TS")) {
          udfread_closedir(dir);
          return std::make_unique<DvdParser>();
        } else if (UDFDIR *dir = udfread_opendir(udf, "BDMV")) {
          udfread_closedir(dir);
          return std::make_unique<BlurayParser>();
        }
        udfread_close(udf);
      }
    }
  }
  return {};
}

} // namespace diskcat