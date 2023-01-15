/**
 * Copyright (c) 2023 - Stanislav Ionascu <stanislav.ionascu@gmail.com>
 * This file is part of Diskcat.
 * SPDX-License-Identifier: MIT or GPL-2.0-or-later
 */

#pragma once

#include <string>
#include <variant>

namespace diskcat {

class Error {
public:
  Error(std::string &&err_msg) : msg_(std::move(err_msg)) {}

  const std::string message() const { return msg_; }

private:
  std::string msg_;
};

template <class T> using Boxed = std::variant<Error, T>;

} // namespace diskcat
