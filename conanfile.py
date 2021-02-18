from conans import ConanFile, CMake, tools

class SaikoEngine(ConanFile):
    name = 'SaikoEngine'
    version = '0.1'
    # license = None
    url = 'https://github.com/SaikoSoft/SaikoEngine'
    # description = None
    settings = 'os', 'compiler', 'build_type', 'arch'
    options = {'shared': [True, False]}
    default_options = {'shared': False}
    generators = 'cmake'
    requires = (
        'boost/1.74.0',
        'catch2/2.13.3',
        'fmt/7.1.2',
        'nlohmann_json/3.9.1',
        'spdlog/1.8.1',
    )

    def source(self):
        self.run('git clone https://github.com/SaikoSoft/SaikoEngine.git')

    def build(self):
        generator = 'Ninja' if tools.os_info.is_linux else 'Visual Studio 16 2019'
        cmake = CMake(self, generator=generator)
        if self.should_configure:
            cmake.configure()
        if self.should_build:
            cmake.build()
        if self.should_test:
            cmake.test(output_on_failure=True)
