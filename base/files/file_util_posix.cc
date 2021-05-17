// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_util.h"

#include <unistd.h>
#include <fcntl.h>

#include "base/posix/eintr_wrapper.h"
#include "base/bits.h"

namespace base {

bool ReadFromFD(int fd, char* buffer, size_t bytes) {
  size_t total_read = 0;
  while (total_read < bytes) {
    ssize_t bytes_read =
        HANDLE_EINTR(read(fd, buffer + total_read, bytes - total_read));
    if (bytes_read <= 0) {
      break;
    }
    total_read += bytes_read;
  }
  return total_read == bytes;
}

// Appended
bool AllocateFileRegion(File* file, int64_t offset, size_t size) {
  DCHECK(file);

  // Explicitly extend |file| to the maximum size. Zeros will fill the new
  // space. It is assumed that the existing file is fully realized as
  // otherwise the entire file would have to be read and possibly written.
  const int64_t original_file_len = file->GetLength();
  if (original_file_len < 0) {
    //DPLOG(ERROR) << "fstat " << file->GetPlatformFile();
    return false;
  }

  // Increase the actual length of the file, if necessary. This can fail if
  // the disk is full and the OS doesn't support sparse files.
  const int64_t new_file_len = offset + size;
  if (!file->SetLength(std::max(original_file_len, new_file_len))) {
    //DPLOG(ERROR) << "ftruncate " << file->GetPlatformFile();
    return false;
  }

  // Realize the extent of the file so that it can't fail (and crash) later
  // when trying to write to a memory page that can't be created. This can
  // fail if the disk is full and the file is sparse.

  // First try the more effective platform-specific way of allocating the disk
  // space. It can fail because the filesystem doesn't support it. In that case,
  // use the manual method below.

#if defined(OS_LINUX) || defined(OS_CHROMEOS)
  if (HANDLE_EINTR(fallocate(file->GetPlatformFile(), 0, offset, size)) != -1)
    return true;
  DPLOG(ERROR) << "fallocate";
#elif defined(OS_APPLE)
  // MacOS doesn't support fallocate even though their new APFS filesystem
  // does support sparse files. It does, however, have the functionality
  // available via fcntl.
  // See also: https://openradar.appspot.com/32720223
  fstore_t params = {F_ALLOCATEALL, F_PEOFPOSMODE, offset, size, 0};
  if (fcntl(file->GetPlatformFile(), F_PREALLOCATE, &params) != -1)
    return true;
  DPLOG(ERROR) << "F_PREALLOCATE";
#endif

  // Manually realize the extended file by writing bytes to it at intervals.
  int64_t block_size = 512;  // Start with something safe.
  stat_wrapper_t statbuf;
  if (File::Fstat(file->GetPlatformFile(), &statbuf) == 0 &&
      statbuf.st_blksize > 0 && base::bits::IsPowerOfTwo(statbuf.st_blksize)) {
    block_size = statbuf.st_blksize;
  }

  // Write starting at the next block boundary after the old file length.
  const int64_t extension_start =
      base::bits::AlignUp(original_file_len, block_size);
  for (int64_t i = extension_start; i < new_file_len; i += block_size) {
    char existing_byte;
    if (HANDLE_EINTR(pread(file->GetPlatformFile(), &existing_byte, 1, i)) !=
        1) {
      return false;  // Can't read? Not viable.
    }
    if (existing_byte != 0) {
      continue;  // Block has data so must already exist.
    }
    if (HANDLE_EINTR(pwrite(file->GetPlatformFile(), &existing_byte, 1, i)) !=
        1) {
      return false;  // Can't write? Not viable.
    }
  }

  return true;
}

// Appended

int WriteFile(const FilePath& filename, const char* data, int size) {
  //ScopedBlockingCall scoped_blocking_call(FROM_HERE, BlockingType::MAY_BLOCK);
  // einter_wrapper.h
  int fd = HANDLE_EINTR(creat(filename.value().c_str(), 0666));
  if (fd < 0)
    return -1;

  int bytes_written = WriteFileDescriptor(fd, data, size) ? size : -1;
  if (IGNORE_EINTR(close(fd)) < 0)
    return -1;
  return bytes_written;
}

// Appended
bool WriteFileDescriptor(const int fd, const char* data, int size) {
  // Allow for partial writes.
  ssize_t bytes_written_total = 0;
  for (ssize_t bytes_written_partial = 0; bytes_written_total < size;
       bytes_written_total += bytes_written_partial) {
    bytes_written_partial =
        HANDLE_EINTR(write(fd, data + bytes_written_total,
                           size - bytes_written_total));
    if (bytes_written_partial < 0)
      return false;
  }

  return true;
}


}  // namespace base
