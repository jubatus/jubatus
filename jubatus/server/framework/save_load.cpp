// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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
#include "jubatus/util/lang/cast.h"
#include "jubatus/util/text/json.h"

#include "jubatus/core/common/exception.hpp"
#include "jubatus/core/common/big_endian.hpp"
#include "jubatus/core/framework/mixable.hpp"
#include "jubatus/core/framework/stream_writer.hpp"

#include "../common/logger/logger.hpp"
#include "../common/crc32.hpp"

using jubatus::core::common::write_big_endian;
using jubatus::core::common::read_big_endian;
using std::string;
using jubatus::util::lang::lexical_cast;

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
  uint32_t crc32 = common::calc_crc32(header, 28);
  crc32 = common::calc_crc32(&header[32], 16, crc32);
  crc32 = common::calc_crc32(system_data, system_data_size, crc32);
  crc32 = common::calc_crc32(user_data, user_data_size, crc32);
  return crc32;
}

bool fwrite_helper(const char* buffer, size_t size, FILE* fp) {
  return fwrite(buffer, 1, size, fp) == size;
}

/**
 * Compare the given two config strings.  Returns true if they are
 * semantically the same (i.e., ignoring spaces and line breaks etc.)
 */
bool compare_config(const std::string& left, const std::string& right) {
  using jubatus::util::text::json::json;
  return
      lexical_cast<std::string>(lexical_cast<json>(left)) ==
      lexical_cast<std::string>(lexical_cast<json>(right));
}

}  // namespace

void save_server(FILE* fp,
    const server_base& server, const std::string& id) {
  init_versions();

  msgpack::sbuffer system_data_buf;
  msgpack::pack(&system_data_buf, system_data_container(server, id));

  msgpack::sbuffer user_data_buf;
  {
    core::framework::stream_writer<msgpack::sbuffer> st(user_data_buf);
    core::framework::jubatus_packer jp(st);
    core::framework::packer packer(jp);
    packer.pack_array(2);

    uint64_t user_data_version = server.user_data_version();
    packer.pack(user_data_version);
    server.get_driver()->pack(packer);
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

  if (!fwrite_helper(header_buf, sizeof(header_buf), fp)) {
    throw std::ios_base::failure("Failed to write header_buf.");
  }
  if (!fwrite_helper(system_data_buf.data(), system_data_buf.size(), fp)) {
    throw std::ios_base::failure("Failed to write system_data_buf.");
  }
  if (!fwrite_helper(user_data_buf.data(), user_data_buf.size(), fp)) {
    throw std::ios_base::failure("Failed to write user_data_buf.");
  }
}

void load_server(
    std::istream& is,
    server_base& server,
    const std::string& id,
    bool overwrite_config) {
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
  if (jubatus_major_read != 1) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(
          "cannot load model file created before v1.0.0: " +
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
    ss << ", expected " << crc32_expected;
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(ss.str()));
  }

  system_data_container system_data_actual;
  try {
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, &system_data_buf[0], system_data_size);
    unpacked.get().convert(&system_data_actual);
  } catch (const msgpack::type_error&) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(
          "system data is broken"));
  }

  if (overwrite_config) {
    server.set_config(system_data_actual.config);
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

  if (!compare_config(
      system_data_actual.config, system_data_expected.config)) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(
          "server config mismatched: " + system_data_actual.config +
          ", expected " + system_data_expected.config));
  }

  try {
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

    server.get_driver()->unpack(objs[1]);
  } catch (const msgpack::type_error&) {
    throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error(
          "user data is broken"));
  }
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus
