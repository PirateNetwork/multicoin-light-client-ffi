// swift-tools-version:5.5

import PackageDescription

let package = Package(
    name: "libpiratelc",
    products: [
        .library(
            name: "libpiratelc",
            targets: ["libpiratelc"]
        ),
    ],
    dependencies: [

    ],
    targets: [
        .binaryTarget(
            name: "libpiratelc",
            path: "releases/XCFramework/libpiratelc.xcframework"
        )
    ]
)
