package("supercell_flash")
    set_homepage("https://github.com/Chi-EEE/SupercellFlash")
    set_description("C++ library for loading and processing Supercell 2D (.sc) assets")
    set_license("MIT")

    add_urls("https://github.com/Chi-EEE/SupercellFlash.git")
    add_versions("2024.06.21", "f824f7f84b892a14f5886515ad16ade45c034f48")

    add_deps("supercell_compression", "supercell_core")

    on_install(function (package)
        local configs = {}
        import("package.tools.xmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <SupercellFlash.h>
            void test() {
                sc::SupercellSWF swf;
            }
        ]]}, {configs = {languages = "c++17"}}))
    end)
