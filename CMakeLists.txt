cmake_minimum_required(VERSION 3.16)
project(cycleOpt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" ON)

include(GNUInstallDirs)

add_library(cycleopt_core
  src/domain/subject_model.cpp
  src/domain/bicycle_geometry.cpp
  src/domain/cycling_condition.cpp
  src/domain/riding_style.cpp
  src/domain/simulation_scenario.cpp
  src/application/optimization_service.cpp
  src/infrastructure/json_input_loader.cpp
  src/infrastructure/result_exporter.cpp
)

target_include_directories(cycleopt_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(cycleopt_core PUBLIC cxx_std_17)

add_executable(cycleopt_cli
  src/interface/main.cpp
)

target_link_libraries(cycleopt_cli PRIVATE cycleopt_core)

if(BUILD_TESTS)
  enable_testing()
  add_executable(cycleopt_tests
    tests/test_main.cpp
  )
  target_link_libraries(cycleopt_tests PRIVATE cycleopt_core)
  add_test(NAME cycleopt_tests COMMAND cycleopt_tests)
endif()

install(TARGETS cycleopt_core cycleopt_cli
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
