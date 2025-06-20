cmake_minimum_required (VERSION 3.18)
project (HDF5_TOOLS_SRC_H5JAM C)

# --------------------------------------------------------------------
# Add the h5jam executables
# --------------------------------------------------------------------
add_executable (h5jam ${HDF5_TOOLS_SRC_H5JAM_SOURCE_DIR}/h5jam.c)
target_include_directories (h5jam PRIVATE "${HDF5_TOOLS_ROOT_DIR}/lib;${HDF5_SRC_INCLUDE_DIRS};${HDF5_SRC_BINARY_DIR};$<$<BOOL:${HDF5_ENABLE_PARALLEL}>:${MPI_C_INCLUDE_DIRS}>")
if (HDF5_BUILD_STATIC_TOOLS)
  TARGET_C_PROPERTIES (h5jam STATIC)
  target_link_libraries (h5jam PRIVATE ${HDF5_TOOLS_LIB_TARGET} ${HDF5_LIB_TARGET})
else ()
  TARGET_C_PROPERTIES (h5jam SHARED)
  target_link_libraries (h5jam PRIVATE ${HDF5_TOOLS_LIBSH_TARGET} ${HDF5_LIBSH_TARGET})
endif ()
set_target_properties (h5jam PROPERTIES FOLDER tools)
set_global_variable (HDF5_UTILS_TO_EXPORT "${HDF5_UTILS_TO_EXPORT};h5jam")

add_executable (h5unjam ${HDF5_TOOLS_SRC_H5JAM_SOURCE_DIR}/h5unjam.c)
target_include_directories (h5unjam PRIVATE "${HDF5_TOOLS_ROOT_DIR}/lib;${HDF5_SRC_INCLUDE_DIRS};${HDF5_SRC_BINARY_DIR};$<$<BOOL:${HDF5_ENABLE_PARALLEL}>:${MPI_C_INCLUDE_DIRS}>")
if (HDF5_BUILD_STATIC_TOOLS)
  TARGET_C_PROPERTIES (h5unjam STATIC)
  target_link_libraries (h5unjam PRIVATE ${HDF5_TOOLS_LIB_TARGET} ${HDF5_LIB_TARGET})
else ()
  TARGET_C_PROPERTIES (h5unjam SHARED)
  target_link_libraries (h5unjam PRIVATE ${HDF5_TOOLS_LIBSH_TARGET} ${HDF5_LIBSH_TARGET})
endif ()
set_target_properties (h5unjam PROPERTIES FOLDER tools)
set_global_variable (HDF5_UTILS_TO_EXPORT "${HDF5_UTILS_TO_EXPORT};h5unjam")

set (H5_DEP_EXECUTABLES
    h5jam
    h5unjam
)

#-----------------------------------------------------------------------------
# Add Target to clang-format
#-----------------------------------------------------------------------------
if (HDF5_ENABLE_FORMATTERS)
  clang_format (HDF5_H5JAM_SRC_FORMAT h5jam)
  clang_format (HDF5_H5UNJAM_SRC_FORMAT h5unjam)
endif ()

##############################################################################
##############################################################################
###           I N S T A L L A T I O N                                      ###
##############################################################################
##############################################################################

#-----------------------------------------------------------------------------
# Rules for Installation of tools using make Install target
#-----------------------------------------------------------------------------
if (HDF5_EXPORTED_TARGETS)
  foreach (exec ${H5_DEP_EXECUTABLES})
    INSTALL_PROGRAM_PDB (${exec} ${HDF5_INSTALL_BIN_DIR} toolsapplications)
  endforeach ()

  install (
      TARGETS
          ${H5_DEP_EXECUTABLES}
      EXPORT
          ${HDF5_EXPORTED_TARGETS}
      RUNTIME DESTINATION ${HDF5_INSTALL_BIN_DIR} COMPONENT toolsapplications
  )
endif ()
