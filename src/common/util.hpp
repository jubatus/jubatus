// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#pragma once
#include <string>
#include <vector>
#include <map>

#include <stdint.h>

namespace jubatus{
namespace util{

void get_ip(const char* nic, std::string& out);
std::string get_ip(const char* nic);
std::string get_program_name();

std::string load(const std::string& file, std::vector< std::pair<std::string, int> >& s);

int daemonize();

void append_env_path(const std::string& env_, const std::string& argv0);
void append_server_path(const std::string& argv0);

void get_machine_status(std::map<std::string, std::string>&);

void set_exit_on_term();
void ignore_sigpipe();

} //util
} //namespace jubatus
