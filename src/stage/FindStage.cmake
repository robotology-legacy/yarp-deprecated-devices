# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

########################################################################
## check pkg-config for information, if available

SET(STAGE_INCLUDE_DIR_GUESS)
SET(STAGE_LIBRARY_DIR_GUESS)
SET(STAGE_LINK_FLAGS)
IF(PKGCONFIG_EXECUTABLE)
	PKGCONFIG(stage STAGE_INCLUDE_DIR_GUESS STAGE_LIBRARY_DIR_GUESS STAGE_LINK_FLAGS STAGE_C_FLAGS)
	IF (NOT STAGE_LINK_FLAGS)
		PKGCONFIG(STAGE STAGE_INCLUDE_DIR_GUESS STAGE_LIBRARY_DIR_GUESS STAGE_LINK_FLAGS STAGE_C_FLAGS)
	ENDIF (NOT STAGE_LINK_FLAGS)
	ADD_DEFINITIONS(${STAGE_C_FLAGS})
ENDIF(PKGCONFIG_EXECUTABLE)

# not recommended to use cache
#SET(STAGE_LIBRARY "${STAGE_LINK_FLAGS}" CACHE INTERNAL "stage link flags")
#SET(STAGE_INCLUDE_DIR "${STAGE_INCLUDE_DIR_GUESS}" CACHE INTERNAL "stage include directories")

SET(STAGE_LIBRARY "${STAGE_LINK_FLAGS}")
SET(STAGE_INCLUDE_DIR "${STAGE_INCLUDE_DIR_GUESS}")

MESSAGE(STATUS "Stage library: ${STAGE_LIBRARY}")

########################################################################
## finished - now just set up flags and complain to user if necessary

IF (STAGE_LIBRARY)
	SET(STAGE_FOUND TRUE)
ELSE (STAGE_LIBRARY)
	SET(STAGE_FOUND FALSE)
ENDIF (STAGE_LIBRARY)

IF (STAGE_DEBUG_LIBRARY)
	SET(STAGE_DEBUG_FOUND TRUE)
ENDIF (STAGE_DEBUG_LIBRARY)

IF (STAGE_FOUND)
	IF (NOT Stage_FIND_QUIETLY)
		MESSAGE(STATUS "Found STAGE library: ${STAGE_LIBRARY}")
		MESSAGE(STATUS "Found STAGE include: ${STAGE_INCLUDE_DIR}")
	ENDIF (NOT Stage_FIND_QUIETLY)
ELSE (STAGE_FOUND)
	IF (Stage_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find STAGE")
	ENDIF (Stage_FIND_REQUIRED)
ENDIF (STAGE_FOUND)
