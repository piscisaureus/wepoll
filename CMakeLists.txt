cmake_minimum_required(VERSION 2.8.0)
project(wepoll)

if(WIN32)
  link_libraries(ws2_32)
endif()

if(MSVC)
  add_compile_options(/Wall /wd4201 /wd4242 /wd4710 /wd4711 /wd4820)
else()
  add_compile_options(-Wall)
endif()

file(GLOB SOURCES_SRC src/*.c src/*.h)
file(GLOB SOURCES_SRC_C src/*.c)
file(GLOB SOURCES_SRC_REGULAR src/regular/*.c src/regular/*.h)
file(GLOB SOURCES_SRC_COMBINED src/combined/*.c src/combined/*.h)
file(GLOB SOURCES_INCLUDE include/*.h)
file(GLOB SOURCES_TEST test/*.c)
file(GLOB_RECURSE SOURCES_TEST_SHARED test/shared/*.c test/shared/*.h)

source_group("" FILES ${SOURCES_INCLUDE})
source_group(src FILES ${SOURCES_SRC})
source_group(src FILES ${SOURCES_SRC_REGULAR})
source_group(src FILES ${SOURCES_SRC_COMBINED})
source_group("" FILES ${SOURCES_TEST})
source_group(test/shared FILES ${SOURCES_TEST_SHARED})

foreach(TEST_SOURCE ${SOURCES_TEST})
  get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_SOURCE} ${SOURCES_SRC} ${SOURCES_SRC_REGULAR} ${SOURCES_TEST_SHARED})
  target_include_directories(${TEST_NAME} PUBLIC include src src/regular test/shared)
  list(APPEND TEST_TARGETS ${TEST_NAME})
  list(APPEND TEST_OUTPUTS $<TARGET_FILE:${TEST_NAME}>)
endforeach(TEST_SOURCE ${SOURCES_TEST})

set(TEST_ALL_STAMP "test-all.stamp")
set(TEST_RUNNER "tools/run-tests.js")
add_custom_command(
  OUTPUT ${TEST_ALL_STAMP}
  COMMAND node ${TEST_RUNNER} ${TEST_OUTPUTS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${TEST_RUNNER} ${TEST_TARGETS}
)
add_custom_target(test-all DEPENDS ${TEST_ALL_STAMP})

foreach(HEADER_SOURCE ${SOURCES_INCLUDE})
  get_filename_component(HEADER_NAME ${HEADER_SOURCE} NAME_WE)
  string(TOUPPER ${HEADER_NAME} HEADER_NAME_UC)

  set(COMBINED_NAME "${HEADER_NAME}-combined")
  set(DIST_SRC_C "dist/${HEADER_NAME}.c")
  set(DIST_SRC_H "dist/${HEADER_NAME}.h")
  set(SOURCES_README "doc/README.md")
  set(DIST_README "dist/README.md")
  add_custom_command(
    OUTPUT ${DIST_SRC_C}
    COMMAND node tools/combine.js -Iinclude -Isrc -Isrc/combined --strip-guards ${HEADER_SOURCE} ${SOURCES_SRC_C} > ${DIST_SRC_C}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    IMPLICIT_DEPENDS c ${SOURCES_INCLUDE} ${SOURCES_SRC} ${SOURCES_SRC_COMBINED}
  )
  add_custom_command(
    OUTPUT ${DIST_SRC_H}
    COMMAND node tools/combine.js ${HEADER_SOURCE} > ${DIST_SRC_H}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    IMPLICIT_DEPENDS c ${SOURCES_INCLUDE}
  )
  add_custom_command(
    OUTPUT ${DIST_README}
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCES_README} ${DIST_README}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${SOURCES_README}
  )
  add_custom_target(${COMBINED_NAME} DEPENDS ${DIST_SRC_C} ${DIST_SRC_H} ${DIST_README})

  set(COMBINED_DLL_NAME "${HEADER_NAME}-combined.dll")
  set(COMBINED_DLL_OUTPUT "${HEADER_NAME}-combined")
  add_library(${COMBINED_DLL_NAME} SHARED ${DIST_SRC_C})
  if(MSVC)
    target_compile_definitions(${COMBINED_DLL_NAME} PUBLIC "-D${HEADER_NAME_UC}_EXPORT=__declspec(dllexport)" )
  else()
    target_compile_definitions(${COMBINED_DLL_NAME} PUBLIC "-D${HEADER_NAME_UC}_EXPORT=__attribute__((visibility(\"default\")))")
  endif()
  set_target_properties(${COMBINED_DLL_NAME} PROPERTIES OUTPUT_NAME ${COMBINED_DLL_OUTPUT})

  set(DLL_NAME "${HEADER_NAME}.dll")
  set(DLL_OUTPUT "${HEADER_NAME}")
  add_library(${DLL_NAME} SHARED ${HEADER_SOURCE} ${SOURCES_SRC} ${SOURCES_SRC_REGULAR})
  target_include_directories(${DLL_NAME} PUBLIC include src/regular)
  if(MSVC)
    target_compile_options(${DLL_NAME} PUBLIC "-FI${HEADER_SOURCE}")
    target_compile_definitions(${DLL_NAME} PUBLIC "-D${HEADER_NAME_UC}_EXPORT=__declspec(dllexport)" )
  else()
    target_compile_options(${DLL_NAME} PUBLIC -include ${HEADER_SOURCE} -fvisibility=hidden)
    target_compile_definitions(${DLL_NAME} PUBLIC "-D${HEADER_NAME_UC}_EXPORT=__attribute__((visibility(\"default\")))")
  endif()
  set_target_properties(${DLL_NAME} PROPERTIES OUTPUT_NAME ${DLL_OUTPUT})

endforeach(HEADER_SOURCE ${SOURCES_INCLUDE})
