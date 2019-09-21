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

cc_library(
    name = "process",
    hdrs = [
        "process.h",
    ],
    srcs = [
        "process.cc",
    ],
    deps = [
        "@com_github_google_glog//:glog",
    ],
)

cc_test(
    name = "process_test",
    srcs = [
        "process_test.cc",
    ],
    deps = [
        ":process",
        "@com_github_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "pts",
    hdrs = [
        "pts.h",
    ],
    srcs = [
        "pts.cc",
    ],
    deps = [
        "@com_github_google_glog//:glog",
    ],
    linkopts = [
        "-lutil",
    ],
)

cc_binary(
    name = "pts_test",
    srcs = [
        "pts_test.cc",
    ],
    deps = [
        ":pts",
        "@com_github_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "epoll",
    hdrs = [
        "epoll.h",
    ],
    srcs = [
        "epoll.cc",
    ],
    deps = [
        "@com_github_google_glog//:glog",
    ],
    linkopts = [
        "-lm",
    ],
)

cc_test(
    name = "epoll_test",
    srcs = [
        "epoll_test.cc",
    ],
    deps = [
        ":epoll",
        ":process",
        "@com_github_google_googletest//:gtest_main",
    ],
)
