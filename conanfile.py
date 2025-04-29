from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class pkgRecipe(ConanFile):
    name = "source2gen"
    version = "0.0.0"
    package_type = "application"

    requires = [
        "abseil/20240722.0",
        "argparse/3.2",
        "gtest/1.15.0",
    ]

    options = {
      "game": [
          "CS2",
          "DOTA2",
          "SBOX",
          "ARTIFACT2",
          "ARTIFACT1",
          "UNDERLORDS",
          "DESKJOB",
          "HL_ALYX",
          "THE_LAB_ROBOT_REPAIR",
          "DEADLOCK",
      ],
    }
    default_options = {
      "game": "CS2",
    }

    settings = "os", "compiler", "build_type", "arch"

    exports_sources = "CMakeLists.txt", "source2gen/*"

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        variables = {
          "SOURCE2GEN_GAME": self.options.game,
        }

        cmake = CMake(self)
        cmake.configure(variables=variables)
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
