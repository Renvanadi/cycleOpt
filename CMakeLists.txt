cmake_minimum_required(VERSION 3.16)
project(cycleOpt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" ON)

# find_package OpenSim
find_package(OpenSim REQUIRED)

add_library(cycleopt_core
  src/domain/subject_model.cpp
  src/domain/bicycle_geometry.cpp
  src/domain/cycling_condition.cpp
  src/domain/riding_style.cpp
  src/domain/simulation_scenario.cpp
  src/application/optimization_service.cpp
  src/application/objective_function.cpp
  src/infrastructure/json_input_loader.cpp
  src/infrastructure/result_exporter.cpp
  src/infrastructure/open_sim_model_factory.cpp
  src/infrastructure/moco_problem_translator.cpp
  src/infrastructure/opensim_adapter.cpp
)

target_include_directories(cycleopt_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)

target_link_libraries(cycleopt_core
  PUBLIC
    osimMoco
)

if(OpenSim_FOUND)
  target_compile_definitions(cycleopt_core PRIVATE OPENSIM_FOUND)
endif()

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
