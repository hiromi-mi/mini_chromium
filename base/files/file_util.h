// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_FILES_FILE_UTIL_H_
#define MINI_CHROMIUM_BASE_FILES_FILE_UTIL_H_

#include "build/build_config.h"

#if defined(OS_POSIX)

#include <sys/types.h>

#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/check.h"

namespace base {

bool ReadFromFD(int fd, char* buffer, size_t bytes);
int WriteFile(const FilePath& filename, const char* data, int size);
bool AllocateFileRegion(File* file, int64_t offset, size_t size);
bool WriteFileDescriptor(const int fd, const char* data, int size);

}  // namespace base

#endif  // OS_POSIX

#endif  // MINI_CHROMIUM_BASE_FILES_FILE_UTIL_H_
