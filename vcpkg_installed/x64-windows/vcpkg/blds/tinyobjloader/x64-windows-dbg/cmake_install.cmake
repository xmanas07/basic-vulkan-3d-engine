# Install script for directory: C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/tinyobjloader/src/v2.0.0rc13-75ee586e9b.clean

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/pkgs/tinyobjloader_x64-windows/debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "OFF")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/tinyobjloader/x64-windows-dbg/tinyobjloader.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/tinyobjloader/x64-windows-dbg/CMakeFiles/tinyobjloader.dir/install-cxx-module-bmi-Debug.cmake" OPTIONAL)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/tinyobjloader/cmake/tinyobjloader-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/tinyobjloader/cmake/tinyobjloader-targets.cmake"
         "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/tinyobjloader/x64-windows-dbg/CMakeFiles/Export/ed38dc2cb1e14c44cc057c68af26fb32/tinyobjloader-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/tinyobjloader/cmake/tinyobjloader-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/tinyobjloader/cmake/tinyobjloader-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/tinyobjloader/cmake" TYPE FILE FILES "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/tinyobjloader/x64-windows-dbg/CMakeFiles/Export/ed38dc2cb1e14c44cc057c68af26fb32/tinyobjloader-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/tinyobjloader/cmake" TYPE FILE FILES "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/tinyobjloader/x64-windows-dbg/CMakeFiles/Export/ed38dc2cb1e14c44cc057c68af26fb32/tinyobjloader-targets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/tinyobjloader/src/v2.0.0rc13-75ee586e9b.clean/tiny_obj_loader.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/tinyobjloader" TYPE FILE FILES "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/tinyobjloader/src/v2.0.0rc13-75ee586e9b.clean/LICENSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/tinyobjloader/cmake" TYPE FILE FILES
    "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/tinyobjloader/x64-windows-dbg/tinyobjloader-config.cmake"
    "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/tinyobjloader/x64-windows-dbg/tinyobjloader-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/tinyobjloader/x64-windows-dbg/tinyobjloader.pc")
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
  string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
  file(WRITE "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/tinyobjloader/x64-windows-dbg/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
