# ~~~
# Summary:      Local, non-generic plugin setup
# Copyright (c) 2020-2021 Mike Rossiter
# License:      GPLv3+
# ~~~

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.


# -------- Options ----------

set(OCPN_TEST_REPO
    "opencpn/logbookkonni-alpha"
    CACHE STRING "Default repository for untagged builds"
)
set(OCPN_BETA_REPO
    "opencpn/logbookkonni-beta"
    CACHE STRING 
    "Default repository for tagged builds matching 'beta'"
)
set(OCPN_RELEASE_REPO
    "opencpn/logbookkonni-prod"
    CACHE STRING 
    "Default repository for tagged builds not matching 'beta'"
)

option(PLUGIN_USE_SVG "Use SVG graphics" ON)

#
#
# -------  Plugin setup --------
#  Using Shipdriver version 3.3.0 from branch v3.0

set(PKG_NAME LogbookKonni_pi)
set(PKG_VERSION  1.4.37.0)
set(PKG_PRERELEASE "")  # Empty, or a tag like 'beta'

set(DISPLAY_NAME Logbook)    # Dialogs, installer artifacts, ...
set(PLUGIN_API_NAME Logbook) # As of GetCommonName() in plugin API
set(PKG_SUMMARY "Complex, feature-rich logbook")
set(PKG_DESCRIPTION [=[ Marine Logbook
with flexible and customizable layout. Help on YouTube,
search LogbookKonni
]=])

set(PKG_AUTHOR "Del Elson")
set(PKG_IS_OPEN_SOURCE "yes")
set(PKG_HOMEPAGE_URL https://github.com/rgleason/LogbookKonni_pi)
set(PKG_INFO_URL https://opencpn.org/OpenCPN/plugins/logbookkonni.html)


SET(SRC
  src/logbook_pi.h
  src/logbook_pi.cpp
  src/Options.h
  src/Options.cpp
  src/icons.h
  src/icons.cpp
  src/Logbook.h
  src/Logbook.cpp
  src/LogbookDialog.h
  src/LogbookDialog.cpp
  src/LogbookOptions.h
  src/LogbookOptions.cpp
  src/LogbookHTML.h
  src/LogbookHTML.cpp
  src/CrewList.h
  src/CrewList.cpp
  src/boat.h
  src/boat.cpp
  src/Maintenance.h
  src/Maintenance.cpp
  src/OverView.h
  src/OverView.cpp
  src/Export.h
  src/Export.cpp
)

set(PKG_API_LIB api-17)  #  A directory in libs/ e. g., api-17 or api-16

macro(late_init)
  # Perform initialization after the PACKAGE_NAME library, compilers
  # and ocpn::api is available.
  if (PLUGIN_USE_SVG)
    target_compile_definitions(${PACKAGE_NAME} PUBLIC PLUGIN_USE_SVG)
  endif ()
endmacro ()

macro(add_plugin_libraries)
  # Add libraries required by this plugin
  add_subdirectory("libs/tinyxml")
  target_link_libraries(${PACKAGE_NAME} ocpn::tinyxml)

  add_subdirectory("libs/wxJSON")
  target_link_libraries(${PACKAGE_NAME} ocpn::wxjson)

  add_subdirectory("libs/plugingl")
  target_link_libraries(${PACKAGE_NAME} ocpn::plugingl)

  # Added by Shipdriver
  add_subdirectory("libs/jsoncpp")
  target_link_libraries(${PACKAGE_NAME} ocpn::jsoncpp)
  
  # Needed for Logbook  
  add_subdirectory("libs/nmea0183")
  target_link_libraries(${PACKAGE_NAME} ocpn::nmea0183

#  The wxsvg library enables SVG overall in the plugin
#  add_subdirectory("libs/wxsvg")
#  target_link_libraries(${PACKAGE_NAME} ocpn::wxsvg)

)

endmacro ()



