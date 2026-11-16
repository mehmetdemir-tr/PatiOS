set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(TOOLCHAIN "/home/mehmet/Masaüstü/PatiOS-3.0/aarch64-linux-musl-cross/bin")

set(CMAKE_C_COMPILER   "${TOOLCHAIN}/aarch64-linux-musl-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN}/aarch64-linux-musl-g++")
set(CMAKE_AR           "${TOOLCHAIN}/aarch64-linux-musl-ar")
set(CMAKE_RANLIB       "${TOOLCHAIN}/aarch64-linux-musl-ranlib")
set(CMAKE_STRIP        "${TOOLCHAIN}/aarch64-linux-musl-strip")

set(CMAKE_FIND_ROOT_PATH
    "/home/mehmet/Masaüstü/PatiOS-3.0/aarch64-linux-musl-cross/aarch64-linux-musl"
)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
