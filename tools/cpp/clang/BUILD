# TODO(phil): Only include what we actually need here.
filegroup(
    name = "all",
    srcs = [
        "llvm-ar",
        "llvm-ld",
        "clang-cpp",
        "clang",
        "llvm-nm",
        "llvm-objcopy",
        "llvm-objdump",
        "llvm-strip",
        "@clang_8_toolchain//:all",
        "@glibc//:glibc",
        "@linux-headers//:linux-headers",
    ],
    visibility = ["//tools/cpp:__subpackages__"],
)
