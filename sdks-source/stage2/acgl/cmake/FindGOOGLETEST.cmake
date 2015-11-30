# Find the native googletest headers
#
#  GOOGLETEST_INCLUDE_DIR  - where to find googletestenc.h, etc.
#  GOOGLETEST_LIBRARIES    - googletest library
#  GOOGLETEST_FOUND        - True if googletest found.

IF(GOOGLETEST_FOUND)
    # Already in cache, be silent
    SET(GOOGLETEST_FIND_QUIETLY TRUE)
ENDIF()

IF(BUILD_GOOGLETEST OR ACGL_GLOBAL_EXTERN_DIR_FOUND_FIRST_TIME OR BUILD_TYPE_CHANGED)
    UNSET(GOOGLETEST_INCLUDE_DIR CACHE)
    UNSET(GOOGLETEST_LIBRARIES CACHE)
ENDIF()

IF(BUILD_GOOGLETEST)
    SET(GOOGLETEST_INCLUDE_DIR "${ACGL_LOCAL_EXTERN_DIR}/googletest/include")
    SET(GOOGLETEST_LIBRARIES GoogleTest${COMPILE_POSTFIX})
ELSE()
    # Look for the header file.
    IF(ACGL_GLOBAL_EXTERN_DIR)
        FIND_PATH(GOOGLETEST_INCLUDE_DIR NAMES gtest/gtest.h PATHS "${ACGL_GLOBAL_EXTERN_DIR}/googletest/include" NO_DEFAULT_PATH)
    ENDIF()
    IF(NOT GOOGLETEST_INCLUDE_DIR)
        FIND_PATH(GOOGLETEST_INCLUDE_DIR NAMES gtest/gtest.h)
    ENDIF()

    # Look for the library file.
    IF(ACGL_GLOBAL_EXTERN_DIR)
        FIND_LIBRARY(GOOGLETEST_LIBRARIES NAMES GoogleTest${COMPILE_POSTFIX} PATHS "${ACGL_GLOBAL_EXTERN_DIR}/googletest/lib" NO_DEFAULT_PATH)
    ENDIF()
    IF(NOT GOOGLETEST_LIBRARIES)
        FIND_LIBRARY(GOOGLETEST_LIBRARIES NAMES GoogleTest${COMPILE_POSTFIX})
    ENDIF()
ENDIF()

# Copy the results to the output variables.
IF(GOOGLETEST_INCLUDE_DIR AND GOOGLETEST_LIBRARIES)
    SET(GOOGLETEST_FOUND TRUE CACHE INTERNAL "")
ELSE()
    SET(GOOGLETEST_FOUND FALSE CACHE INTERNAL "")
ENDIF()

# Report the results.
IF(NOT GOOGLETEST_FOUND)
    SET(GOOGLETEST_MESSAGE "googletest was not found. Make sure GOOGLETEST_INCLUDE_DIR AND GOOGLETEST_LIBRARIES are set correctly.")
    IF(GOOGLETEST_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "${GOOGLETEST_MESSAGE}")
    ELSEIF(NOT GOOGLETEST_FIND_QUIETLY)
        MESSAGE(STATUS "${GOOGLETEST_MESSAGE}")
    ENDIF()
ELSEIF(NOT GOOGLETEST_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for googletest - found")
ENDIF()

MESSAGE(STATUS "GOOGLETEST_INCLUDE_DIR:${GOOGLETEST_INCLUDE_DIR}")
MESSAGE(STATUS "GOOGLETEST_LIBRARIES:${GOOGLETEST_LIBRARIES}")

