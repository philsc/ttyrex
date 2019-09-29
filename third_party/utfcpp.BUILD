cc_library(
    name = "utfcpp",
    hdrs = glob([
        "source/**/*.h",
    ]),
    includes = [
        "source",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "negative_test",
    srcs = [
        "tests/negative.cpp",
    ],
    deps = [
        ":utfcpp",
    ],
    args = [
        "$(location tests/test_data/utf8_invalid.txt)",
    ],
    data = [
        "tests/test_data/utf8_invalid.txt",
    ],
)

[cc_test(
    name = "test__%s" % file,
    srcs = [
        file,
    ],
    deps = [
        ":utfcpp",
        "@com_github_google_googletest//:gtest_main",
    ],
) for file in glob([
    "tests/test_*.cpp",
])]
