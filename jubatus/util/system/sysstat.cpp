// Copyright (c)2008-2011, Preferred Infrastructure Inc.
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of Preferred Infrastructure nor the names of other
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "sysstat.h"

#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <unistd.h>

#if defined(__linux__)
#include <sys/vfs.h>
#endif
#if defined(__sparcv8) || defined(__sparcv9)
#include <kstat.h>
#include <sys/statvfs.h>
#endif
#if defined(__APPLE__)
#include <sys/param.h>
#include <sys/mount.h>
#include <mach/mach_traps.h>
#include <mach/mach_types.h>
#include <mach/mach.h>
#endif

using namespace std;

namespace jubatus {
namespace util{
namespace system{
namespace sysstat{

static double get_loadavg();
static uint64_t get_total_memory();
static uint64_t get_free_memory();

#if defined(__sparcv8) || defined(__sparcv9)
#if defined(__sparcv8)
typedef uint32_t kstat_ret_type;
#else
typedef uint64_t kstat_ret_type;
#endif
static kstat_ret_type kstat_get_ui(const char* ks_module,
                                   int ks_instance,
                                   const char* ks_name,
                                   const char* name);
#endif

int get_sysstat(sysstat_ret& st)
{
  st.loadavg = get_loadavg();
  st.total_memory = get_total_memory();
  st.free_memory = get_free_memory();
  return 0;
}

static double get_loadavg()
{
#if defined(__linux__)
  ifstream fin("/proc/loadavg");

  string avg1m;
  string avg5m;
  string avg15m;
  fin >> avg1m >> avg5m >> avg15m;

  double d = atof(avg1m.c_str());
  return d;
#elif defined(__sparcv8) || defined(__sparcv9)
  kstat_ret_type kstat_res = kstat_get_ui("unix", 0, "system_misc", "avenrun_1min");
  double res = (double)kstat_res / 256.0;
  return res;
#elif defined(__APPLE__)
  struct host_load_info hl;
  mach_msg_type_number_t count = HOST_LOAD_INFO_COUNT;
  kern_return_t error = host_statistics(mach_host_self(), HOST_LOAD_INFO, (host_info_t)&hl,&count);
  double res = static_cast<double>(hl.avenrun[2]) / static_cast<double>(LOAD_SCALE);
  return res;
#else
#error I dont know how to get loadavg
#endif
}

static uint64_t get_total_memory()
{
#if defined(__linux__)
  ifstream fin("/proc/meminfo");
  string s;

  uint64_t memtotal = 0;
  while (getline(fin, s)) {
    if (s.find("MemTotal") == 0) {
      istringstream iss(s);
      string type;
      string mem;
      string kb;
      iss >> type >> mem >> kb;
      if (mem != "") {
        memtotal = atoll(mem.c_str());
        if (kb == "kB") {
          memtotal *= 1000;
        }
      }
    }
  }
  return memtotal;
#elif defined(__sparcv8) || defined(__sparcv9)
  kstat_ret_type kstat_res = kstat_get_ui("unix", 0, "system_pages", "availrmem");
  uint64_t res = (uint64_t)kstat_res * getpagesize();
  return res;
#elif defined(__APPLE__)
  struct host_basic_info host;
  mach_msg_type_number_t count = HOST_BASIC_INFO_COUNT;
  host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&host, &count);
  return host.memory_size;
#else
#error I dont know how to get total memory size
#endif
}

static uint64_t get_free_memory()
{
#if defined(__linux__)
  ifstream fin("/proc/meminfo");
  string s;

  uint64_t memfree = 0;
  uint64_t memcached = 0;
  while (getline(fin, s)) {
    if (s.find("MemFree") == 0) {
      istringstream iss(s);
      string type;
      string mem;
      string kb;
      iss >> type >> mem >> kb;
      if (mem != "") {
        memfree = atoi(mem.c_str());
        if (kb == "kB") {
          memfree *= 1000;
        }
      }
    }
    if (s.find("Cached") == 0) {
      istringstream iss(s);
      string type;
      string cached;
      string kb;
      iss >> type >> cached >> kb;
      if (cached != "") {
        memcached = atoi(cached.c_str());
        if (kb == "kB") {
          memcached *= 1000;
        }
      }
    }
  }

  return memfree + memcached;
#elif defined(__sparcv8) || defined(__sparcv9)
  kstat_ret_type kstat_res = kstat_get_ui("unix", 0, "system_pages", "freemem");
  uint64_t res = (uint64_t)kstat_res * getpagesize();
  return res;
#elif defined(__APPLE__)
  vm_statistics_data_t vm_info;
  mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
  host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vm_info, &count);
  return vm_info.free_count*vm_page_size;
#else
#error I dont know how to get free memory size
#endif
}


#if defined(__sparcv8) || defined(__sparcv9)
kstat_ret_type kstat_get_ui(const char* ks_module,
                            int ks_instance,
                            const char* ks_name,
                            const char* name)
{
  kstat_ctl_t* kc;
  kstat_t* ks;
  kstat_named_t* kn;
  kid_t kid;
  kstat_ret_type res = 0;

  kc = kstat_open();
  if (kc == NULL) {
    return res;
  }

  char* str1 = strdup(ks_module);
  char* str2 = strdup(ks_name);
  ks = kstat_lookup(kc, str1, 0, str2);
  free(str1);
  free(str2);
  if (ks == NULL) {
    goto KSTAT_GET_UI_RET;
  }

  kid = kstat_read(kc, ks, NULL);
  if (kid == -1) {
    goto KSTAT_GET_UI_RET;
  }

  str1 = strdup(name);
  kn = (kstat_named_t*)kstat_data_lookup(ks, str1);
  free(str1);
  if (kn == NULL) {
    goto KSTAT_GET_UI_RET;
  }

#if defined(__sparcv8)
  res = kn->value.ui32;
#else
  res = kn->value.ui64;
#endif

KSTAT_GET_UI_RET:
  kstat_close(kc);
  return res;
}
#endif

}
}
}
}
