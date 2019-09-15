cc_library(
    name = "zero_copy",
    hdrs = [
        "zero_copy.h",
    ],
    srcs = [
        "zero_copy.cc",
    ],
    deps = [
        "@com_github_google_glog//:glog",
    ],
)

cc_test(
    name = "zero_copy_test",
    srcs = [
        "zero_copy_test.cc",
    ],
    deps = [
        ":zero_copy",
        "@com_github_google_googletest//:gtest_main",
        "@com_github_ericniebler_range-v3//:range-v3",
    ],
)

cc_binary(
    name = "ttyrex",
    srcs = [
        "ttyrex.cc",
        "Shader.h"
    ],
    deps = [
        "@com_github_google_glog//:glog",
        "@usr//:glfw3",
        "@python_glad//:c_glad",
        "@org_freetype_freetype2//:freetype2",
        "@net_gtruc_glm//:glm",
    ],
    data = [
        "7x13.pcf",
        "shaders/text.vs",
        "shaders/text.frag",
    ],
)
