# RocksDB SwiftPM Build Notes

How RocksDB v10.10.1 is integrated as a Swift Package Manager C++ target for macOS and iOS (ARM64).

## Source Files

### Explicitly enumerated in Package.swift

RocksDB's CMake build uses directory globbing, but SPM requires explicit source file lists. `Package.swift` enumerates ~729 `.cc` files.

### Platform-specific sources

These files are required for POSIX/macOS but are easy to miss since CMake adds them conditionally:

| File | Purpose |
|------|---------|
| `env/env_posix.cc` | `Env::Default()`, `SystemClock::Default()` — without this, every RocksDB operation fails at link time |
| `env/io_posix.cc` | POSIX I/O implementations |
| `env/fs_posix.cc` | POSIX filesystem implementations |
| `port/port_posix.cc` | POSIX port layer (mutexes, threads, etc.) |
| `util/crc32c_arm64.cc` | ARM64 CRC32C intrinsics — required on Apple Silicon, referenced by `util/crc32c.cc` |

### Source file audit

The RocksDB source tree contains ~727 `.cc` files. Of those, ~350 are included in `Package.swift`. The remainder are correctly excluded:

| Category | Count | Reason |
|----------|-------|--------|
| Test files (`*_test.cc`) | 216 | Unit tests |
| Java/JNI bindings (`java/rocksjni/`) | 82 | JNI wrapper, not needed for Swift |
| Benchmark files (`*_bench.cc`) | 16 | Performance benchmarks |
| Standalone tool mains (`int main()`) | 11 | Separate executables (ldb, sst_dump, etc.) |
| db_stress_tool (except `_compression_manager`) | 19 | Stress test executable |
| Example files (`examples/`) | 9 | Standalone examples |
| Windows-only (`port/win/`) | 8 | Wrong platform |
| Test-only utilities | 7 | `db_test_util.cc`, `mock_table.cc`, etc. |
| Fuzz harnesses (`fuzz/`) | 3 | Fuzz testing |
| Third-party (gtest) | 2 | Google Test framework |
| Microbench | 2 | Micro-benchmarks |
| `faiss_ivf_index.cc` | 1 | Requires external FAISS library |

**Notable inclusions:**

- `test_util/testutil.cc` — Included in `Package.swift` matching `CMakeLists.txt` behavior, even though `src.mk` places it in `TEST_LIB_SOURCES`. Provides sync point utilities used at runtime.
- `db_stress_tool/db_stress_compression_manager.cc` — The one `db_stress_tool/` file that is a library source (in both `CMakeLists.txt` `SOURCES` and `src.mk` `TOOL_LIB_SOURCES`), not part of the stress tool executable.
- `utilities/convenience/info_log_finder.cc` — Present in `src.mk` `LIB_SOURCES` but missing from `CMakeLists.txt` (upstream omission). Implements `GetInfoLogList()` from public header `rocksdb/utilities/info_log_finder.h`.

**Intentionally excluded borderline file:**

- `monitoring/thread_status_updater_debug.cc` — In `src.mk` `LIB_SOURCES` but `CMakeLists.txt` puts it in `TESTUTIL_SOURCE`. Entire file is wrapped in `#ifndef NDEBUG`, compiling to nothing in release builds.

### Generated source: `build_version.cc`

RocksDB's build generates `util/build_version.cc` from a template at build time. Since SPM has no code generation step, a static copy lives in `Sources/rocksdb-generated/build_version.cc` as a separate `rocksdb-generated` target. This provides:

- `GetRocksVersionAsString()` -> `"10.10.1"`
- `rocksdb_build_git_sha` / `rocksdb_build_git_date`
- `rocksdb_build_compile_date`

## Preprocessor Defines

### Defines used (from CMakeLists.txt analysis)

| Define | Source | Purpose |
|--------|--------|---------|
| `ROCKSDB_PLATFORM_POSIX` | CMakeLists.txt:540 | Selects POSIX platform layer (non-Windows) |
| `ROCKSDB_LIB_IO_POSIX` | CMakeLists.txt:540 | Enables POSIX I/O code paths |
| `OS_MACOSX` | CMakeLists.txt:513 | macOS/iOS-specific code (e.g., `F_FULLFSYNC`) |
| `IOS_CROSS_COMPILE` | CMakeLists.txt:515 | iOS-only; disables features unavailable on iOS |
| `PORTABLE` | Manual | Disables CPU-specific optimizations for broad compatibility |
| `HAVE_FULLFSYNC` | CMakeLists.txt:599 | `F_FULLFSYNC` exists on Apple platforms for durable writes |
| `HAVE_UINT128_EXTENSION` | build_detect_platform:738 | `__uint128_t` supported by Apple Clang |

### Defines NOT used (Linux-only, confirmed via CMake compile checks)

| Define | Why excluded |
|--------|-------------|
| `ROCKSDB_PTHREAD_ADAPTIVE_MUTEX` | `PTHREAD_MUTEX_ADAPTIVE_NP` is a Linux glibc extension |
| `ROCKSDB_MALLOC_USABLE_SIZE` | `malloc_usable_size()` is Linux; macOS uses `malloc_size()` |
| `ROCKSDB_SCHED_GETCPU_PRESENT` | `sched_getcpu()` is Linux-only |
| `ROCKSDB_AUXV_GETAUXVAL_PRESENT` | `getauxval()` is Linux-only |
| `ROCKSDB_FALLOCATE_PRESENT` | `fallocate()` is Linux-only |
| `ROCKSDB_RANGESYNC_PRESENT` | `sync_file_range()` is Linux-only |

## Submodule Patches

Two patches are applied to the RocksDB submodule to enable SwiftPM's auto-generated module map (no custom `module.modulemap` needed):

### `include/rocksdb/c.h` — move includes above `extern "C"`

The original `c.h` places `#include <stdint.h>` etc. inside `extern "C" {}`. When Swift's C++ module system parses this, it fails with: `error: import of C++ module 'std_stdint_h' appears within extern "C" language linkage specification`. Moving the includes above the `extern "C"` block fixes this — the headers are valid in both C and C++.

### `include/rocksdb/utilities/lua/` — deleted

The Lua headers (`rocks_lua_util.h`, `rocks_lua_custom_library.h`) `#include <lauxlib.h>` which doesn't exist on macOS/iOS. These support an optional Lua CompactionFilter feature that requires linking an external Lua library. Since no Lua `.cc` source files are compiled, the headers serve no purpose and are deleted.

### Why not a custom module.modulemap?

A custom modulemap with `exclude header` directives would also work, but patching the submodule is simpler — it eliminates a file that would need to be kept in sync with upstream header changes. Both approaches require re-applying after submodule updates.

## SPM Target Structure

```
rocksdb (C++ library)
  └── Sources/rocksdb/        — RocksDB v10.10.1 source (git submodule)

rocksdb-generated (C++ library, depends on rocksdb)
  └── Sources/rocksdb-generated/
      ├── build_version.cc    — static build version info
      └── include/            — public headers (if any)

SwiftRocksDB (Swift library, depends on rocksdb + rocksdb-generated)
  └── Sources/SwiftRocksDB/   — Swift wrapper APIs

SwiftRocksDB-cli (executable "swrocks", depends on rocksdb + rocksdb-generated)
  └── Sources/SwiftRocksDB-cli/
```

## Updating RocksDB Version

When upgrading the RocksDB submodule:

1. Update `Sources/rocksdb` submodule to new version
2. Diff the new `CMakeLists.txt` for added/removed source files and new defines
3. Update the `sources` array in `Package.swift`
4. Update `Sources/rocksdb-generated/build_version.cc` with new version strings
5. Re-apply submodule patches: move `c.h` includes above `extern "C"`, delete Lua headers
6. Verify clean build with `swift build --product swrocks`
