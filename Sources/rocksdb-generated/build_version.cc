// Generated build_version.cc for SwiftRocksDB
// Based on rocksdb/util/build_version.cc.in with template variables resolved.

#include <memory>
#include <unordered_map>
#include <string>

#include "rocksdb/version.h"
#include "rocksdb/utilities/object_registry.h"

// No plugins
std::unordered_map<std::string, ROCKSDB_NAMESPACE::RegistrarFunc>
    ROCKSDB_NAMESPACE::ObjectRegistry::builtins_ = {};

namespace ROCKSDB_NAMESPACE {

const std::unordered_map<std::string, std::string>& GetRocksBuildProperties() {
  static const std::unordered_map<std::string, std::string> props;
  return props;
}

std::string GetRocksVersionAsString(bool with_patch) {
  std::string version =
      std::to_string(ROCKSDB_MAJOR) + "." + std::to_string(ROCKSDB_MINOR);
  if (with_patch) {
    return version + "." + std::to_string(ROCKSDB_PATCH);
  } else {
    return version;
  }
}

std::string GetRocksBuildInfoAsString(const std::string& program,
                                      bool verbose) {
  std::string info = program + " (RocksDB) " + GetRocksVersionAsString(true);
  if (verbose) {
    for (const auto& it : GetRocksBuildProperties()) {
      info.append("\n    ");
      info.append(it.first);
      info.append(": ");
      info.append(it.second);
    }
  }
  return info;
}

}  // namespace ROCKSDB_NAMESPACE
