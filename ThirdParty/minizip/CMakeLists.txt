cmake_minimum_required(VERSION 2.4.4...3.15.0)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

project(minizip C)

set(VERSION 1.3)

set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Installation directory for executables")
set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Installation directory for libraries")
set(INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Installation directory for headers")
set(INSTALL_MAN_DIR "${CMAKE_INSTALL_PREFIX}/share/man" CACHE PATH "Installation directory for manual pages")
set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)
enable_testing()

check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(stdint.h    HAVE_STDINT_H)
check_include_file(stddef.h    HAVE_STDDEF_H)

#
# Check to see if we have large file support
#
set(CMAKE_REQUIRED_DEFINITIONS -D_LARGEFILE64_SOURCE=1)
# We add these other definitions here because CheckTypeSize.cmake
# in CMake 2.4.x does not automatically do so and we want
# compatibility with CMake 2.4.x.
if(HAVE_SYS_TYPES_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_SYS_TYPES_H)
endif()
if(HAVE_STDINT_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDINT_H)
endif()
if(HAVE_STDDEF_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDDEF_H)
endif()
check_type_size(off64_t OFF64_T)
if(HAVE_OFF64_T)
   add_definitions(-D_LARGEFILE64_SOURCE=1)
endif()
set(CMAKE_REQUIRED_DEFINITIONS) # clear variable

#
# Check for fseeko
#
check_function_exists(fseeko HAVE_FSEEKO)
if(NOT HAVE_FSEEKO)
    add_definitions(-DNO_FSEEKO)
endif()

#
# Check for unistd.h
#
check_include_file(unistd.h Z_HAVE_UNISTD_H)

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    set(WIN_FILES iowin32.h iowin32.c)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
endif()


set(MINIZIP_PC ${CMAKE_CURRENT_BINARY_DIR}/minizip.pc)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/minizip.pc.cmakein
		${MINIZIP_PC} @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})

set(MINIZIP_PUBLIC_HDRS
    ioapi.h
    zip.h
    unzip.h
    mzconfig.h
)
set(MINIZIP_PRIVATE_HDRS
    mztools.h
    iowin32.h
    crypt.h
)
set(MINIZIP_SRCS
    ioapi.c
    iowin32.c
    miniunz.c
    minizip.c
    mztools.c
    unzip.c
    zip.c
)

add_library(minizip SHARED ${MINIZIP_SRCS} ${MINIZIP_DLL_SRCS} ${MINIZIP_PUBLIC_HDRS} ${MINIZIP_PRIVATE_HDRS} ${WIN_FILES})
add_library(minizipstatic STATIC ${MINIZIP_SRCS} ${MINIZIP_PUBLIC_HDRS} ${MINIZIP_PRIVATE_HDRS} ${WIN_FILES})
set_target_properties(minizipstatic PROPERTIES DEFINE_SYMBOL MZ_LOCAL)
set_target_properties(minizip PROPERTIES SOVERSION 1)
set_target_properties(minizip PROPERTIES DEFINE_SYMBOL MZ_DLL_EXPORTS)
# add_dependencies(minizip zlib)

find_package(ZLIB)
include_directories(${ZLIB_INCLUDE_DIR})

link_directories(${ZLIB_LIB})

message(STATUS "zlib include dir: ${ZLIB_LIB}")

target_link_libraries(${PROJECT_NAME}
PRIVATE
    ZLIB::ZLIB
)

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL )
    install(TARGETS minizip minizipstatic
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" )
endif()
if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL )
    install(FILES ${MINIZIP_PUBLIC_HDRS} DESTINATION "${INSTALL_INC_DIR}")
endif()
if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL )
    install(FILES miniunzip.1 DESTINATION "${INSTALL_MAN_DIR}/man3")
    install(FILES minizip.1 DESTINATION "${INSTALL_MAN_DIR}/man3")
endif()
if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL )
    install(FILES ${MINIZIP_PC} DESTINATION "${INSTALL_PKGCONFIG_DIR}")
endif()

