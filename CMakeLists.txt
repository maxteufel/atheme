project(atheme)

cmake_minimum_required(VERSION 2.8)

include(ExternalProject)
ExternalProject_Add(mowgli-2
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libmowgli-2
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/libmowgli-2/configure --prefix=<INSTALL_DIR>
    BUILD_COMMAND ${MAKE}
    BUILD_IN_SOURCE 1)
ExternalProject_Get_Property(mowgli-2 INSTALL_DIR)

set(MOWGLI_LIB_DIR ${INSTALL_DIR}/lib)
set(MOWGLI_INCLUDE_DIR ${INSTALL_DIR}/include/libmowgli-2)
set(MOWGLI_LIBRARY_NAME ${CMAKE_SHARED_LIBRARY_PREFIX}mowgli-2${CMAKE_SHARED_LIBRARY_SUFFIX})
set(MOWGLI_LIBRARY ${INSTALL_DIR}/lib/${MOWGLI_LIBRARY_NAME})

add_library(libmowgli-2 SHARED IMPORTED)
set_target_properties(libmowgli-2 PROPERTIES IMPORTED_LOCATION ${MOWGLI_LIBRARY})
link_directories(${MOWGLI_LIB_DIR})
include_directories(${MOWGLI_INCLUDE_DIR})

file(GLOB LIBATHEMECORE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} libathemecore/*.c)
list(SORT LIBATHEMECORE_SOURCES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libathemecore)
include_directories(SYSTEM "/usr/include")
link_directories("/usr/lib")
link_directories("${CMAKE_INSTALL_PREFIX}/lib")

add_library(libathemecore SHARED ${LIBATHEMECORE_SOURCES})
add_dependencies(libathemecore mowgli-2)

target_link_libraries(libathemecore libmowgli-2)
target_link_libraries(libathemecore "/usr/lib/libqrencode.so")

set_target_properties(libathemecore PROPERTIES OUTPUT_NAME "athemecore")
set_target_properties(libathemecore PROPERTIES COMPILE_DEFINITIONS "PREFIX=\"${CMAKE_INSTALL_PREFIX}\";DOCDIR=\"${CMAKE_INSTALL_PREFIX}/doc\";MODDIR=\"${CMAKE_INSTALL_PREFIX}\";SHAREDIR=\"${CMAKE_INSTALL_PREFIX}\";LOGDIR=\"${CMAKE_INSTALL_PREFIX}/var\";DATADIR=\"${CMAKE_INSTALL_PREFIX}/etc\";RUNDIR=\"${CMAKE_INSTALL_PREFIX}/var\";SYSCONFDIR=\"${CMAKE_INSTALL_PREFIX}/etc\";LOCALEDIR=\"${CMAKE_INSTALL_PREFIX}/etc/locale\";BINDIR=\"${CMAKE_INSTALL_PREFIX}/bin\"")

file(GLOB ATHEME_SERVICES_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/services/*.c)
list(SORT ATHEME_SERVICES_SOURCES)

add_executable(atheme-services ${ATHEME_SERVICES_SOURCES})
target_link_libraries(atheme-services libathemecore)

#install(FILES "${MOWGLI_LIBRARY}.0.0.0" DESTINATION "lib")
install(TARGETS libathemecore DESTINATION "lib")
install(TARGETS atheme-services DESTINATION "bin")
