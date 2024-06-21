package("atlas_generator")
    set_homepage("https://github.com/Chi-EEE/AtlasGenerator")
    set_description("")
    set_license("MIT")

    add_urls("https://github.com/Chi-EEE/AtlasGenerator.git")
    add_versions("2024.06.21", "ffe142c03cc1602b30a846ea70eb0c2bb62381f7")

    add_deps("opencv", "libnest2d")
    add_deps("supercell_core")

    on_install(function (package)
        local configs = {}
        import("package.tools.xmake").install(package, configs)
    end)
