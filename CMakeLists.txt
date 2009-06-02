CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(PROJECT_NAME "LEMON-PROJECT-TEMPLATE")
SET(PROJECT_VERSION "hg-tip"
                    CACHE STRING "LEMON PROJECT TEMPLATE version string.")

PROJECT(${PROJECT_NAME})

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

IF(EXISTS ${CMAKE_SOURCE_DIR}/lemon)
  ADD_SUBDIRECTORY(lemon)
  SET(LEMON_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/lemon
    ${CMAKE_BINARY_DIR}/lemon
  )
  SET(LEMON_LIBRARIES lemon)
ELSE()
  FIND_PACKAGE(LEMON QUIET NO_MODULE)
  FIND_PACKAGE(LEMON REQUIRED)
ENDIF()

FIND_PACKAGE(Doxygen)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LEMON_INCLUDE_DIRS}
)

ADD_EXECUTABLE(lemon-project main.cc)
TARGET_LINK_LIBRARIES(lemon-project ${LEMON_LIBRARIES})
INSTALL(
  TARGETS lemon-project
  RUNTIME DESTINATION bin
  COMPONENT bin
)

# IF(MSVC)
#   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4250 /wd4355 /wd4503 /wd4800 /wd4996")
# # Suppressed warnings:
# # C4250: 'class1' : inherits 'class2::member' via dominance
# # C4355: 'this' : used in base member initializer list
# # C4503: 'function' : decorated name length exceeded, name was truncated
# # C4800: 'type' : forcing value to bool 'true' or 'false' (performance warning)
# # C4996: 'function': was declared deprecated
# ENDIF(MSVC)

ENABLE_TESTING()

ADD_SUBDIRECTORY(doc)

######################################################################
# CPACK configuration 
######################################################################

IF(${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
  SET(CPACK_PACKAGE_NAME ${PROJECT_NAME})
  SET(CPACK_PACKAGE_VENDOR "EGRES")
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY
      "LEMON PROJECT TEMPLATE - A Template Build Environment for LEMON")
  SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

  SET(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

  SET(CPACK_PACKAGE_INSTALL_DIRECTORY
      "${PROJECT_NAME}")
  SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY
      "${PROJECT_NAME}")

  SET(CPACK_COMPONENTS_ALL
      html_documentation
      bin)

  SET(CPACK_COMPONENT_BIN_DISPLAY_NAME "Command line utilities")
  SET(CPACK_COMPONENT_HTML_DOCUMENTATION_DISPLAY_NAME "HTML documentation")

  SET(CPACK_COMPONENT_BIN_DESCRIPTION
      "Command line utilities")
  SET(CPACK_COMPONENT_HTML_DOCUMENTATION_DESCRIPTION
      "Doxygen generated documentation")

  SET(CPACK_GENERATOR "NSIS")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} ${PROJECT_NAME}")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\lemon.cs.elte.hu")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\lemon.cs.elte.hu")
  SET(CPACK_NSIS_CONTACT "lemon-user@lemon.cs.elte.hu")
  SET(CPACK_NSIS_CREATE_ICONS_EXTRA "
      CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Documentation.lnk\\\" \\\"$INSTDIR\\\\share\\\\doc\\\\index.html\\\"
      ")
  SET(CPACK_NSIS_DELETE_ICONS_EXTRA "
      !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
      Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\Documentation.lnk\\\"
      ")

  INCLUDE(CPack)
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
