// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SYSTEM_SYS_INFO_H_
#define BASE_SYSTEM_SYS_INFO_H_

#include <stddef.h>
#include <stdint.h>

#include <map>
#include <string>

#include "base/base_export.h"
#include "base/callback_forward.h"
//#include "base/gtest_prod_util.h"
//#include "base/time/time.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"

// Added
#include <unistd.h>
namespace base {

namespace debug {
//FORWARD_DECLARE_TEST(SystemMetricsTest, ParseMeminfo);
}

class FilePath;
struct SystemMemoryInfoKB;

class BASE_EXPORT SysInfo {
 public:
  // Return the number of logical processors/cores on the current machine.
  static int NumberOfProcessors();

  // Return the number of bytes of physical memory on the current machine.
  static int64_t AmountOfPhysicalMemory();

  // Return the number of bytes of current available physical memory on the
  // machine.
  // (The amount of memory that can be allocated without any significant
  // impact on the system. It can lead to freeing inactive file-backed
  // and/or speculative file-backed memory).
  static int64_t AmountOfAvailablePhysicalMemory();

  // Return the number of bytes of virtual memory of this process. A return
  // value of zero means that there is no limit on the available virtual
  // memory.
  static int64_t AmountOfVirtualMemory();

  // Return the number of megabytes of physical memory on the current machine.
  static int AmountOfPhysicalMemoryMB() {
    return static_cast<int>(AmountOfPhysicalMemory() / 1024 / 1024);
  }

  // Return the number of megabytes of available virtual memory, or zero if it
  // is unlimited.
  static int AmountOfVirtualMemoryMB() {
    return static_cast<int>(AmountOfVirtualMemory() / 1024 / 1024);
  }

  // Return the available disk space in bytes on the volume containing |path|,
  // or -1 on failure.
  static int64_t AmountOfFreeDiskSpace(const FilePath& path);

  // Return the total disk space in bytes on the volume containing |path|, or -1
  // on failure.
  static int64_t AmountOfTotalDiskSpace(const FilePath& path);

#if defined(OS_FUCHSIA)
  // Sets the total amount of disk space to report under the specified |path|.
  // If |bytes| is -ve then any existing entry for |path| is removed.
  static void SetAmountOfTotalDiskSpace(const FilePath& path, int64_t bytes);
#endif

  // Returns system uptime.
  //static TimeDelta Uptime();

  // Returns a descriptive string for the current machine model or an empty
  // string if the machine model is unknown or an error occurred.
  // e.g. "MacPro1,1" on Mac, "iPhone9,3" on iOS or "Nexus 5" on Android. Only
  // implemented on OS X, iOS, Android and Chrome OS. This returns an empty
  // string on other platforms.
  static std::string HardwareModelName();

  struct HardwareInfo {
    std::string manufacturer;
    std::string model;
  };
  // Returns via |callback| a struct containing descriptive UTF-8 strings for
  // the current machine manufacturer and model, or empty strings if the
  // information is unknown or an error occurred. Implemented on Windows, OS X,
  // iOS, Linux, Chrome OS and Android.
  static void GetHardwareInfo(base::OnceCallback<void(HardwareInfo)> callback);

  // Returns the name of the host operating system.
  static std::string OperatingSystemName();

  // Returns the version of the host operating system.
  static std::string OperatingSystemVersion();

  // Retrieves detailed numeric values for the OS version.
  // DON'T USE THIS ON THE MAC OR WINDOWS to determine the current OS release
  // for OS version-specific feature checks and workarounds. If you must use
  // an OS version check instead of a feature check, use the base::mac::IsOS*
  // family from base/mac/mac_util.h, or base::win::GetVersion from
  // base/win/windows_version.h.
  static void OperatingSystemVersionNumbers(int32_t* major_version,
                                            int32_t* minor_version,
                                            int32_t* bugfix_version);

  // Returns the architecture of the running operating system.
  // Exact return value may differ across platforms.
  // e.g. a 32-bit x86 kernel on a 64-bit capable CPU will return "x86",
  //      whereas a x86-64 kernel on the same CPU will return "x86_64"
  static std::string OperatingSystemArchitecture();

  // Returns the architecture of the running process, which might be different
  // than the architecture returned by OperatingSystemArchitecture() (e.g.
  // macOS Rosetta, a 32-bit binary on a 64-bit OS, etc).
  // Will return one of: "x86", "x86_64", "ARM", "ARM_64", or an empty string if
  // none of the above.
  static std::string ProcessCPUArchitecture();

  // Avoid using this. Use base/cpu.h to get information about the CPU instead.
  // http://crbug.com/148884
  // Returns the CPU model name of the system. If it can not be figured out,
  // an empty string is returned.
  static std::string CPUModelName();

  // Return the smallest amount of memory (in bytes) which the VM system will
  // allocate.
  static size_t VMAllocationGranularity();

#if defined(OS_ANDROID)
  // Returns the Android build's codename.
  static std::string GetAndroidBuildCodename();

  // Returns the Android build ID.
  static std::string GetAndroidBuildID();

  // Returns the Android hardware EGL system property.
  static std::string GetAndroidHardwareEGL();

  static int DalvikHeapSizeMB();
  static int DalvikHeapGrowthLimitMB();
#endif  // defined(OS_ANDROID)

#if defined(OS_IOS)
  // Returns the iOS build number string which is normally an alphanumeric
  // string like 12E456. This build number can differentiate between different
  // versions of iOS that may have the same major/minor/bugfix version numbers.
  // For example, iOS beta releases have the same version number but different
  // build number strings.
  static std::string GetIOSBuildNumber();
#endif  // defined(OS_IOS)

  // Returns true for low-end devices that may require extreme tradeoffs,
  // including user-visible changes, for acceptable performance.
  // For general memory optimizations, consider |AmountOfPhysicalMemoryMB|.
  //
  // On Android this returns:
  //   true when memory <= 1GB on Android O and later.
  //   true when memory <= 512MB on Android N and earlier.
  // This is not the same as "low-memory" and will be false on a large number of
  // <=1GB pre-O Android devices. See: |detectLowEndDevice| in SysUtils.java.
  // On Desktop this returns true when memory <= 512MB.
  static bool IsLowEndDevice();

 private:
  /*
  FRIEND_TEST_ALL_PREFIXES(SysInfoTest, AmountOfAvailablePhysicalMemory);
  FRIEND_TEST_ALL_PREFIXES(debug::SystemMetricsTest, ParseMeminfo);
  */

  static int64_t AmountOfPhysicalMemoryImpl();
  static int64_t AmountOfAvailablePhysicalMemoryImpl();
  static bool IsLowEndDeviceImpl();
  static HardwareInfo GetHardwareInfoSync();

#if defined(OS_LINUX) || defined(OS_CHROMEOS) || defined(OS_ANDROID) || \
    defined(OS_AIX)
  static int64_t AmountOfAvailablePhysicalMemory(
      const SystemMemoryInfoKB& meminfo);
#endif
};

// Added from  sys_info_posix.h
// static
size_t SysInfo::VMAllocationGranularity() {
  return getpagesize();
}

}  // namespace base

#endif  // BASE_SYSTEM_SYS_INFO_H_
