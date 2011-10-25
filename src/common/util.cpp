// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include "util.hpp"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <string.h>

#include <pficommon/lang/exception.h>

#include <fstream>

#include <limits.h>

#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

using std::string;
using namespace pfi::lang;

namespace jubatus { namespace util{

void get_ip(const char* nic, string& out){
    int fd;
    struct ifreq ifr;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, nic, IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    struct sockaddr_in * sin = (struct sockaddr_in*)(&(ifr.ifr_addr));
    out = inet_ntoa((struct in_addr)(sin->sin_addr));
}

string get_ip(const char* nic){
  string ret;
  get_ip(nic, ret);
  return ret;
}

//local server list should be like:
//  192.168.1.2 9199
//  192.168.1.3 9199
//  192.168.1.4 10090
//  ...
//  192.168.1.23 2345
//and must include self IP got from "eth0"
std::string load(const std::string& file, std::vector< std::pair<std::string, int> >& s){
  std::string tmp;
  std::string self;
  get_ip("eth0", self);
  bool self_included = false;
  int port;
  int line = 0;
  std::ifstream ifs(file.c_str());
  if(!ifs){
    return self;
  }
  while(ifs >> tmp){
    if(self==tmp)
      self_included = true;
    if(!(ifs >> port)){
      throw parse_error(file, line, tmp.size(), string("input port"));
    }
    s.push_back(std::pair<std::string,int>(tmp, port));
    line++;
  }
  if(!self_included){
    throw parse_error(file, s.size(), 0, //FIXME: 0
                      string("self IP(eth0) not included in list"));
  }
  return self;
}


int daemonize(){
  return daemon(0, 0);
}

void append_env_path(const string& e, const string& argv0){
  const char * env = getenv(e.c_str());
  // char cwd[PATH_MAX];
  // getcwd(cwd, PATH_MAX);
  
  string new_path = string(env) + ":" + argv0;
  setenv(e.c_str(), new_path.c_str(), new_path.size());
}

void append_server_path(const string& argv0){
  const char * env = getenv("PATH");
  char cwd[PATH_MAX];
  if (!getcwd(cwd, PATH_MAX)) {
    char* s = strerror(errno);
    throw std::runtime_error(std::string(s));
  }  
  
  string p = argv0.substr(0, argv0.find_last_of('/'));
  string new_path = string(env) + ":" + cwd + "/" + p + "/../server";
  setenv("PATH", new_path.c_str(), new_path.size());

}

void get_machine_status(std::map<std::string, std::string>& ret){
  pid_t pid = getpid();

  // WARNING: this code will only work on linux
  std::ostringstream fname;
  fname << "/proc/" << pid << "/statm";
  std::ifstream statm(fname.str().c_str());

  uint64_t vm_virt; statm >> vm_virt;
  uint64_t vm_rss; statm >> vm_rss;
  uint64_t vm_shr; statm >> vm_shr;

  ret["VIRT"] = pfi::lang::lexical_cast<std::string>(vm_virt);
  ret["RSS"] = pfi::lang::lexical_cast<std::string>(vm_rss);
  ret["SHR"] = pfi::lang::lexical_cast<std::string>(vm_shr);

}

} //util
} //jubatus
