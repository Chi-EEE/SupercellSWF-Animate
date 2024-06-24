add_rules("mode.debug", "mode.releasedbg")
if is_kind("shared") then
    add_requireconfs("*", {configs = {shared = true}})
    set_runtimes("MD")
else
    add_requireconfs("*", {configs = {shared = false}})
    set_runtimes("MT")
end

add_repositories("local-repo local_repository")

add_requires("libnest2d 2022.11.16")
add_requires("wxwidgets 3.2.4")
add_requires("cdt 1.4.1")
add_requires("nlohmann_json 3.11.3")
add_requires("fmt 10.2.1")
add_requires("spdlog v1.13.0")
add_requires("opencv 4.9.0")

add_requires("supercell_flash")
add_requires("atlas_generator")

target("Plugin")
    set_kind("$(kind)")
    set_languages("cxx17")

    add_packages("supercell_flash")
    add_packages("atlas_generator", "wxwidgets", "cdt", "nlohmann_json", "fmt", "spdlog", "opencv", "openmp")

    add_files("source/**.cpp")
    add_includedirs("include")
    add_headerfiles("include/(**.h)", "include/(**.hpp)")

    set_extension(".fcm")

    if is_plat("windows") and is_kind("shared") then
        add_rules("utils.symbols.export_all", {export_classes = true})
    end

    add_syslinks("msvcrt", "user32", "gdi32", "winspool", "comdlg32", "advapi32", "shell32", "ole32", "oleaut32", "uuid", "odbc32", "odbccp32", "opengl32")

    -- SC Animate Image Debug Mode
    if has_config("SC_ANIMATE_IMAGE_DEBUG") then
        add_defines("CV_DEBUG")
    end
    if is_mode("debug") then
        set_targetdir("build/animate_bin/Debug")
    else
        set_targetdir("build_release/animate_bin/RelWithDebInfo")
    end

    if is_plat("windows") then
        add_defines("_CRT_SECURE_NO_WARNINGS", "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING")
        add_defines("_WINDOWS", "SC_MSVC")
    end

    before_build(function(target)
        local publisherName = "SupercellSWF"
        local publisherId = "org.scWorkshop.SupercellSWF.Publisher"
        local publisherUi = "org.scWorkshop.SupercellSWF.PublishSettings"
        local doctypeName = "SupercellSWF"
        local doctypeId = "org.scWorkshop.SupercellSWF"
        local MAJOR = 1
        local MINOR = 1
        local MAINTENANCE = 0

        local code = [[
#pragma once

#define PUBLISHER_NAME                      "${publisherName}"
#define PUBLISHER_UNIVERSAL_NAME            "${publisherId}"

#define PUBLISH_SETTINGS_UI_ID              "${publisherUi}"

#define DOCTYPE_NAME                        "${doctypeName}"
#define DOCTYPE_UNIVERSAL_NAME              "${doctypeId}"

#define PLUGIN_VERSION_MAJOR                ${MAJOR}
#define PLUGIN_VERSION_MINOR                ${MINOR}
#define PLUGIN_VERSION_MAINTENANCE          ${MAINTENANCE}
        ]]

        code = code:gsub("${publisherName}", publisherName)
                :gsub("${publisherId}", publisherId)
                :gsub("${publisherUi}", publisherUi)
                :gsub("${doctypeName}", doctypeName)
                :gsub("${doctypeId}", doctypeId)
                :gsub("${MAJOR}", tostring(MAJOR))
                :gsub("${MINOR}", tostring(MINOR))
                :gsub("${MAINTENANCE}", tostring(MAINTENANCE))
        
        io.writefile("include/PluginConfiguration.h", code)
    end)