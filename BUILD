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
