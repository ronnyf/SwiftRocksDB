# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SwiftRocksDB is a Swift package wrapping RocksDB v10.10.1 (embedded persistent key-value store) for iOS and macOS. It uses Swift 6.3's C++ interoperability mode (`.interoperabilityMode(.Cxx)`) to call RocksDB C++ APIs directly from Swift without a C shim layer.

RocksDB source is included as a git submodule at `Sources/rocksdb/` from an internal fork.

## Build Commands

```bash
swift build                    # Build all targets
swift build --product swrocks  # Build just the CLI executable
swift build -c release         # Release build
swift test                     # Run tests (Swift Testing framework)
swift run swrocks              # Run the CLI tool
```

The initial build compiles 727 RocksDB C++ source files and is slow. Subsequent incremental builds are fast.

## Architecture

**Three SPM targets:**

- **`rocksdb`** — C++ library target wrapping the RocksDB submodule. Compiles all .cc files with C++20, defines `ROCKSDB_PLATFORM_POSIX`, `ROCKSDB_LIB_IO_POSIX`, `PORTABLE`, `OS_MACOSX`. Public headers exposed from `Sources/rocksdb/include/`.
- **`SwiftRocksDB`** — Swift wrapper library. Depends on `rocksdb`. This is where Swift-friendly APIs live. Currently a placeholder.
- **`SwiftRocksDB-cli`** (product name: `swrocks`) — Executable target. Depends on `rocksdb` directly with C++ interop enabled.

**Key design choice:** Package.swift explicitly enumerates all 727 .cc source files rather than using directory-based inclusion. When adding/removing RocksDB source files, Package.swift must be updated.

## Platform Requirements

- Swift 6.3+ (swift-tools-version: 6.3, language mode: v6)
- C++20
- iOS 26+ / macOS 26+
- Xcode 16+

## Key Files

- `Package.swift` — ~411 lines; contains explicit source file list for RocksDB
- `Sources/rocksdb/include/rocksdb/c.h` — C bindings (alternative to direct C++ interop)
- `Sources/rocksdb/include/rocksdb/db.h` — Core DB C++ API
- `Sources/rocksdb/include/rocksdb/options.h` — Configuration options
