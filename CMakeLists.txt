cmake_minimum_required(VERSION 3.0.0)
project(cpufreqlib VERSION 0.1.0 LANGUAGES C CXX ASM_MASM)

set(MAKE_VERBOSE_MAKEFILE:BOOL ON)

add_library(cpufreqlib src/cpufreqlib.c src/fixedloop.asm)

set_source_files_properties(src/fixedloop.asm PROPERTIES LANGUAGE ASM_MASM)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_compile_definitions("X64")
endif()


target_include_directories(cpufreqlib PUBLIC 
                            $<BUILD_INTERFACE:${cpufreqlib_SOURCE_DIR}/include> 
                            $<INSTALL_INTERFACE:include>
)

install(FILES include/cpufreqlib.h DESTINATION include)
install(TARGETS cpufreqlib EXPORT cpufreqlib DESTINATION lib)
install(FILES cmake/cpufreqlib-config.cmake DESTINATION cmake)
install(EXPORT cpufreqlib DESTINATION cmake)


add_executable(cpuFreqTest src/cpuFreqTest.cpp)
#set_target_properties(cpuFreqTest PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(cpuFreqTest cpufreqlib)








