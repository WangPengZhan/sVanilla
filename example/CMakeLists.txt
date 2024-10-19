# AlignConsecutiveMacros: 
cmake_minimum_required(VERSION 3.15)

option(SUI_Exmaple "ENABLE SUI example" ON)
if(SUI_Exmaple)
    add_subdirectory(SUI)
endif()
