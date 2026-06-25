# VirtualNes 现代化稳定直白版本 - 通行证修复版
cmake_minimum_required(VERSION 3.15)
project(VirtuaNES VERSION 0.97)

# 这句话告诉机器：我们要使用老旧界面的各种封装窗体
set(CMAKE_MFC_FLAG 1)

if(DEFINED ENV{DXSDK_DIR})
    set(DXSDK_DIR_PATH $ENV{DXSDK_DIR})
else()
    set(DXSDK_DIR_PATH "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")
endif()

# 从网络源实时拉过来 zip 功能组件避免解压库丢失
include(FetchContent)
FetchContent_Declare(
    ZLIB
    URL "https://github.com/madler/zlib/archive/refs/tags/v1.3.1.zip"
)
FetchContent_MakeAvailable(ZLIB)

# 给模拟器指明它都依赖哪些目录下的引述库头文件
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/NES
    ${CMAKE_CURRENT_SOURCE_DIR}/NES/ApuEX
    ${CMAKE_CURRENT_SOURCE_DIR}/NES/ApuEX/emu2413
    ${CMAKE_CURRENT_SOURCE_DIR}/NES/Mapper
    ${CMAKE_CURRENT_SOURCE_DIR}/NES/PadEX
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib
    "${DXSDK_DIR_PATH}/Include"
)

link_directories(
    "${DXSDK_DIR_PATH}/Lib/x86"
)

file(GLOB_RECURSE ALL_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.rc"
)

# 【开始核心整理，防范大锅炖报错】
set(FINAL_SRC "")
foreach(FILE_PATH IN LISTS ALL_SRC)
    get_filename_component(FNAME ${FILE_PATH} NAME)
    
    if(FILE_PATH MATCHES "build")
        continue()
    endif()

    if(FNAME STREQUAL "DebugWindow.cpp")
        continue()
    endif()

    # 【这就是之前的痛点修补！】筛除一切杂物补充小号 Mapper，但**必须放行 MapperFactory** 老板！！
    if(FNAME MATCHES "^Mapper[a-zA-Z0-9_]+\\.cpp$")
        if(NOT FNAME STREQUAL "MapperFactory.cpp")
            continue()
        endif()
    endif()
    
    # 附带手柄功能件是也是在母文件被牵进的，必须作为非主文件独立拦截防冲撞。
    if(FNAME MATCHES "^EXPAD_.+\\.cpp$")
        continue()
    endif()
    
    # ROM 内部独立注册插件也不当核心运行抛掉。
    if(FNAME STREQUAL "ROM_Patch.cpp")
        continue()
    endif()

    # 安全可依赖的内容进终集清单
    list(APPEND FINAL_SRC ${FILE_PATH})

endforeach()

add_executable(${PROJECT_NAME} WIN32 ${FINAL_SRC})

# 旧标控制防禁及编码去扰宏压死旧写法：
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:forScope- /permissive /wd4430 /wd4566 /wd4244 /wd4101")

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    WIN32 
    _WINDOWS 
    _DEBUGOUT 
    NDEBUG
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_DEPRECATE
)

# 合并补齐 WinMM、Vfw32、ZLIB静组连编和所有基本面
target_link_libraries(${PROJECT_NAME}
    ddraw
    dsound
    dinput8
    dxguid
    winmm
    vfw32 
    version
    comctl32
    imm32         
    zlibstatic
)
