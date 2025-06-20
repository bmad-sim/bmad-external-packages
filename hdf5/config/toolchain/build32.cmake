if (WIN32)
  set (CMAKE_SYSTEM_NAME Windows)
  set (CMAKE_GENERATOR_PLATFORM "x86")
elseif(APPLE)
  set (CMAKE_OSX_ARCHITECTURES "i386")
elseif(MINGW)
  set (CMAKE_SYSTEM_NAME Windows)
  set (TOOLCHAIN_PREFIX i686-w64-mingw32)
  set (CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
  set (CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
  set (CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)
  set (CMAKE_Fortran_COMPILER ${TOOLCHAIN_PREFIX}-gfortran)

  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32" CACHE STRING "c++ flags")
  set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32" CACHE STRING "c flags")

  set (LIB32 /usr/lib) # Fedora

  if (EXISTS "/usr/lib32")
    set (LIB32 /usr/lib32) # Arch, Solus
  endif ()

  set (CMAKE_SYSTEM_LIBRARY_PATH ${LIB32} CACHE STRING "system library search path" FORCE)
  set (CMAKE_LIBRARY_PATH        ${LIB32} CACHE STRING "library search path"        FORCE)

  # this is probably unlikely to be needed, but just in case
  set (CMAKE_EXE_LINKER_FLAGS    "-m32 -L${LIB32}" CACHE STRING "executable linker flags"     FORCE)
  set (CMAKE_SHARED_LINKER_FLAGS "-m32 -L${LIB32}" CACHE STRING "shared library linker flags" FORCE)
  set (CMAKE_MODULE_LINKER_FLAGS "-m32 -L${LIB32}" CACHE STRING "module linker flags"         FORCE)

  # on Fedora and Arch and similar, point pkgconfig at 32 bit .pc files. We have
  # to include the regular system .pc files as well (at the end), because some
  # are not always present in the 32 bit directory
  if (EXISTS "${LIB32}/pkgconfig")
    set (ENV{PKG_CONFIG_LIBDIR} ${LIB32}/pkgconfig:/usr/share/pkgconfig:/usr/lib/pkgconfig:/usr/lib64/pkgconfig)
  endif ()

  set (CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})
  set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
  set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
  set (CMAKE_CROSSCOMPILING_EMULATOR wine)

  include_directories(/usr/${TOOLCHAIN_PREFIX}/include)
  set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS On CACHE BOOL "Export windows symbols")
else ()
  set (CMAKE_SYSTEM_NAME Linux)

  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32" CACHE STRING "c++ flags")
  set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32" CACHE STRING "c flags")

  set (LIB32 /usr/lib) # Fedora

  if (EXISTS "/usr/lib32")
    set (LIB32 /usr/lib32) # Arch, Solus
  endif ()

  set (CMAKE_SYSTEM_LIBRARY_PATH ${LIB32} CACHE STRING "system library search path" FORCE)
  set (CMAKE_LIBRARY_PATH        ${LIB32} CACHE STRING "library search path"        FORCE)

  # this is probably unlikely to be needed, but just in case
  set (CMAKE_EXE_LINKER_FLAGS    "-m32 -L${LIB32}" CACHE STRING "executable linker flags"     FORCE)
  set (CMAKE_SHARED_LINKER_FLAGS "-m32 -L${LIB32}" CACHE STRING "shared library linker flags" FORCE)
  set (CMAKE_MODULE_LINKER_FLAGS "-m32 -L${LIB32}" CACHE STRING "module linker flags"         FORCE)

  # on Fedora and Arch and similar, point pkgconfig at 32 bit .pc files. We have
  # to include the regular system .pc files as well (at the end), because some
  # are not always present in the 32 bit directory
  if (EXISTS "${LIB32}/pkgconfig")
    set (ENV{PKG_CONFIG_LIBDIR} ${LIB32}/pkgconfig:/usr/share/pkgconfig:/usr/lib/pkgconfig:/usr/lib64/pkgconfig)
  endif ()
# where is the target environment
  set (CMAKE_FIND_ROOT_PATH ${LIB32})
# search for programs in the build host directories
  set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
  set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif ()
