###---
### include the correct boost lib and include files
###
message("\tExternal: FreeImage")
include(FindPkgMacros)

set(FREEIMAGE_INCLUDE_DIR ${extern_root}/include/)
set(FREEIMAGE_LIBRARY_DIR ${extern_root}/lib)
set(Target_Inc ${Target_Inc} ${FREEIMAGE_INCLUDE_DIR})

set(FREEIMAGE_LIBRARY_NAMES FreeImage libFreeImage)
get_debug_names(FREEIMAGE_LIBRARY_NAMES)

find_library(FREEIMAGE_LIBRARY_REL NAMES ${FREEIMAGE_LIBRARY_NAMES} HINTS ${FREEIMAGE_LIBRARY_DIR} PATH_SUFFIXES "" release Release)
find_library(FREEIMAGE_LIBRARY_DBG NAMES ${FREEIMAGE_LIBRARY_NAMES_DBG} HINTS ${FREEIMAGE_LIBRARY_DIR} PATH_SUFFIXES "" debug Debug)

make_library_set(FREEIMAGE_LIBRARY)

findpkg_finish(FREEIMAGE)

set(Target_Lib ${Target_Lib} ${FREEIMAGE_LIBRARY})
