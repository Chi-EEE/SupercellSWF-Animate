package("supercell_compression")
    set_homepage("https://github.com/Chi-EEE/SupercellCompression")
    set_description("C++ library for compressing and decompressing Supercell assets")
    set_license("MIT")

    add_urls("https://github.com/Chi-EEE/SupercellCompression.git")
    add_versions("2024.09.18", "6c670d6515bc95e51e56c18e16ca37f01f903976")
    add_versions("2024.06.21", "9bea65e9308f3694788001e59209325a1cec7982")

    add_deps("lzham_codec", "lzma", "zstd")
    add_deps("astc-encoder", {configs = {sse41 = true, native = true, cli = false}})
    add_deps("supercell_core")

    on_install(function (package)
        local configs = {}
        import("package.tools.xmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <SupercellCompression.h>
            void test() {
                sc::Decompressor::Lzham::Props props;
                sc::Decompressor::Lzham context(props);
            }
        ]]}, {configs = {languages = "c++17"}}))
    end)
