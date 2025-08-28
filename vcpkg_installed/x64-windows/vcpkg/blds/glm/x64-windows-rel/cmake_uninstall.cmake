if(NOT EXISTS "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/glm/x64-windows-rel/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/glm/x64-windows-rel/install_manifest.txt")
endif()

file(READ "C:/Users/pivop/source/repos/xmanas07/basic-vulkan-3d-engine/vcpkg_installed/x64-windows/vcpkg/blds/glm/x64-windows-rel/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
  message(STATUS "Uninstalling $ENV{DESTDIR}${file}")
  if(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    exec_program(
      "C:/Users/pivop/AppData/Local/vcpkg/downloads/tools/cmake-3.30.1-windows/cmake-3.30.1-windows-i386/bin/cmake.exe" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    if(NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing $ENV{DESTDIR}${file}")
    endif()
  else(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    message(STATUS "File $ENV{DESTDIR}${file} does not exist.")
  endif()
endforeach()
