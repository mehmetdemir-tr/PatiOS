# Install script for directory: /home/mehmet/Masaüstü/PatiOS-3.0/fish-shell

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/home/mehmet/Masaüstü/PatiOS-3.0/aarch64-linux-musl-cross/bin/aarch64-linux-musl-objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM MESSAGE_NEVER PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/fish")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(CREATE_LINK $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/fish $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/fish_indent)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(CREATE_LINK $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/fish $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/fish_key_reader)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/etc/fish/conf.d" TYPE DIRECTORY MESSAGE_NEVER FILES "")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/etc/fish/completions" TYPE DIRECTORY MESSAGE_NEVER FILES "")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/etc/fish/functions" TYPE DIRECTORY MESSAGE_NEVER FILES "")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/etc/fish" TYPE FILE MESSAGE_NEVER FILES "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/etc/config.fish")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/fish" TYPE DIRECTORY MESSAGE_NEVER FILES "")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/fish/man/man1" TYPE DIRECTORY MESSAGE_NEVER FILES "")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/fish/vendor_completions.d" TYPE DIRECTORY MESSAGE_NEVER FILES "")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/fish/vendor_functions.d" TYPE DIRECTORY MESSAGE_NEVER FILES "")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/fish/vendor_conf.d" TYPE DIRECTORY MESSAGE_NEVER FILES "")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  
            EXECUTE_PROCESS(COMMAND mkdir -p $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/pkgconfig OUTPUT_QUIET ERROR_QUIET)
            execute_process(COMMAND chmod 755 $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/pkgconfig OUTPUT_QUIET ERROR_QUIET)
        
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" TYPE FILE MESSAGE_NEVER FILES "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/fish.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/fish/man/man1" TYPE DIRECTORY MESSAGE_NEVER FILES "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/" FILES_MATCHING REGEX "/[^/]*\\.1$" REGEX "/realpath\\.1$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/man/man1" TYPE FILE OPTIONAL MESSAGE_NEVER FILES
    "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/fish.1"
    "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/fish_indent.1"
    "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/fish_key_reader.1"
    "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/fish-doc.1"
    "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/fish-tutorial.1"
    "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/fish-language.1"
    "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/fish-interactive.1"
    "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/fish-terminal-compatibility.1"
    "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/fish-completions.1"
    "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/fish-prompt-tutorial.1"
    "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/fish-for-bash-users.1"
    "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/man/man1/fish-faq.1"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/fish" TYPE DIRECTORY OPTIONAL MESSAGE_NEVER FILES "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/cargo/fish-docs/html/")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/fish" TYPE FILE MESSAGE_NEVER FILES "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/CHANGELOG.rst")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/mehmet/Masaüstü/PatiOS-3.0/fish-shell/build-arm64/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
