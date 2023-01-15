/**
 * Copyright (c) 2023 - Stanislav Ionascu <stanislav.ionascu@gmail.com>
 * This file is part of Diskcat.
 * SPDX-License-Identifier: MIT or GPL-2.0-or-later
 */

#pragma once

#include <filesystem>
#include <memory>

namespace diskcat {

class Parser;

std::unique_ptr<Parser> probe(const std::filesystem::path &path);

} // namespace diskcat