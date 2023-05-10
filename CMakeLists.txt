# Nova-Context
# This file is part of the Nova-Context library source code.
# Copyright (c) 2023 - The Nova Project
#
# Parts of this build configuration have been brazenly stolen from the boost.context foundational library:
# https://github.com/boostorg/context
# Copyright (c) 2009 - Oliver Kowalke, licensed under the Boost Software License 1.0:
# http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5...3.16)
project(nova_context VERSION 0.1.0 LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(NOVA_CONTEXT_BUILD_STATIC "Build Nova-Context as static library" ON)
#option(NOVA_CONTEXT_BUILD_TEST "Nova-Context build test app" OFF)
option(NOVA_CONTEXT_PIC "Compile Nova-Context with position independent code (PIC)" OFF)
option(NOVA_CONTEXT_INSTALL "Generate installation target for Nova-Context" ON)

# Binary format
if(WIN32)
    set(_default_binfmt pe)
elseif(APPLE)
    set(_default_binfmt mach-o)
else()
    set(_default_binfmt elf)
endif()

set(NOVA_CONTEXT_BINARY_FORMAT "${_default_binfmt}" CACHE STRING "Nova-Context binary format (elf, mach-o, pe, xcoff)")
set_property(CACHE NOVA_CONTEXT_BINARY_FORMAT PROPERTY STRINGS elf mach-o pe xcoff)
unset(_default_binfmt)

# ABI
math(EXPR _bits "${CMAKE_SIZEOF_VOID_P} * 8")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^[Aa][Rr][Mm]" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(_default_abi aapcs)
elseif(WIN32)
    set(_default_abi ms)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^mips")
    if(_bits EQUAL 32)
        set(_default_abi o32)
    else()
        set(_default_abi n64)
    endif()
else()
    set(_default_abi sysv)
endif()

set(NOVA_CONTEXT_ABI "${_default_abi}" CACHE STRING "Nova-Context ABI (aapcs, eabi, ms, n32, n64, o32, o64, sysv, x32)")
set_property(CACHE NOVA_CONTEXT_ABI PROPERTY STRINGS aapcs eabi ms n32 n64 o32 o64 sysv x32)
unset(_default_abi)

# Arch-and-model
set(_all_archs arm arm64 loongarch64 mips32 mips64 ppc32 ppc64 riscv64 s390x i386 x86_64 combined)

# Try at start to auto determine arch from CMake.
if(CMAKE_SYSTEM_PROCESSOR IN_LIST _all_archs)
    set(_default_arch ${CMAKE_SYSTEM_PROCESSOR})
elseif(_bits EQUAL 32)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^[Aa][Rr][Mm]")
        set(_default_arch arm)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^mips")
        set(_default_arch mips32)
    else()
        set(_default_arch i386)
    endif()
else()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^[Aa][Rr][Mm]" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64") # armv8
        set(_default_arch arm64)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^mips")
        set(_default_arch mips64)
    else()
        set(_default_arch x86_64)
    endif()
endif()

set(NOVA_CONTEXT_ARCHITECTURE "${_default_arch}" CACHE STRING "Nova-Context architecture (arm, arm64, loongarch64, mips32, mips64, ppc32, ppc64, riscv64, s390x, i386, x86_64, combined)")
set_property(CACHE NOVA_CONTEXT_ARCHITECTURE PROPERTY STRINGS ${_all_archs})

unset(_all_archs)
unset(_bits)
unset(_default_arch)

# Assembler type
if(MSVC)
    if(NOVA_CONTEXT_ARCHITECTURE STREQUAL arm64 OR NOVA_CONTEXT_ARCHITECTURE STREQUAL arm)
        set(_default_asm armasm)
    else()
        set(_default_asm masm)
    endif()
else()
    set(_default_asm gas)
endif()

set(NOVA_CONTEXT_ASSEMBLER "${_default_asm}" CACHE STRING "Nova-Context assembler (masm, gas, armasm)")
set_property(CACHE NOVA_CONTEXT_ASSEMBLER PROPERTY STRINGS masm gas armasm)
unset(_default_asm)

# Assembler source suffix
if(NOVA_CONTEXT_BINARY_FORMAT STREQUAL pe)
    set(_default_ext .asm)
elseif(NOVA_CONTEXT_ASSEMBLER STREQUAL gas)
    set(_default_ext .S)
else()
    set(_default_ext .asm)
endif()

set(NOVA_CONTEXT_ASM_SUFFIX "${_default_ext}" CACHE STRING "Nova-Context assembler source suffix (.asm, .S)")
set_property(CACHE NOVA_CONTEXT_ASM_SUFFIX PROPERTY STRINGS .asm .S)
unset(_default_ext)

message(STATUS "Nova-Context: "
        "architecture ${NOVA_CONTEXT_ARCHITECTURE}, "
        "binary format ${NOVA_CONTEXT_BINARY_FORMAT}, "
        "ABI ${NOVA_CONTEXT_ABI}, "
        "assembler ${NOVA_CONTEXT_ASSEMBLER}, "
        "suffix ${NOVA_CONTEXT_ASM_SUFFIX}")

# Enable the right assembler
if(NOVA_CONTEXT_ASSEMBLER STREQUAL gas)
    if(CMAKE_CXX_PLATFORM_ID MATCHES "Cygwin")
        enable_language(CXX ASM-ATT)
    else()
        enable_language(CXX ASM)
    endif()
elseif(NOVA_CONTEXT_ASSEMBLER STREQUAL armasm)
    enable_language(CXX ASM_ARMASM)
else()
    enable_language(CXX ASM_MASM)

    # NOTE(julian): workaround for CMake MSVC ASM_MASM building bug #18889:
    #   https://gitlab.kitware.com/cmake/cmake/-/issues/18889
    set(CMAKE_ASM_MASM_CREATE_STATIC_LIBRARY "<CMAKE_AR> ${CMAKE_CL_NOLOGO} <LINK_FLAGS> /out:<TARGET> <OBJECTS> ")
endif()

# Choose .asm sources
if(NOVA_CONTEXT_BINARY_FORMAT STREQUAL mach-o)
    set(NOVA_CONTEXT_BINARY_FORMAT macho)
endif()

set(_asm_suffix
        "${NOVA_CONTEXT_ARCHITECTURE}_${NOVA_CONTEXT_ABI}_${NOVA_CONTEXT_BINARY_FORMAT}_${NOVA_CONTEXT_ASSEMBLER}${NOVA_CONTEXT_ASM_SUFFIX}")

set(ASM_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asm/make_${_asm_suffix}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asm/jump_${_asm_suffix}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asm/ontop_${_asm_suffix})

unset(_asm_suffix)

# POWERPC_32/SYSV
# Special case: https://github.com/boostorg/context/issues/120
if((NOVA_CONTEXT_ARCHITECTURE STREQUAL ppc32) AND (NOVA_CONTEXT_ABI STREQUAL sysv))
    string(APPEND ASM_SRC " ${CMAKE_CURRENT_SOURCE_DIR}/asm/tail_ppc32_sysv.cpp")
endif()

# Assembly source file properties
if(NOVA_CONTEXT_ASSEMBLER STREQUAL masm AND NOVA_CONTEXT_ARCHITECTURE STREQUAL i386)
    set_source_files_properties(${ASM_SRC} PROPERTIES COMPILE_FLAGS "/safeseh")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    set_source_files_properties(${ASM_SRC} PROPERTIES COMPILE_OPTIONS "-x assembler-with-cpp")
endif()

# Boost specific definitions
if(MSVC)
    add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
    add_definitions(-D_HAS_EXCEPTIONS=0)
endif()

# Find project sources
file(GLOB_RECURSE INC ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

# Create target
if(NOVA_CONTEXT_BUILD_STATIC)
    add_library(nova_context STATIC ${INC} ${SRC} ${ASM_SRC})
    target_compile_definitions(nova_context PUBLIC NOVA_CONTEXT_STATIC BOOST_CONTEXT_EXPORT=)
else()
    add_library(nova_context SHARED ${INC} ${SRC} ${ASM_SRC})
    target_compile_definitions(nova_context PUBLIC BOOST_CONTEXT_EXPORT=EXPORT)
endif()

# Link threading system dependencies
#if(WIN32)
#    target_link_libraries(nova_context wsock32 ws2_32)
#endif()
#if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
#    target_link_libraries(nova_context pthread)
#endif()

# Position-independent code
if(NOVA_CONTEXT_PIC)
    set_target_properties(nova_context PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Include directories
target_include_directories(nova_context PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if(NOVA_CONTEXT_BUILD_TEST)
    # TODO(julian): Build test project
endif()

# Install library
if(NOVA_CONTEXT_INSTALL)
    include(GNUInstallDirs)

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING
            PATTERN *.h)

    install(TARGETS nova_context
            EXPORT nova-context-export
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

    install(EXPORT nova-context-export
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nova-context
            FILE nova-context-config.cmake)
endif()
