# VirtualNES - 古董化兼容特别修正 CMAKE 版
cmake_minimum_required(VERSION 3.15)
project(VirtuaNES VERSION 0.97)

# 【核心解毒操作一】 宣称并启用静态的旧式基础封装框架类层 MFC！！无这个根本启动不起来编译！
set(CMAKE_MFC_FLAG 1) # (值为 1 即：将底层图形调用在静态下静态链接兼容执行)

if(DEFINED ENV{DXSDK_DIR})
    set(DXSDK_DIR_PATH $ENV{DXSDK_DIR})
else()
    set(DXSDK_DIR_PATH "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")
endif()

# 所有引入支持均放置此下：其中专门指派其对早时音源扩展 /emu2413 提供目录渗透。
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

# 统一抓出各种源代码进行过滤和清扫构建（排除单独组建引发错误污染库文件群！）
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.rc"
)

# 【核心解毒操作二】：绝育清除非主力文件的参差部分不被纳入主轴连缀触发“天女散花”般崩塌，全隔离如下名单！！
set(FILES_TO_EXCLUDE "")
foreach(FILE IN LISTS SRC_FILES)
    if(FILE MATCHES "Mapper/[Mm]apper[0-9a-zA-Z]+\\.cpp$"
        OR FILE MATCHES "PadEX/EXPAD_.+\\.cpp$"
        OR FILE MATCHES "NES/ROM_Patch\\.cpp$"
        OR FILE MATCHES "NES/VsUnisystem\\.cpp$"
        OR FILE MATCHES "NES/VS_Setting\\.h$")
        
        # 只保留这些正体组中做为调度指挥所必需不包含上述特殊子分项的大佬级别源身。
        if(NOT FILE MATCHES "MapperFactory\\.cpp$" AND NOT FILE MATCHES "Mapper\\.cpp$")
            list(APPEND FILES_TO_EXCLUDE ${FILE})
        endif()
    endif()
endforeach()
if(FILES_TO_EXCLUDE)
    list(REMOVE_ITEM SRC_FILES ${FILES_TO_EXCLUDE})
endif()

# 开始搭建源应用程序本身执行流 （Win32 是保证不要打出 cmd 小黑方块在背后跳脸）。
add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES})

# 【核心解毒操作三】全局下架那些新时代的管手规则束缚（例如：宽恕对于变量越狱在老版的允许）。
# 注: 直接全局改在配置底层（取代使用不稳定的 add_compile_options）以100%强压住这版高傲编译器！
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:forScope- /permissive /wd4430")

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    WIN32 
    _WINDOWS 
    _DEBUGOUT 
    NDEBUG
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_DEPRECATE
)

# 添加链接。
target_link_libraries(${PROJECT_NAME}
    ddraw
    dsound
    dinput8
    dxguid
    winmm
    vfw32 
    version
    comctl32
)
