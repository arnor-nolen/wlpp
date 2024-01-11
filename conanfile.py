import os
from conan import ConanFile
from conan.tools.files import replace_in_file
from conan.tools.cmake import cmake_layout, CMake, CMakeDeps, CMakeToolchain
from conan.tools.microsoft import is_msvc
from conan.tools.microsoft.toolchain import msvs_toolset


class CppTemplate(ConanFile):
    name = "CppTemplate"
    version = "1.0"
    package_type = "application"

    license = "MIT"
    author = ""
    url = ""
    description = "C++ template."
    topics = ("C++", "Conan", "Template")

    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "src/*"

    default_options = {"fmt/*:shared": True}

    def requirements(self):
        self.requires("fmt/10.1.1")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)
        tc.user_presets_path = "ConanPresets.json"
        vars = {}
        if self.settings.build_type == "Debug":
            vars["CMAKE_EXPORT_COMPILE_COMMANDS"] = "ON"
        if tc.generator == "Ninja":
            vars["CMAKE_COLOR_DIAGNOSTICS"] = "ON"
        tc.cache_variables = vars
        tc.generate()

        # Conan for some reason doesn't include arch and toolset info in presets.
        if is_msvc(self):
            arch = {
                "x86": "x86",
                "x86_64": "x64",
                "armv7": "ARM",
                "armv8": "ARM64",
            }.get(self.settings.get_safe("arch"))

            toolset = msvs_toolset(self)

            preset_path = os.path.join(self.generators_folder, "CMakePresets.json")
            replace_in_file(
                self,
                file_path=preset_path,
                search='"name": "conan-default",',
                replace=f'"name": "conan-default",\n            "architecture": "{arch}",\n            "toolset": "{toolset}",',
            )

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
