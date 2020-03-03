cmake_minimum_required(VERSION 3.15)
project(curl_example C)

set(CMAKE_C_STANDARD 99)

#set(BUILD_FOR_ARM_LINUX ON)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")     ### if (${RELEASE}) 는 if (${RELEASE} == ON) 와 같다
    message("BUILD MODE : RELEASE")
    ## Build 하여 산출된 응용프로그램이 실행될 운영체제 선택
    ## 주석 시 Host OS / 주석 해제 시 ARM-Linux
    set(BUILD_FOR_ARM_LINUX ON)
else ()             ### DEBUG
    message("BUILD MODE : DEBUG")
endif ()

#################### Macro ######################
## ARM-Linux 툴체인 셋팅
## @Param
## ARM_LINUX_TOOLCHAIN_DIR : 툴체인 위치, 예시) /usr/local/arm-linux-gnueabi
macro(set_arm_linux_toolchain ARM_LINUX_TOOLCHAIN_DIR)
    ## arm-linux Cross Compile Options for macOS(Ubuntu dosen't need below options)
    set(CMAKE_C_COMPILER ${ARM_LINUX_TOOLCHAIN_DIR}/bin/arm-unknown-linux-gnueabi-gcc)
    set(CMAKE_LINKER ${ARM_LINUX_TOOLCHAIN_DIR}/bin/arm-unknown-linux-gnueabi-ld)
    set(CMAKE_NM ${ARM_LINUX_TOOLCHAIN_DIR}/bin/arm-unknown-linux-gnueabi-nm)
    set(CMAKE_OBJCOPY ${ARM_LINUX_TOOLCHAIN_DIR}/bin/arm-unknown-linux-gnueabi-objcopy)
    set(CMAKE_OBJDUMP ${ARM_LINUX_TOOLCHAIN_DIR}/bin/arm-unknown-linux-gnueabi-objdump)
    set(CMAKE_RANLIB ${ARM_LINUX_TOOLCHAIN_DIR}/bin/arm-unknown-linux-gnueabi-ranlib)
    include_directories(${ARM_LINUX_TOOLCHAIN_DIR}/arm-unknown-linux-gnueabi/sysroot/usr/include)

    if (APPLE)
        ## macOS - warning: cannot find entry symbol arch_paths_first; 에러 대처
        set(HAVE_FLAG_SEARCH_PATHS_FIRST 0)
        set(CMAKE_C_LINK_FLAGS "")
        set(CMAKE_CXX_LINK_FLAGS "")
    endif ()
endmacro()

macro(set_external_library_static)
    find_library(
            CURL
            NAMES libcurl.a
            PATHS ${EXTERNAL_LIB_DIR} NO_DEFAULT_PATH REQUIRED)
    set(SSL "")
    set(CRYPTO "")
    set(LIBZ "")
endmacro()

macro(set_external_library_dynamic)
    find_library(
            CURL
            NAMES curl
            PATHS ${EXTERNAL_LIB_DIR} NO_DEFAULT_PATH REQUIRED)
    set(SSL "")
    set(CRYPTO "")
    set(LIBZ "")
endmacro()

macro(set_external_library_dynamic_arm_linux)
    find_library(
            CURL
            NAMES curl
            PATHS ${CMAKE_SOURCE_DIR}/external/ARM-Linux/lib NO_DEFAULT_PATH REQUIRED)
    find_library(
            SSL
            NAMES ssl
            PATHS ${CMAKE_SOURCE_DIR}/external/ARM-Linux/lib NO_DEFAULT_PATH REQUIRED)
    find_library(
            CRYPTO
            NAMES crypto
            PATHS ${CMAKE_SOURCE_DIR}/external/ARM-Linux/lib NO_DEFAULT_PATH REQUIRED)
    find_library(
            LIBZ
            NAMES z
            PATHS ${CMAKE_SOURCE_DIR}/external/ARM-Linux/lib NO_DEFAULT_PATH REQUIRED)
endmacro()

##########################################
## CLion이 구동되는 운영체제에 따른 셋팅
if (WIN32)
    ### Microsoft Windows
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexec-charset=CP949")         # 윈도우에서 CLion을 통해 생성한 .exe 파일 실행 시 한글 깨짐 현상 방지
endif ()

message("BUILD HOST OS : ${CMAKE_HOST_SYSTEM_NAME}")
##########################################
## Build 하여 산출된 응용프로그램이 실행될 운영체제에 따른 셋팅 : ARM-Linux / Host OS 중 택
if (${BUILD_FOR_ARM_LINUX})
    message("BUILD TARGET OS : ARM-LINUX")
    set_arm_linux_toolchain(/usr/local/arm-unknown-linux-gnueabi)

    ### external 디렉터리 아래 CMake가 자동 인식하는 OS 이름으로 된 디렉터리 명에 따라서
    ### 해당 OS 에서 사용할 라이브러리를 선택해주는 매크로
    set(EXTERNAL_LIB_ROOT ${CMAKE_SOURCE_DIR}/external/ARM-Linux)
    set(EXTERNAL_INCLUDE_DIR ${EXTERNAL_LIB_ROOT}/include)
    set(EXTERNAL_LIB_DIR ${EXTERNAL_LIB_ROOT}/lib)
    set_external_library_dynamic_arm_linux()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--rpath=/data/glibc2.29/lib -Wl,--dynamic-linker=/data/glibc2.29/lib/ld-linux.so.3")
else () # Host OS
    message("BUILD TARGET OS : ${CMAKE_HOST_SYSTEM_NAME}")

    ### external 디렉터리 아래 CMake가 자동 인식하는 OS 이름으로 된 디렉터리 명에 따라서
    ### 해당 OS 에서 사용할 라이브러리를 선택해주는 매크로
    set(EXTERNAL_LIB_ROOT ${CMAKE_SOURCE_DIR}/external/${CMAKE_HOST_SYSTEM_NAME})
    set(EXTERNAL_INCLUDE_DIR ${EXTERNAL_LIB_ROOT}/include)
    set(EXTERNAL_LIB_DIR ${EXTERNAL_LIB_ROOT}/lib)
    set_external_library_dynamic()

endif ()

message("CURL : " ${CURL})
message("SSL : " ${SSL})
message("CRYPTO : " ${CRYPTO})
message("LIBZ : " ${LIBZ})
message("EXTERNAL_INCLUDE_DIR : " ${EXTERNAL_INCLUDE_DIR})
message("CMAKE_C_FLAGS : " ${CMAKE_C_FLAGS})

##########################################
## Application Build 셋팅
include_directories(
        ${EXTERNAL_INCLUDE_DIR}
        src
)

add_executable(curl_simple_req src/curl_simple_req.c)
target_link_libraries(curl_simple_req ${CURL} ${SSL} ${CRYPTO} ${LIBZ})

add_executable(curl_file_req src/curl_file_req.c)
target_link_libraries(curl_file_req ${CURL} ${SSL} ${CRYPTO} ${LIBZ})
