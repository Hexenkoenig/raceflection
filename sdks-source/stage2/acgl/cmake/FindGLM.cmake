# - Find GLM
# Find the native GLM headers
#
#  GLM_INCLUDE_DIR -  where to find glm.hpp, etc.
#  GLM_FOUND        - True if GLM found.

IF(GLM_FOUND)
  # Already in cache, be silent
  SET(GLM_FIND_QUIETLY TRUE)
ENDIF()

IF(BUILD_GLM OR ACGL_GLOBAL_EXTERN_DIR_FOUND_FIRST_TIME OR BUILD_TYPE_CHANGED)
    UNSET(GLM_INCLUDE_DIR CACHE)
ENDIF()

# Look for the header file.
IF(BUILD_GLM)
    SET(GLM_INCLUDE_DIR "${ACGL_LOCAL_EXTERN_DIR}/glm")
ELSEIF(ACGL_GLOBAL_EXTERN_DIR)
    FIND_PATH(GLM_INCLUDE_DIR NAMES glm/glm.hpp PATHS "${ACGL_GLOBAL_EXTERN_DIR}/glm" NO_DEFAULT_PATH)
ENDIF()
IF(NOT GLM_INCLUDE_DIR)
    FIND_PATH(GLM_INCLUDE_DIR NAMES glm/glm.hpp)
ENDIF()

# Copy the results to the output variables.
IF(GLM_INCLUDE_DIR)
    SET(GLM_FOUND TRUE CACHE INTERNAL "")
ELSE()
    SET(GLM_FOUND FALSE CACHE INTERNAL "")
ENDIF()

# Report the results.
IF(NOT GLM_FOUND)
    SET(GLM_MESSAGE "GLM was not found. Make sure GLM_INCLUDE_DIR is set to the directories containing the include files for GLM.")
    IF(GLM_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "${GLM_MESSAGE}")
    ELSEIF(NOT GLM_FIND_QUIETLY)
        MESSAGE(STATUS "${GLM_MESSAGE}")
    ENDIF()
ELSEIF(NOT GLM_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for GLM - found")
ENDIF()

MESSAGE(STATUS "GLM_INCLUDE_DIR:${GLM_INCLUDE_DIR}")

