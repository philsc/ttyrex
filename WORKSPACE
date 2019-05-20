load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")


# Built with tools/cpp/build-libc.sh.
http_archive(
    name = "glibc",
    url = "http://philsc.net/Build-Dependencies/glibc-2.28-built.tar.xz",
    sha256 = "df0d063899f2f78d5165d44a2c41ba504e6696796b385f549139c68f9709760a",
    build_file = "@//tools/cpp:glibc.BUILD",
)

# Built with tools/cpp/build-kernel-headers.sh.
http_archive(
    name = "linux-headers",
    url = "http://philsc.net/Build-Dependencies/linux-headers-3.16.62.tar.xz",
    sha256 = "161606d41559d0f03b27c415c3e06bc9294924740ea61a36ff6c1d8e8d72bff5",
    build_file = "@//tools/cpp:linux-headers.BUILD",
)

http_archive(
    name = "clang_8_toolchain",
    url = "http://releases.llvm.org/8.0.0/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz",
    sha256 = "87b88d620284d1f0573923e6f7cc89edccf11d19ebaec1cfb83b4f09ac5db09c",
    build_file = "@//tools/cpp/clang:clang.BUILD",
    strip_prefix = "clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04",
)

new_local_repository(
    name = "usr",
    path = "/usr",
    build_file = "@//third_party:usr.BUILD",
)

load("//third_party:repositories.bzl", "ttyrex_repositories")
ttyrex_repositories()
