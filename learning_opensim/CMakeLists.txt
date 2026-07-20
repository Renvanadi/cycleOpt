cmake_minimum_required(VERSION 3.16)
project(LearningOpenSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSim REQUIRED)

# 1. Model Building Test
add_executable(test_model_building test_model_building.cpp)
#target_link_libraries(test_model_building PRIVATE osimSimulation)
target_link_libraries(test_model_building PRIVATE
    osimSimulation
    osimActuators
    osimAnalyses
    osimTools
)
# 2. Scaling Test
#add_executable(test_scaling test_scaling.cpp)
#target_link_libraries(test_scaling PRIVATE osimSimulation osimTools)
#
## 3. Moco Basics Test
#add_executable(test_moco_basics test_moco_basics.cpp)
#target_link_libraries(test_moco_basics PRIVATE osimMoco)
