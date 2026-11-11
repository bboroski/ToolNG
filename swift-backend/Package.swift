// swift-tools-version: 6.0
import PackageDescription

let package = Package(
    name: "ToolNGBackend",
    platforms: [.macOS(.v13)],
    products: [
        .library(name: "ToolNGBackendCore", targets: ["ToolNGBackendCore"]),
        .executable(name: "toolng-server", targets: ["ToolNGBackendServer"])
    ],
    dependencies: [
        .package(url: "https://github.com/vapor/vapor.git", from: "4.110.0")
    ],
    targets: [
        .target(name: "ToolNGBackendCore", dependencies: [.product(name: "Vapor", package: "vapor")]),
        .executableTarget(name: "ToolNGBackendServer", dependencies: ["ToolNGBackendCore", .product(name: "Vapor", package: "vapor")]),
        .testTarget(name: "ToolNGBackendCoreTests", dependencies: ["ToolNGBackendCore"])
    ],
    swiftLanguageModes: [.v6]
)
