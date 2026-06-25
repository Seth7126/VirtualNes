# VirtualNes 清晰简单直白版本
cmake_minimum_required(VERSION 3.15)
project(VirtuaNES VERSION 0.97)

# 这句话告诉机器：我们要使用老旧界面的各种封装窗体（不可去！）
set(CMAKE_MFC_FLAG 1)

if(DEFINED ENV{DXSDK_DIR})
    set(DXSDK_DIR_PATH $ENV{DXSDK_DIR})
else()
    set(DXSDK_DIR_PATH "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")
endif()

# 第一件正事：从网络源实时拔过来压缩文件依赖，避免找不到。这个库叫zlib
include(FetchContent)
FetchContent_Declare(
    ZLIB
    URL "https://github.com/madler/zlib/archive/refs/tags/v1.3.1.zip"
)
FetchContent_MakeAvailable(ZLIB)

# 给模拟器指明它都依赖哪些目录下的头（含引用类声明的文件）
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

# DX文件引用的实际零件包(.lib库)
link_directories(
    "${DXSDK_DIR_PATH}/Lib/x86"
)

# 拿小网在当前库扫下所有的核心源文(.cpp/.c)，装进 ALL_SRC 名单
file(GLOB_RECURSE ALL_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.rc"
)

# 【核心整理：清洗名单中的非需要执行类和产生自导错项】
set(FINAL_SRC "")
foreach(FILE_PATH IN LISTS ALL_SRC)
    
    # 【土方法但是百分百精准】，我们只取单纯的“名字（叫啥）.cpp”，不管那些左划线还是右横杠去作对比匹配！
    get_filename_component(FNAME ${FILE_PATH} NAME)
    
    # ① 不要 build 这个临时的干扰地：
    if(FILE_PATH MATCHES "build")
        continue()
    endif()

    # ② 两个不要的外置小程序：不能要，有两套方向盘：
    if(FNAME STREQUAL "DebugWindow.cpp")
        continue()
    endif()

    # ③ 单独跑容易坏的特型文件：那些依附寄生性质存在的特殊补充块代码。
    if(FNAME MATCHES "^Mapper[a-zA-Z0-9_]+\\.cpp$")
        continue()
    endif()
    
    if(FNAME MATCHES "^EXPAD_.+\\.cpp$")
        continue()
    endif()
    
    if(FNAME STREQUAL "ROM_Patch.cpp")
        continue()
    endif()

    # 余下的全放进来
    list(APPEND FINAL_SRC ${FILE_PATH})

endforeach()

# 指定用那些余下全干净安全的文本打程序大基（WIN32 指这是一个要开有视窗不能跑出来 cmd 控制方台的那种桌级程式）。
add_executable(${PROJECT_NAME} WIN32 ${FINAL_SRC})

# /Zc:forScope-  不设旧的循环内含变量声明约束检查!   /wdXXXX 为解除无关注要格式告警告(烦人)。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:forScope- /permissive /wd4430 /wd4566 /wd4244 /wd4101")

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    WIN32 
    _WINDOWS 
    _DEBUGOUT 
    NDEBUG
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_DEPRECATE
)

# 为最后的执行文件配补上缺失的核心基础引向部件！特别是你要补上的系统文字组件 `imm32` 以及网络拔出来的解包核`zlibstatic`。
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
