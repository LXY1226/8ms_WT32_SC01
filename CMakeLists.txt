# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set( ENV{IDF_PATH} C:/Users/Lin/Code/esp-idf )
set( GIT_EXECUTABLE C:/Program Files/Git/bin/git.exe )
set( ENV{PATH} $ENV{PATH}\;C:/Users/Lin/.espressif/tools/xtensa-esp32-elf/esp-2019r2-8.2.0/xtensa-esp32-elf/bin )
set( ENV{python} C:/Users/Lin/.espressif/python_env/idf4.1_py3.8_env/Scripts/python.exe )

include(C:/Users/Lin/Code/esp-idf/tools/cmake/project.cmake)
project(esp C CXX ASM)