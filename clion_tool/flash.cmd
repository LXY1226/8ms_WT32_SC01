@echo off
set "PYTHON=C:/Users/Lin/.espressif/python_env/idf4.1_py3.8_env/Scripts/python.exe"
set "PYTHONPATH=C:/Users/Lin/.espressif/python_env/idf4.1_py3.8_env/Lib/site-packages"
set "IDF_PATH=C:/Users/Lin/Code/esp-idf"
set "IDF_TOOLS_JSON_PATH=%IDF_PATH%/tools/tools.json"
set "IDF_TOOLS_PY_PATH=%IDF_PATH%/tools/idf_tools.py"
set "PATH=%PATH%;C:/Users/Lin/.espressif/tools/xtensa-esp32-elf/esp-2019r2-8.2.0/xtensa-esp32-elf/bin;C:/Users/Lin/.espressif/tools/xtensa-esp32s2-elf/esp-2019r2-8.2.0/xtensa-esp32s2-elf/bin;C:/Users/Lin/.espressif/tools/esp32ulp-elf/2.28.51-esp-20191205/esp32ulp-elf-binutils/bin;C:/Users/Lin/.espressif/tools/esp32s2ulp-elf/2.28.51-esp-20191205/esp32s2ulp-elf-binutils/bin;C:/Users/Lin/.espressif/tools/cmake/3.13.4/bin;C:/Users/Lin/.espressif/tools/openocd-esp32/v0.10.0-esp32-20191114/openocd-esp32/bin;C:/Users/Lin/.espressif/tools/ninja/1.9.0/;C:/Users/Lin/.espressif/tools/idf-exe/1.0.1/;C:/Users/Lin/.espressif/tools/ccache/3.7/;C:/Users/Lin/.espressif/python_env/idf4.1_py3.8_env/Scripts;C:/Users/Lin/Code/esp-idf/tools"
set ESPBAUD=2000000
idf.py app-flash && idf.py monitor