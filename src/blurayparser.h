/**
 * Copyright (c) 2023 - Stanislav Ionascu <stanislav.ionascu@gmail.com>
 * This file is part of Diskcat.
 * SPDX-License-Identifier: MIT or GPL-2.0-or-later
 */

#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include "boxed.h"
#include "parser.h"

namespace diskcat {

class BlurayParser : public Parser {
  class Context;

public:
  BlurayParser();
  ~BlurayParser() override;

  void open(const std::filesystem::path &path) override;
  Boxed<std::vector<uint8_t>> next() override;
  void close();

private:
  std::unique_ptr<Context> ctx_;
};

} // namespace diskcat