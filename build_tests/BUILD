cc_test(
    name = "glfw_run_test",
    srcs = [
        "glfw_run_test.cc",
    ],
    deps = [
        "@usr//:glfw3",
        "@python_glad//:c_glad",
    ],
)

cc_binary(
    name = "glfw_build_test",
    srcs = [
        "glfw_build_test.cc",
    ],
    deps = [
        "@usr//:glfw3",
        "@python_glad//:c_glad",
    ],
)

cc_test(
    name = "glog_test",
    srcs = ["glog_test.cc"],
    deps = [
        "@com_github_google_glog//:glog",
    ],
)

# Taken from https://www.freetype.org/freetype2/docs/tutorial/step1.html
cc_test(
    name = "freetype2_example1_test",
    srcs = [
        "freetype2_example1_test.c",
    ],
    deps = [
        "@org_freetype_freetype2//:freetype2",
    ],
    args = [
        "/usr/share/fonts/opentype/linux-libertine/LinLibertine_DR.otf",
        "HelloWorld",
    ],
)

cc_test(
    name = "googletest_test",
    srcs = [
        "googletest_test.cc",
    ],
    deps = [
        "@com_github_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "range_v3_test",
    srcs = [
        "range_v3_test.cc",
    ],
    deps = [
        "@com_github_ericniebler_range-v3//:range-v3",
    ],
)
