/**
 * Copyright (c) 2023 - Stanislav Ionascu <stanislav.ionascu@gmail.com>
 * This file is part of Diskcat.
 * SPDX-License-Identifier: MIT or GPL-2.0-or-later
 */

#include <filesystem>
#include <iostream>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/log/globals.h>
#include <absl/log/initialize.h>
#include <absl/log/log.h>
#include <absl/log/globals.h>
#include <absl/log/log_sink.h>
#include <absl/log/log_sink_registry.h>
#include <absl/strings/str_format.h>
#include <absl/strings/str_join.h>

#include "parser.h"
#include "probe.h"

namespace fs = std::filesystem;

ABSL_FLAG(int, title, -1, "Title number to use for playback. Default picks the longest detected title.");
ABSL_FLAG(int, angle, 0, "Angle to select for playback. Defaults to 0.");
ABSL_FLAG(int, min_title_length, 180, "Ignore titles that are shorter than 180s.");

struct LogSink : absl::LogSink {
  void Send(const absl::LogEntry& entry) override {
    if (entry.log_severity() < absl::StderrThreshold()) {
      std::cerr << entry.text_message_with_prefix_and_newline();
    }
  }
};

int main(int argc, char **argv) {
  using namespace diskcat;

  auto log_sink = std::make_unique<LogSink>();
  absl::AddLogSink(log_sink.get());

  absl::InitializeLog();
  absl::SetProgramUsageMessage(
      R"(diskcat outputs content of the DVDs and BluRays to stdout. Sample usage: diskcat FILE_ISO_OR_DIR)");

  auto flags = absl::ParseCommandLine(argc, argv);
  if (flags.size() <= 1) {
    LOG(ERROR) << "No input file provided.";
    return 1;
  }

  auto path = fs::path(flags[1]);
  {
    std::unique_ptr<Parser> parser = probe(path);
    parser->open(path);

    bool done = false;
    while (!done) {
      auto maybeBuffer = parser->next();
      if (std::holds_alternative<Error>(maybeBuffer)) {
        LOG(INFO) << std::get<Error>(std::move(maybeBuffer)).message();
        done = true;
        break;
      }

      const auto &buffer = std::get<std::vector<uint8_t>>(maybeBuffer);
      write(fileno(stdout), buffer.data(), buffer.size());
    }
    fflush(stdout);
  }

  return 0;
}