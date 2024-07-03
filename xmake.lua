set_project("RainbowEngine")
 
set_version("0.0.1")
 
set_xmakever("2.8.8")
 
set_allowedmodes("debug", "release")
set_allowedplats("windows")
 
set_languages("c++20")
 
add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
 
set_encodings("source:utf-8")
 
add_requires("libsdl", "glm", "glfw", "vulkansdk", "tiny_obj_loader", "stb", "eigen", "openmp")
-- add_requires("glfw", {configs = {glfw_include = "vulkan"}})
 
target("RainbowEngine")
    set_kind("binary")
    add_files("src/*.cpp")
    add_headerfiles(
        "src/*.h",
        "src/extends/drawing/*.h",
        "src/extends/window/*.h"
        )
    add_headerfiles(
        "src/*.hpp",
        "src/extends/drawing/*.hpp",
        "src/extends/window/*.hpp"
        )
    add_defines("SDL_MAIN_HANDLED")
    add_packages("libsdl", "glm", "glfw", "vulkansdk", "tiny_obj_loader", "stb", "eigen", "openmp")
 
package("tiny_obj_loader")
    add_urls("https://github.com/tinyobjloader/tinyobjloader.git")
 
    on_install(function (package)
        os.cp("tiny_obj_loader.h", package:installdir("include"))
    end)
 
    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test() {
                std::string inputfile = "cornell_box.obj";
                tinyobj::attrib_t attrib;
                std::vector<tinyobj::shape_t> shapes;
                std::vector<tinyobj::material_t> materials;
 
                std::string warn;
                std::string err;
 
                bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());
            }
        ]]}, {configs = {languages = "c++11", defines = "TINYOBJLOADER_IMPLEMENTATION"}, includes = "tiny_obj_loader.h"}))
    end)