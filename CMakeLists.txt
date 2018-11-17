cmake_minimum_required(VERSION 3.0.0)
project(wepoll)

link_libraries(ws2_32)

if(MSVC)
  add_compile_options(/Wall /WX /wd4127 /wd4201 /wd4242 /wd4710 /wd4711 /wd4820)
  if(MSVC_VERSION GREATER_EQUAL 1900)
    add_compile_options(/wd5045)
  endif()
else()
  add_compile_options(-Wall -Wextra -Werror)
endif()

set(DIST_DIR "dist")
set(TEST_RUNNER "tools/run-tests.js")

file(GLOB SOURCES_HEADER include/*.h)
file(GLOB SOURCES_SRC src/*.c src/*.h)
file(GLOB SOURCES_SRC_C src/*.c)
file(GLOB SOURCES_SRC_REGULAR src/regular/*.c src/regular/*.h)
file(GLOB SOURCES_SRC_COMBINED src/combined/*.c src/combined/*.h)
file(GLOB SOURCES_TEST test/*.c)
file(GLOB SOURCES_TEST_SHARED test/shared/*.c test/shared/*.h)
file(GLOB SOURCES_DOC LICENSE *.md)

get_filename_component(HEADER_NAME ${SOURCES_HEADER} NAME_WE)
string(TOUPPER ${HEADER_NAME} HEADER_NAME_UC)

set(COMBINED_HEADER "${DIST_DIR}/${HEADER_NAME}.h")
set(COMBINED_SRC "${DIST_DIR}/${HEADER_NAME}.c")
set(DLL_NAME "${HEADER_NAME}")
set(COMBINED_DLL_NAME "${HEADER_NAME}-combined")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group("" FILES ${SOURCES_HEADER})
source_group(src FILES ${SOURCES_SRC})
source_group(src FILES ${SOURCES_SRC_REGULAR})
source_group(src FILES ${SOURCES_SRC_COMBINED})
source_group("" FILES ${SOURCES_TEST})
source_group(test/shared FILES ${SOURCES_TEST_SHARED})
source_group(doc FILES ${SOURCES_DOC})
source_group(${DIST_DIR} FILES ${COMBINED_HEADER} ${COMBINED_SRC})

add_custom_command(
  OUTPUT ${COMBINED_HEADER}
  COMMAND node tools/combine.js ${SOURCES_HEADER} > ${COMBINED_HEADER}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  IMPLICIT_DEPENDS c ${SOURCES_HEADER}
)

add_custom_command(
  OUTPUT ${COMBINED_SRC}
  COMMAND node tools/combine.js -Iinclude -Isrc -Isrc/combined --strip-guards ${SOURCES_HEADER} ${SOURCES_SRC_C} > ${COMBINED_SRC}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  IMPLICIT_DEPENDS c ${SOURCES_HEADER} ${SOURCES_SRC} ${SOURCES_SRC_COMBINED}
)

add_custom_target(dist DEPENDS ${COMBINED_SRC} ${COMBINED_HEADER})
file(MAKE_DIRECTORY ${DIST_DIR})
add_custom_command(
  TARGET dist POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCES_DOC} ${DIST_DIR}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${SOURCES_DOC}
)

set(DLL_TARGET "${HEADER_NAME}.dll")
add_library(${DLL_TARGET} SHARED ${SOURCES_HEADER} ${SOURCES_SRC} ${SOURCES_SRC_REGULAR})
target_include_directories(${DLL_TARGET} PUBLIC include src/regular)
if(MSVC)
  target_compile_options(${DLL_TARGET} PUBLIC "-FI${SOURCES_HEADER}")
  target_compile_definitions(${DLL_TARGET} PUBLIC "-D${HEADER_NAME_UC}_EXPORT=__declspec(dllexport)" )
else()
  target_compile_options(${DLL_TARGET} PUBLIC -include ${SOURCES_HEADER} -fvisibility=hidden)
  target_compile_definitions(${DLL_TARGET} PUBLIC "-D${HEADER_NAME_UC}_EXPORT=__attribute__((visibility(\"default\")))")
endif()
set_target_properties(${DLL_TARGET} PROPERTIES OUTPUT_NAME ${DLL_NAME})

set(COMBINED_DLL_TARGET "${COMBINED_DLL_NAME}.dll")
add_library(${COMBINED_DLL_TARGET} SHARED ${COMBINED_HEADER} ${COMBINED_SRC})
if(MSVC)
  target_compile_definitions(${COMBINED_DLL_TARGET} PUBLIC "-D${HEADER_NAME_UC}_EXPORT=__declspec(dllexport)" )
else()
  target_compile_definitions(${COMBINED_DLL_TARGET} PUBLIC "-D${HEADER_NAME_UC}_EXPORT=__attribute__((visibility(\"default\")))")
endif()
set_target_properties(${COMBINED_DLL_TARGET} PROPERTIES OUTPUT_NAME ${COMBINED_DLL_NAME})

foreach(TEST_SOURCE ${SOURCES_TEST})
  get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_SOURCE} ${SOURCES_SRC} ${SOURCES_SRC_REGULAR} ${SOURCES_TEST_SHARED})
  set_target_properties(${TEST_NAME} PROPERTIES FOLDER Tests)
  target_include_directories(${TEST_NAME} PUBLIC include src src/regular test/shared)
  list(APPEND TEST_TARGETS ${TEST_NAME})
  list(APPEND TEST_OUTPUTS $<TARGET_FILE:${TEST_NAME}>)
endforeach(TEST_SOURCE ${SOURCES_TEST})

add_custom_target(
  test-all
  COMMAND node ${TEST_RUNNER} ${TEST_OUTPUTS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${TEST_RUNNER} ${TEST_TARGETS}
)
