#pragma once

#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


extern std::shared_ptr<spdlog::logger> my_logger;

bool is_file_exists(const std::string &name);
