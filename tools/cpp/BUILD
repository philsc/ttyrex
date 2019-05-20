filegroup(
    name = "empty",
)

cc_toolchain_suite(
    name = "toolchain",
    toolchains = {
        "k8": "cc-compiler-k8-clang",
        "k8|gcc": "cc-compiler-k8-gcc",
        "k8|clang": "cc-compiler-k8-clang",
    },
    visibility = ["//visibility:public"],
)

cc_toolchain(
    name = "cc-compiler-k8-gcc",
    all_files = ":empty",
    compiler_files = ":empty",
    cpu = "k8",
    dwp_files = ":empty",
    dynamic_runtime_lib = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    static_runtime_lib = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
    visibility = ["//visibility:public"],
)

cc_toolchain(
    name = "cc-compiler-k8-clang",
    toolchain_identifier = "clang8",
    all_files = "//tools/cpp/clang:all",
    compiler_files = "//tools/cpp/clang:all",
    cpu = "k8",
    dwp_files = ":empty",
    dynamic_runtime_lib = ":empty",
    linker_files = "//tools/cpp/clang:all",
    objcopy_files = "//tools/cpp/clang:all",
    static_runtime_lib = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
    visibility = ["//visibility:public"],
)
