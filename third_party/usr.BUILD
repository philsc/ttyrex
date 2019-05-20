cc_library(
    name = "glfw3",
    hdrs = [
        "include/GLFW/glfw3.h",
        "include/GLFW/glfw3native.h",
    ],
    deps = [
        ":gl",
        ":glfw3_import",
    ],
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"],
)

cc_import(
    name = "glfw3_import",
    interface_library = "lib/x86_64-linux-gnu/libglfw.so",
    system_provided = 1,
)

cc_library(
    name = "gl",
    hdrs = [
        "include/GL/gl.h",
    ],
    deps = [
    ],
)
