//
//  File.swift
//  SwiftRocksDB
//
//  Created by Ronny Falk on 2/27/26.
//

import Foundation
import rocksdb

@main
struct SwiftRocksDBcli {
    static func main() {
        let version = String(rocksdb.GetRocksVersionAsString())
        print("RocksDB version: \(version)")
    }
}
