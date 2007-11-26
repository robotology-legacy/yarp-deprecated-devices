

SET(YARPDEV_NAMES "wxsdl")
SET(YARPDEV_WRAPPERS "None")
SET(YARPDEV_INCLUDES "WxsdlWriter.h")
SET(YARPDEV_TYPES "WxsdlWriter")


IF (ENABLE_wxsdl)

FIND_PACKAGE(wxWidgets REQUIRED)
FIND_PACKAGE(SDL REQUIRED)

INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS})
ADD_DEFINITIONS(${wxWidgets_DEFINITIONS})
LINK_LIBRARIES(${wxWidgets_LIBRARIES})

INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
LINK_LIBRARIES(${SDL_LIBRARY})


ENDIF (ENABLE_wxsdl)
