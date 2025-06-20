cmake_minimum_required (VERSION 3.18)
project (HDF5_JAVA_JNI C)

set (HDF5_JAVA_JNI_CSRCS
    ${HDF5_JAVA_JNI_SOURCE_DIR}/exceptionImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5aImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5Constants.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5dImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5eImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5fImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5gImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5iImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5Imp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5lImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5oImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pACPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pDAPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pDCPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pDXPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pFAPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pFCPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pGAPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pGCPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pLAPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pLCPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pOCPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pOCpyPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pStrCPLImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5plImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5rImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5sImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5tImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5util.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5vlImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5zImp.c
    ${HDF5_JAVA_JNI_SOURCE_DIR}/nativeData.c
)

set (HDF5_JAVA_JNI_CHDRS
    ${HDF5_JAVA_JNI_SOURCE_DIR}/exceptionImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5aImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5dImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5eImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5fImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5gImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5iImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5Imp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5jni.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5lImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5oImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pACPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pDAPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pDCPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pDXPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pFAPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pFCPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pGAPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pGCPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pLAPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pLCPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pOCPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pOCpyPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5pStrCPLImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5plImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5rImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5sImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5tImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5util.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5vlImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/h5zImp.h
    ${HDF5_JAVA_JNI_SOURCE_DIR}/nativeData.h
)

set (CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

########### JNI libraries always must be built shared  ###############
add_library (${HDF5_JAVA_JNI_LIB_TARGET} SHARED ${HDF5_JAVA_JNI_CSRCS} ${HDF5_JAVA_JNI_CHDRS})
target_include_directories (${HDF5_JAVA_JNI_LIB_TARGET}
    PRIVATE "${HDF5_SRC_INCLUDE_DIRS};${HDF5_SRC_BINARY_DIR};${HDF5_JAVA_JNI_SOURCE_DIR};$<$<BOOL:${HDF5_ENABLE_PARALLEL}>:${MPI_C_INCLUDE_DIRS}>"
)
target_compile_options(${HDF5_JAVA_JNI_LIB_TARGET} PRIVATE "${HDF5_CMAKE_C_FLAGS}")
TARGET_C_PROPERTIES (${HDF5_JAVA_JNI_LIB_TARGET} SHARED)
target_link_libraries (${HDF5_JAVA_JNI_LIB_TARGET} PUBLIC ${HDF5_LIBSH_TARGET})
set_target_properties (${HDF5_JAVA_JNI_LIB_TARGET} PROPERTIES FOLDER libraries/jni)
SET_GLOBAL_VARIABLE (HDF5_LIBRARIES_TO_EXPORT "${HDF5_LIBRARIES_TO_EXPORT};${HDF5_JAVA_JNI_LIB_TARGET}")
H5_SET_LIB_OPTIONS (${HDF5_JAVA_JNI_LIB_TARGET} ${HDF5_JAVA_JNI_LIB_NAME} SHARED "JAVA")

#-----------------------------------------------------------------------------
# Add Target to clang-format
#-----------------------------------------------------------------------------
if (HDF5_ENABLE_FORMATTERS)
  clang_format (HDF5_JNI_SRC_FORMAT ${HDF5_JAVA_JNI_LIB_TARGET})
endif ()

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (HDF5_EXPORTED_TARGETS)
  INSTALL_TARGET_PDB (${HDF5_JAVA_JNI_LIB_TARGET} ${HDF5_INSTALL_BIN_DIR} libraries)
  install (
      TARGETS
          ${HDF5_JAVA_JNI_LIB_TARGET}
      EXPORT
          ${HDF5_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${HDF5_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${HDF5_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${HDF5_INSTALL_BIN_DIR} COMPONENT libraries
      FRAMEWORK DESTINATION ${HDF5_INSTALL_FWRK_DIR} COMPONENT libraries
      INCLUDES DESTINATION include
  )
endif ()
