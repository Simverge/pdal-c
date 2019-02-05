set(TARGET pdalc)

message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
message(STATUS "PDAL_DIR: ${PDAL_DIR}")

find_package(PDAL)

if(NOT PDAL_FOUND)
	message(STATUS "Could not find PDAL, attempting config-based search")
	find_package(PDAL REQUIRED CONFIG)
endif()

message(STATUS "Found PDAL ${PDAL_VERSION}")
message(STATUS "Found PDAL ${PDAL_LIBRARIES}")
message(STATUS "Found PDAL ${PDAL_LIBRARY_DIRS}")
message(STATUS "Found PDAL ${PDAL_INCLUDE_DIRS}")

set(SOURCES
	pdalc_config.cpp
	pdalc_dimtype.cpp
	pdalc_pipeline.cpp
	pdalc_pointlayout.cpp
	pdalc_pointview.cpp
	pdalc_pointviewiterator.cpp
)

set(HEADERS
	pdalc.h
	pdalc_config.h
	pdalc_defines.h
	pdalc_dimtype.h
	pdalc_forward.h
	pdalc_pipeline.h
	pdalc_pointlayout.h
	pdalc_pointview.h
	pdalc_pointviewiterator.h
)

set(DEPENDENCIES
	${PDAL_LIBRARIES}
)

link_directories(
	${PDAL_LIBRARY_DIRS}
)

include_directories(
	${PDAL_INCLUDE_DIRS}
)

add_definitions(${PDAL_DEFINITIONS})

add_library(${TARGET} SHARED ${SOURCES} ${HEADERS})

string(TOUPPER "${TARGET}_BUILD_DLL" BUILD_SYMBOL)

set_target_properties(${TARGET} PROPERTIES
	DEFINE_SYMBOL ${BUILD_SYMBOL}
	VERSION ${PDAL_VERSION}
	SOVERSION ${PDAL_VERSION}
)

# Measure code coverage on gcc
if(CMAKE_COMPILER_IS_GNUCXX AND PDALC_ENABLE_CODE_COVERAGE)
	SETUP_TARGET_FOR_COVERAGE(
		NAME coverage_${TARGET}
		EXECUTABLE test_${TARGET}
		DEPENDENCIES test_${TARGET}
	)
endif()

target_link_libraries(${TARGET} ${DEPENDENCIES})

install(TARGETS ${TARGET}
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin)

file(RELATIVE_PATH ${TARGET}_PATH ${CMAKE_SOURCE_DIR}/source ${CMAKE_CURRENT_SOURCE_DIR})
install(FILES ${HEADERS} DESTINATION include/${${TARGET}_PATH})
