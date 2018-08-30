add_library(cgra SHARED libs/cgralib.cpp)
target_include_directories(cgra PRIVATE ${COREIR_INCLUDE_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR}/../include)

FILE(GLOB HEADER_FILES */*.h)
# don't know why everything is implemented in header files
add_executable(cgra-mapper ${HEADER_FILES} mapper.cpp)
target_include_directories(cgra-mapper PRIVATE ${COREIR_INCLUDE_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(cgra-mapper ${COREIR_LIBRARIES} cgra)
