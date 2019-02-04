# CMake "find" script for PDAL

set(PDAL_LIBRARIES )
set(PDAL_INCLUDE_DIRS )

macro(DEFINE_PDAL_MODULE NAME HEADER LIBRARY)
	find_path(PDAL_${NAME}_INCLUDE_DIR ${HEADER} HINTS "${CMAKE_PREFIX_PATH}/include")

	if(PDAL_${NAME}_INCLUDE_DIR)
		list(APPEND PDAL_INCLUDE_DIRS ${PDAL_${NAME}_INCLUDE_DIR})
	endif()

	set(PDAL_${NAME}_LIBRARY)
	find_library(PDAL_${NAME}_LIBRARY_RELEASE NAMES ${LIBRARY})

	if(PDAL_${NAME}_LIBRARY_RELEASE)
		list(APPEND PDAL_${NAME}_LIBRARY optimized ${PDAL_${NAME}_LIBRARY_RELEASE}
	endif()

	if(PDAL_${NAME}_LIBRARY)
		list(APPEND PDAL_LIBRARIES ${PDAL_${NAME}_LIBRARY})
	endif()
endmacro

DEFINE_PDAL_MODULE(cpp "pdal/pdal.hpp" pdalcpp)

if(WIN32)
	DEFINE_PDAL_MODULE(util "pdal/util/Utils.hpp" pdal_util)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PDAL DEFAULT_MSG PDAL_LIBRARIES PDAL_INCLUDE_DIRS)
