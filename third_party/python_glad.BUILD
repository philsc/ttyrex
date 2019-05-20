py_binary(
    name = "python_glad",
    srcs = glob([
        "glad/**/*.py",
    ]),
    data = glob([
        "glad/**/*.xml",
        "glad/**/*.h",
    ]),
    main = "glad/__main__.py",
)

genrule(
    name = "generate_files",
    tools = [
        ":python_glad",
    ],
    outs = [
        "output/include/KHR/khrplatform.h",
        "output/include/glad/glad.h",
        "output/src/glad.c",
    ],
    cmd = "$(location :python_glad) --reproducible --generator c --no-loader --out-path $(@D)/output 2>/dev/null",
)

cc_library(
    name = "c_glad",
    hdrs = [
        "output/include/KHR/khrplatform.h",
        "output/include/glad/glad.h",
    ],
    srcs = [
        "output/src/glad.c",
    ],
    includes = [
        "output/include",
    ],
    visibility = ["//visibility:public"],
)
