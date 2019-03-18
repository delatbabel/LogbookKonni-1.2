##---------------------------------------------------------------------------
## Author:      nohal aka. Pavel Kalian
## Copyright:   
## License:     wxWidgets License
##---------------------------------------------------------------------------
 
# define minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)
cmake_policy(SET CMP0012 NEW)

PROJECT(logbookkonni_pi)

SET(PACKAGE_NAME logbookkonni_pi)
SET(VERBOSE_NAME Logbookkonni)
SET(TITLE_NAME LOGBOOKKONNI)
SET(CPACK_PACKAGE_CONTACT "Del Elson")

SET(BUNDLE_DATA TRUE)

INCLUDE( "VERSION.cmake" )
SET(OCPN_MIN_VERSION "ov50")
SET(PLUGIN_VERSION "${PLUGIN_VERSION_MAJOR}.${PLUGIN_VERSION_MINOR}.${PLUGIN_VERSION_PATCH}" )

SET(VERSION_MAJOR ${PLUGIN_VERSION_MAJOR})
SET(VERSION_MINOR "${PLUGIN_VERSION_MINOR}${PLUGIN_VERSION_PATCH}")
SET(VERSION_DATE ${PLUGIN_VERSION_DATE})

#SET(CMAKE_BUILD_TYPE Debug)

INCLUDE("cmake/PluginConfigure.cmake")

SET(SRC_LOGBOOKKONNI
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
        	src/json_defs.h
       		src/jsonreader.h
        	src/jsonval.h
        	src/jsonwriter.h
        	src/jsonreader.cpp
        	src/jsonval.cpp
        	src/jsonwriter.cpp
 	)

IF(WIN32)
SET(SRC_LTINYXML
		src/tinyxml/tinyxml.cpp
		src/tinyxml/tinyxml.h
		src/tinyxml/tinyxmlerror.cpp
		src/tinyxml/tinyxmlparser.cpp
		src/tinyxml/tinystr.cpp
		src/tinyxml/tinystr.h 
)
ENDIF(WIN32)

SET(SRC_LNMEA0183
		src/nmea0183/LatLong.hpp
		src/nmea0183/latlong.cpp
		src/nmea0183/long.cpp
		src/nmea0183/nmea0183.cpp
		src/nmea0183/nmea0183.hpp
		src/nmea0183/Response.hpp
		src/nmea0183/response.cpp
		src/nmea0183/RMB.hpp
		src/nmea0183/rmb.cpp
		src/nmea0183/Sentence.hpp
		src/nmea0183/sentence.cpp
		src/nmea0183/talkerid.cpp
		src/nmea0183/RMC.HPP
		src/nmea0183/rmc.cpp
		src/nmea0183/hexvalue.cpp
		src/nmea0183/lat.cpp
		src/nmea0183/expid.cpp
		src/nmea0183/wpl.hpp
		src/nmea0183/wpl.cpp
		src/nmea0183/rte.hpp
		src/nmea0183/rte.cpp
		src/nmea0183/hdt.hpp
		src/nmea0183/hdt.cpp
		src/nmea0183/hdg.hpp
		src/nmea0183/hdg.cpp
		src/nmea0183/hdm.hpp
		src/nmea0183/hdm.cpp
		src/nmea0183/gll.hpp
		src/nmea0183/gll.cpp
		src/nmea0183/vtg.hpp
		src/nmea0183/vtg.cpp
		src/nmea0183/gga.hpp
		src/nmea0183/gga.cpp
		src/nmea0183/gsv.hpp
		src/nmea0183/gsv.cpp
		src/nmea0183/dbt.cpp
		src/nmea0183/dbt.hpp
		src/nmea0183/dpt.cpp
		src/nmea0183/dpt.hpp
		src/nmea0183/mtw.cpp
		src/nmea0183/mtw.hpp
		src/nmea0183/mwv.cpp
		src/nmea0183/mwv.hpp
		src/nmea0183/vhw.hpp
		src/nmea0183/vhw.cpp
		src/nmea0183/vwr.cpp
		src/nmea0183/vwr.hpp
		src/nmea0183/vwt.cpp
		src/nmea0183/vwt.hpp
		src/nmea0183/xdr.cpp
		src/nmea0183/xdr.hpp
		src/nmea0183/zda.cpp
		src/nmea0183/zda.hpp
		src/nmea0183/rsa.hpp
		src/nmea0183/rsa.cpp
		src/nmea0183/SatInfo.h

        )

ADD_DEFINITIONS(-DTIXML_USE_STL)
ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_LOGBOOKKONNI} ${SRC_LTINYXML} ${SRC_LNMEA0183})

INCLUDE("cmake/PluginInstall.cmake")
INCLUDE("cmake/PluginLocalization.cmake")
INCLUDE("cmake/PluginPackage.cmake")
