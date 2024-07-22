cmake_minimum_required(VERSION 3.0.0)
project(wepoll)

include(CMakeParseArguments)

link_libraries(ws2_32)

if(MSVC)
  add_compile_options(/Wall /WX /wd4127 /wd4191 /wd4201 /wd4242 /wd4710 /wd4711 /wd4820)
  if(MSVC_VERSION GREATER_EQUAL 1900)
    add_compile_options(/wd5045)
  endif()
else()
  add_compile_options(-Wall -Wextra -Werror)
endif()

# Work around bug in in Windows SDK 10.0.17763.0 and maybe other versions.
add_definitions(-D_CRT_HAS_CXX17=0)

file(GLOB SOURCES_DOC LICENSE *.md)
file(GLOB_RECURSE SOURCES_CONFIG config/*.h)
file(GLOB SOURCES_CONFIG_EXTERNAL_DLLEXPORT config/external/dllexport/*.h)
file(GLOB SOURCES_CONFIG_EXTERNAL_BUNDLE config/external/bundle/*.h)
file(GLOB SOURCES_CONFIG_EXTERNAL_STATIC config/external/static/*.h)
file(GLOB SOURCES_CONFIG_INTERNAL_BUNDLE config/internal/bundle/*.h)
file(GLOB SOURCES_CONFIG_INTERNAL_DEFAULT config/internal/default/*.h)
file(GLOB SOURCES_INCLUDE include/*.h)
file(GLOB SOURCES_SRC src/*.c src/*.h)
file(GLOB SOURCES_SRC_C src/*.c)
file(GLOB SOURCES_TEST test/*.c)
file(GLOB SOURCES_TEST_LEAK_CHECK test/shared/leak-check.*)
file(GLOB SOURCES_TEST_UTIL test/shared/test-util.*)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(doc FILES ${SOURCES_DOC})
source_group(config FILES ${SOURCES_CONFIG})
source_group(include FILES ${SOURCES_INCLUDE})
source_group(src FILES ${SOURCES_SRC})
source_group("" FILES ${SOURCES_TEST})
source_group(test/shared FILES ${SOURCES_TEST_LEAK_CHECK} ${SOURCES_TEST_UTIL})

get_filename_component(LIB_NAME ${SOURCES_INCLUDE} NAME_WE)

set(TOOL_RUN_TESTS "tools/run-tests.js")
set(TOOL_BUNDLE "tools/bundle.js")

set(DIST_DIR "${PROJECT_BINARY_DIR}/dist")
set(GEN_DIR "${PROJECT_BINARY_DIR}/gen")

file(MAKE_DIRECTORY ${DIST_DIR})
file(MAKE_DIRECTORY ${GEN_DIR})

function(bundle_header)
  cmake_parse_arguments(BUNDLE "" "OUTPUT;EXTERNAL_CONFIG" "" ${ARGN})
  set(CONFIG_EXTERNAL_DIR "config/external/${BUNDLE_EXTERNAL_CONFIG}")
  file(GLOB SOURCES_CONFIG_EXTERNAL "${CONFIG_EXTERNAL_DIR}/*.h")
  add_custom_command(
    OUTPUT ${BUNDLE_OUTPUT}
    COMMAND node ${TOOL_BUNDLE}
            -I${CONFIG_EXTERNAL_DIR}
            ${SOURCES_INCLUDE}
            > ${BUNDLE_OUTPUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${TOOL_BUNDLE} ${SOURCES_CONFIG_EXTERNAL} ${SOURCES_INCLUDE}
  )
endfunction()

function(bundle_source)
  cmake_parse_arguments(BUNDLE "" "OUTPUT;EXTERNAL_CONFIG" "" ${ARGN})
  set(CONFIG_EXTERNAL_DIR "config/external/${BUNDLE_EXTERNAL_CONFIG}")
  file(GLOB SOURCES_CONFIG_EXTERNAL "${CONFIG_EXTERNAL_DIR}/*.h")
  add_custom_command(
    OUTPUT ${BUNDLE_OUTPUT}
    COMMAND node ${TOOL_BUNDLE} --strip-guards
            -I${CONFIG_EXTERNAL_DIR} -Iconfig/internal/bundle -Iinclude -Isrc
            ${SOURCES_INCLUDE} ${SOURCES_SRC_C}
            > ${BUNDLE_OUTPUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${TOOL_BUNDLE}
            ${SOURCES_CONFIG_EXTERNAL} ${SOURCES_CONFIG_INTERNAL_BUNDLE}
            ${SOURCES_INCLUDE} ${SOURCES_SRC}
  )
endfunction()

set(BUNDLE_DIST_TARGET "dist")
set(BUNDLE_DIST_HEADER "${DIST_DIR}/${LIB_NAME}.h")
set(BUNDLE_DIST_SOURCE "${DIST_DIR}/${LIB_NAME}.c")
bundle_header(OUTPUT ${BUNDLE_DIST_HEADER} EXTERNAL_CONFIG bundle)
bundle_source(OUTPUT ${BUNDLE_DIST_SOURCE} EXTERNAL_CONFIG bundle)
set_source_files_properties(
  ${BUNDLE_DIST_HEADER} ${BUNDLE_DIST_SOURCE}
  PROPERTIES GENERATED TRUE
)
add_custom_target(
  ${BUNDLE_DIST_TARGET}
  DEPENDS ${BUNDLE_DIST_SOURCE} ${BUNDLE_DIST_HEADER}
  SOURCES ${SOURCES_CONFIG_EXTERNAL_BUNDLE} ${SOURCES_CONFIG_INTERNAL_BUNDLE}
          ${SOURCES_INCLUDE} ${SOURCES_SRC} ${SOURCES_DOC}
)
add_custom_command(
  TARGET ${BUNDLE_DIST_TARGET} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCES_DOC} ${DIST_DIR}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${SOURCES_DOC}
)

set(DLL_TARGET "${LIB_NAME}.dll")
add_library(
  ${DLL_TARGET} SHARED
  ${SOURCES_CONFIG_EXTERNAL_DLLEXPORT} ${SOURCES_CONFIG_INTERNAL_DEFAULT}
  ${SOURCES_INCLUDE} ${SOURCES_SRC}
)
target_include_directories(
  ${DLL_TARGET} PUBLIC
  config/external/dllexport config/internal/default include src
)
set_target_properties(${DLL_TARGET} PROPERTIES OUTPUT_NAME ${DLL_TARGET})

set(BUNDLE_DLL_TARGET "${LIB_NAME}-bundle.dll")
set(BUNDLE_DLL_SOURCE "${GEN_DIR}/${LIB_NAME}-bundle-dll.c")
bundle_source(OUTPUT ${BUNDLE_DLL_SOURCE} EXTERNAL_CONFIG dllexport)
set_source_files_properties(${BUNDLE_DLL_SOURCE} PROPERTIES GENERATED TRUE)
add_library(
  ${BUNDLE_DLL_TARGET} SHARED
  ${SOURCES_CONFIG_EXTERNAL_DLLEXPORT} ${SOURCES_CONFIG_INTERNAL_DEFAULT}
  ${BUNDLE_DLL_SOURCE}
)
set_target_properties(
  ${BUNDLE_DLL_TARGET}
  PROPERTIES OUTPUT_NAME "${LIB_NAME}-bundle"
)

set(TESTING_LIB_TARGET "${LIB_NAME}-testing.lib")
add_library(
  ${TESTING_LIB_TARGET} STATIC
  ${SOURCES_CONFIG_EXTERNAL_STATIC} ${SOURCES_CONFIG_INTERNAL_DEFAULT}
  ${SOURCES_INCLUDE} ${SOURCES_SRC} ${SOURCES_TEST_UTIL}
)
target_include_directories(
  ${TESTING_LIB_TARGET} PUBLIC
  config/external/static config/internal/default include src
)
set_target_properties(
  ${TESTING_LIB_TARGET} PROPERTIES
  OUTPUT_NAME "${LIB_NAME}-testing"
)

foreach(TEST_SOURCE ${SOURCES_TEST})
  get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_SOURCE} ${SOURCES_TEST_LEAK_CHECK})
  target_include_directories(
    ${TEST_NAME} PUBLIC
    config/external/static config/internal/default include src test/shared
  )
  target_link_libraries(${TEST_NAME} ${TESTING_LIB_TARGET})
  set_target_properties(${TEST_NAME} PROPERTIES FOLDER Tests)
  list(APPEND TEST_TARGETS ${TEST_NAME})
  list(APPEND TEST_OUTPUTS $<TARGET_FILE:${TEST_NAME}>)
endforeach()

add_custom_target(
  test-all
  COMMAND node ${TOOL_RUN_TESTS} ${TEST_OUTPUTS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${TOOL_RUN_TESTS} ${TEST_TARGETS}
)
