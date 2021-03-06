# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(stage
                    CATEGORY device
                    TYPE yarp::dev::StageControl
                    INCLUDE StageControl.h
                    DEPENDS "YARP_HAS_STAGE;YARP_COMPILE_UNMAINTAINED")

if(NOT SKIP_stage)

  include_directories(${Stage_INCLUDE_DIRS})
  yarp_add_plugin(yarp_stage StageControl.cpp
                             StageControl.h)
  target_link_libraries(yarp_stage YARP::YARP_OS
                                   YARP::YARP_dev
                                   ${Stage_LIBRARIES})
  yarp_install(TARGETS yarp_stage
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES stage.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_stage PROPERTY FOLDER "Unsupported")
endif()
