package("atlas_generator")
    set_homepage("https://github.com/Chi-EEE/AtlasGenerator")
    set_description("")
    set_license("MIT")

    add_urls("https://github.com/Chi-EEE/AtlasGenerator.git")
    add_versions("2024.06.21", "e481a0710460b68b1a73dc5f666cc84b44b15791")

    add_deps("opencv", "libnest2d")
    add_deps("supercell_core")

    on_install(function (package)
        local configs = {}
        import("package.tools.xmake").install(package, configs)
    end)
