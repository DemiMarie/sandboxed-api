// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CONTRIB_FILE_FILE_SAPI_H_
#define CONTRIB_FILE_FILE_SAPI_H_

#include <syscall.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "sandboxed_api/util/fileops.h"
#include "file_sapi.sapi.h"  // NOLINT(build/include)
class FileSapiSandbox : public file_sapi::FileSandbox {
 public:
  std::unique_ptr<sandbox2::Policy> ModifyPolicy(
      sandbox2::PolicyBuilder*) override {
    auto builder = sandbox2::PolicyBuilder()
        .AllowSystemMalloc()
        .AllowRead()
        .AllowStat()
        .AllowWrite()
        .AllowExit()
        .AllowMmap()
        .AllowSyscalls({
            __NR_futex,
            __NR_close,
            __NR_lseek,
            __NR_getpid,
            __NR_clock_gettime,
            __NR_recvmsg,
            __NR_openat,
            __NR_mprotect,
        });
    for (auto p : {"/etc/magic.mgc", "/usr/share/misc/magic.mgc"}) {
      struct stat64 st;
      if (::lstat64(p, &st) == 0 && S_ISREG(st.st_mode)) builder.AddFile(p);
    }
    {
      const char *p = "/usr/share/misc/magic";
      struct stat64 st;
      if (::lstat64(p, &st) == 0 && S_ISDIR(st.st_mode)) builder.AddDirectory(p);
    }
    return builder.BuildOrDie();
  }
};

#endif  // CONTRIB_FILE_FILE_SAPI_H_

