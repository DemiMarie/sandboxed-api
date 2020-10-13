// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "utils.h"  // NOLINT(build/include)

#include <unistd.h>

#include "sandboxed_api/sandbox2/util/fileops.h"
#include "sandboxed_api/sandbox2/util/temp_file.h"

namespace gdal::sandbox::utils {

namespace {

  inline constexpr absl::string_view kProjDbEnvVariableName = "PROJ_PATH";
  inline constexpr absl::string_view kDefaultProjDbPath 
      = "/usr/local/share/proj/proj.db";

}  // namespace

TempFile::TempFile(absl::string_view prefix) {
  auto file_data = sandbox2::CreateNamedTempFile(prefix);

  if (file_data.ok()) {
    file_data_ = std::move(file_data.value());
  }
}

TempFile::~TempFile() {
  if (HasValue()) {
    unlink(file_data_.value().first.c_str());
  }
}

bool TempFile::HasValue() const {
  return file_data_.has_value();
}

int TempFile::GetFd() const {
  return file_data_.value().second;
}

std::string TempFile::GetPath() const {
  return file_data_.value().first;
}

std::optional<std::string> FindProjDbPath() {
  const char* proj_db_path_ptr = std::getenv(kProjDbEnvVariableName.data());

  std::string proj_db_path = proj_db_path_ptr == nullptr ? 
      std::string(kDefaultProjDbPath) : std::string(proj_db_path_ptr);

  if (!sandbox2::file_util::fileops::Exists(proj_db_path, false)) {
    return std::nullopt;
  }

  return proj_db_path;
}

bool IsAbsolute(absl::string_view path) {
  return path.length() > 0 && path.at(0) == '/';
}

}  // namespace gdal::sandbox::utils
