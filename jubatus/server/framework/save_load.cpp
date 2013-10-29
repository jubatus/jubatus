// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include "save_load.hpp"

#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <glog/logging.h>
#include <pficommon/lang/cast.h>

#include "jubatus/core/common/exception.hpp"
#include "jubatus/core/common/big_endian.hpp"
#include "jubatus/core/common/crc32.hpp"
#include "jubatus/core/framework/mixable.hpp"

using jubatus::core::common::write_big_endian;
using jubatus::core::common::read_big_endian;
using std::string;
using pfi::lang::lexical_cast;

namespace jubatus {
namespace server {
namespace framework {

namespace {

const char magic_number[8] = "jubatus";
const uint64_t format_version = 1;

uint32_t jubatus_version_major = -1;
uint32_t jubatus_version_minor = -1;
uint32_t jubatus_version_maintenance = -1;

// TODO(gintenlabo): remove sscanf
void init_versions() {
  if (jubatus_version_major == static_cast<uint32_t>(-1)) {
    int major, minor, maintenance;
    std::sscanf(JUBATUS_VERSION, "%d.%d.%d", &major, &minor, &maintenance);  // NOLINT
    jubatus_version_major = major;
    jubatus_version_minor = minor;
    jubatus_version_maintenance = maintenance;
  }
}

const uint64_t system_data_container_version = 1;

struct system_data_container {
  uint64_t version;
  time_t timestamp;
  std::string type;
  std::string id;
  std::string config;

  system_data_container()
      : version(), timestamp(), type(), id(), config() {
  }

  system_data_container(const server_base& server, const std::string& id_)
      : version(system_data_container_version),
        timestamp(std::time(NULL)),
        type(server.argv().type), id(id_),
        config(server.get_config()) {
  }

  MSGPACK_DEFINE(version, timestamp, type, id, config);
};

uint32_t calc_crc32(const char* header,  // header size is 28 (fixed)
    const char* system_data, uint64_t system_data_size,
    const char* user_data, uint64_t user_data_size) {
  uint32_t crc32 = core::common::calc_crc32(header, 28);
  crc32 = core::common::calc_crc32(&header[32], 16, crc32);
  crc32 = core::common::calc_crc32(system_data, system_data_size, crc32);
  crc32 = core::common::calc_crc32(user_data, user_data_size, crc32);
  return crc32;
}

}  // namespace

void save_server(std::ostream& os,
    const server_base& server, const std::string& id) {
  if (id == "") {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("empty id is not allowed"));
  }

  init_versions();

  msgpack::sbuffer system_data_buf;
  msgpack::pack(&system_data_buf, system_data_container(server, id));

  msgpack::sbuffer user_data_buf;
  {
    msgpack::packer<msgpack::sbuffer> packer(user_data_buf);
    packer.pack_array(2);

    uint64_t user_data_version = server.user_data_version();
    packer.pack(user_data_version);
    server.get_mixable_holder()->pack(packer);
  }

  char header_buf[48];
  std::memcpy(header_buf, magic_number, 8);
  write_big_endian(format_version, &header_buf[8]);
  write_big_endian(jubatus_version_major, &header_buf[16]);
  write_big_endian(jubatus_version_minor, &header_buf[20]);
  write_big_endian(jubatus_version_maintenance, &header_buf[24]);
  // write_big_endian(crc32, &header_buf[28]);  // skipped
  write_big_endian(static_cast<uint64_t>(system_data_buf.size()),
                   &header_buf[32]);
  write_big_endian(static_cast<uint64_t>(user_data_buf.size()),
                   &header_buf[40]);

  uint32_t crc32 = calc_crc32(header_buf,
      system_data_buf.data(), system_data_buf.size(),
      user_data_buf.data(), user_data_buf.size());
  write_big_endian(crc32, &header_buf[28]);

  os.write(header_buf, 48);
  os.write(system_data_buf.data(), system_data_buf.size());
  os.write(user_data_buf.data(), user_data_buf.size());
}

void load_server(std::istream& is,
    server_base& server, const std::string& id) {
  init_versions();

  char header_buf[48];
  is.read(header_buf, 48);
  if (std::memcmp(header_buf, magic_number, 8) != 0) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("invalid file format"));
  }
  uint64_t format_version_read = read_big_endian<uint64_t>(&header_buf[8]);
  if (format_version_read != format_version) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(
          "invalid format version: " +
          lexical_cast<string>(format_version_read) +
          ", expected " +
          lexical_cast<string>(format_version)));
  }
  uint32_t jubatus_major_read = read_big_endian<uint32_t>(&header_buf[16]);
  uint32_t jubatus_minor_read = read_big_endian<uint32_t>(&header_buf[20]);
  uint32_t jubatus_maintenance_read =
      read_big_endian<uint32_t>(&header_buf[24]);
  if (jubatus_major_read != jubatus_version_major ||
      jubatus_minor_read != jubatus_version_minor ||
      jubatus_maintenance_read != jubatus_version_maintenance) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(
          "jubatus version mismatched: current version: " JUBATUS_VERSION
          ", saved version: " +
          lexical_cast<std::string>(jubatus_major_read) + "." +
          lexical_cast<std::string>(jubatus_minor_read) + "." +
          lexical_cast<std::string>(jubatus_maintenance_read)));
  }
  uint32_t crc32_expected = read_big_endian<uint32_t>(&header_buf[28]);
  uint64_t system_data_size = read_big_endian<uint64_t>(&header_buf[32]);
  uint64_t user_data_size = read_big_endian<uint64_t>(&header_buf[40]);

  std::vector<char> system_data_buf(system_data_size);
  is.read(&system_data_buf[0], system_data_size);

  std::vector<char> user_data_buf(user_data_size);
  is.read(&user_data_buf[0], user_data_size);

  uint32_t crc32_actual = calc_crc32(header_buf,
      &system_data_buf[0], system_data_size,
      &user_data_buf[0], user_data_size);
  if (crc32_actual != crc32_expected) {
    std::ostringstream ss;
    ss << "invalid crc32 checksum: " << std::hex << crc32_actual;
    ss << ", read " << crc32_expected;
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(ss.str()));
  }

  system_data_container system_data_actual;
  {
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, &system_data_buf[0], system_data_size);
    unpacked.get().convert(&system_data_actual);
  }
  system_data_container system_data_expected(server, id);
  if (system_data_actual.version != system_data_expected.version) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(
          "invalid system data version: saved version: " +
          lexical_cast<string>(system_data_actual.version) +
          ", expected " +
          lexical_cast<string>(system_data_expected.version)));
  }
  if (system_data_actual.type != system_data_expected.type) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(
          "server type mismatched: " + system_data_actual.type +
          ", expected " + system_data_expected.type));
  }
  if (id != "" && system_data_actual.id != system_data_expected.id) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(
          "server id mismatched: " + system_data_actual.type +
          ", expected " + system_data_expected.type));
  }
  if (system_data_actual.config != system_data_expected.config) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(
          "server config mismatched" + system_data_actual.config +
          ", expected " + system_data_expected.config));
  }

  {
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, &user_data_buf[0], user_data_size);

    std::vector<msgpack::object> objs;
    unpacked.get().convert(&objs);
    if (objs.size() != 2) {
      throw JUBATUS_EXCEPTION(
          core::common::exception::runtime_error("invalid user container"));
    }

    uint64_t user_data_version_expected = server.user_data_version();
    uint64_t user_data_version_actual;
    objs[0].convert(&user_data_version_actual);
    if (user_data_version_actual != user_data_version_expected) {
      throw JUBATUS_EXCEPTION(
          core::common::exception::runtime_error(
            "user data version mismatched: " +
            lexical_cast<string>(user_data_version_actual) +
            ", expected " +
            lexical_cast<string>(user_data_version_expected)));
    }

    server.get_mixable_holder()->unpack(objs[1]);
  }
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus
