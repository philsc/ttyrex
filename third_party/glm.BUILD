cc_library(
    name = "glm",
    hdrs = glob([
        "glm/**/*.h",
        "glm/**/*.hpp",
        "glm/**/*.inl",
    ]),
    deps = [
    ],
    includes = [
        "."
    ],
    linkopts = [
        "-lm",
    ],
    visibility = ["//visibility:public"],
)

[cc_test(
    name = "test__%s" % file,
    srcs = [
        file,
    ],
    deps = [
        ":glm",
    ],
    copts = [
        "-Wno-sign-compare",
        "-Wno-unused-variable",
    ],
) for file in glob([
    "test/**/*.cpp",
], exclude = [
    # Not sure what's going on with this one.
    "test/gtc/gtc_user_defined_types.cpp",
    # The readme.md says that they removed these extensions?
    "test/gtx/gtx_simd_mat4.cpp",
    "test/gtx/gtx_simd_vec4.cpp",
    # Looks like someone forgot to delete this maybe?
    "test/gtx/gtx_random.cpp",
])]
