cmake_minimum_required(VERSION 2.8)

project(db_bms)

aux_source_directory(. ENTRY)
aux_source_directory(./dms DMS_SRC)
aux_source_directory(./bms BMS_SRC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
add_executable(db_dms ${ENTRY};${DMS_SRC};${BMS_SRC})

