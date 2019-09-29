load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

def ttyrex_repositories():
    _maybe(
        http_archive,
        name = "org_freetype_freetype2",
        build_file = "@//third_party:freetype2.BUILD",
        sha256 = "33a28fabac471891d0523033e99c0005b95e5618dc8ffa7fa47f9dadcacb1c9b",
        strip_prefix = "freetype-2.8",
        urls = [
            "https://mirror.bazel.build/download.savannah.gnu.org/releases/freetype/freetype-2.8.tar.gz",
            "http://download.savannah.gnu.org/releases/freetype/freetype-2.8.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "org_libpng_libpng",
        build_file = "@//third_party:libpng.BUILD",
        sha256 = "7f415186d38ca71c23058386d7cf5135c8beda821ee1beecdc2a7a26c0356615",
        strip_prefix = "libpng-1.2.57",
        urls = [
            "https://mirror.bazel.build/github.com/glennrp/libpng/archive/v1.2.57.tar.gz",
            "https://github.com/glennrp/libpng/archive/v1.2.57.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "net_zlib_zlib",
        sha256 = "6d4d6640ca3121620995ee255945161821218752b551a1a180f4215f7d124d45",
        build_file = "@//third_party:zlib.BUILD",
        strip_prefix = "zlib-cacf7f1d4e3d44d871b605da3b647f07d718623f",
        urls = [
            "https://mirror.bazel.build/github.com/madler/zlib/archive/cacf7f1d4e3d44d871b605da3b647f07d718623f.tar.gz",
            "https://github.com/madler/zlib/archive/cacf7f1d4e3d44d871b605da3b647f07d718623f.tar.gz",
        ],
    )

    new_git_repository(
        name = "python_glad",
        commit = "6d1f1fe1895832b8957d58f1e8f6df8865c56f94",
        shallow_since = "1549191278 +0100",
        remote = "https://github.com/Dav1dde/glad.git",
        build_file = "@//third_party:python_glad.BUILD",
    )

    new_git_repository(
        name = "net_gtruc_glm",
        commit = "7c07544b34c2f8655e4134239137d32aa2ccd5c8",
        shallow_since = "1568317371 +0200",
        remote = "https://github.com/g-truc/glm.git",
        build_file = "@//third_party:glm.BUILD",
    )

    git_repository(
        name = "com_github_google_glog",
        remote = "https://github.com/google/glog.git",
        commit = "ba8a9f6952d04d1403b97df24e6836227751454e",
        shallow_since = "1557212526 +0900",
        patches = [
            "@//third_party:glog.patch",
        ],
        patch_args = ["-p1"],
    )

    http_archive(
        name = "com_github_gflags_gflags",
        strip_prefix = "gflags-2.2.2",
        sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
        urls = [
            "https://mirror.bazel.build/github.com/gflags/gflags/archive/v2.2.2.tar.gz",
            "https://github.com/gflags/gflags/archive/v2.2.2.tar.gz",
        ],
    )

    git_repository(
        name = "com_github_google_googletest",
        remote = "https://github.com/google/googletest.git",
        commit = "8b6d3f9c4a774bef3081195d422993323b6bb2e0",
        shallow_since = "1551793181 -0500",
        patches = [
            "@//third_party:gtest.patch",
        ],
        patch_args = ["-p1"],
    )

    git_repository(
        name = "com_github_ericniebler_range-v3",
        remote = "https://github.com/ericniebler/range-v3.git",
        commit = "67d564568359b1e09f92ee0f2a0c1f36dfd59671",
        shallow_since = "1569527231 -0700",
    )

    new_git_repository(
        name = "utfcpp",
        remote = "https://github.com/nemtrif/utfcpp.git",
        commit = "170e2d11f516539d18bf39f552fa204dbf41d9ce",
        shallow_since = "1563040376 -0400",
        build_file = "@//third_party:utfcpp.BUILD",
    )

def _maybe(repo_rule, name, **kwargs):
    if name not in native.existing_rules():
        repo_rule(name = name, **kwargs)
