/**
 * Copyright (c) 2023 - Stanislav Ionascu <stanislav.ionascu@gmail.com>
 * This file is part of Diskcat.
 * SPDX-License-Identifier: MIT or GPL-2.0-or-later
 */

#pragma once

#include "parser.h"

namespace diskcat {
class DvdParser : public Parser {
  struct Context;
public:
  DvdParser();
  ~DvdParser() override;

  void open(const std::filesystem::path &path, int selected_title = -1) override;
  Boxed<std::vector<uint8_t>> next() override;
  void close() override;

private:
  std::unique_ptr<Context> ctx_{nullptr};
  int selected_title_;
};
} // namespace diskcat
