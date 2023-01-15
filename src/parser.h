/**
 * Copyright (c) 2023 - Stanislav Ionascu <stanislav.ionascu@gmail.com>
 * This file is part of Diskcat.
 * SPDX-License-Identifier: MIT or GPL-2.0-or-later
 */

#pragma once

#include <filesystem>
#include <vector>

#include "boxed.h"

namespace diskcat {

class Parser {
public:
  virtual ~Parser() = default;

  virtual void open(const std::filesystem::path &path, int title = -1) = 0;
  virtual Boxed<std::vector<uint8_t>> next() = 0;
  virtual void close() = 0;
};
} // namespace diskcat
