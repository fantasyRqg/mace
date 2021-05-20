# Tested with gcc-linaro-7.3.1-2018.05-x86_64_arm-linux-gnueabihf
# https://releases.linaro.org/components/toolchain/binaries/7.3-2018.05/

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER "/root/cross-pi-gcc-8.3.0-1/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "/root/cross-pi-gcc-8.3.0-1/bin/arm-linux-gnueabihf-g++")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_CXX_FLAGS "-march=armv7-a -mfloat-abi=hard -mfpu=neon-vfpv4 ${CMAKE_CXX_FLAGS}")
