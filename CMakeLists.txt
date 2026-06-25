# 此代码用于替代原有的 .dsw 和 .dsp 文件，使用 CMake 构建系统重新组织 VirtuaNES 项目。
# 它配置了项目名称、C++标准、包含目录，并收集了所有的源文件以编译为一个 Windows 桌面应用程序 (WIN32)。
# 同时也配置了针对旧版 DirectX SDK 的依赖及相关宏定义。

cmake_minimum_required(VERSION 3.15)
project(VirtuaNES VERSION 0.97)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)

# 配置 DirectX SDK (June 2010) 的路径
# 优先从环境变量读取，如果不存在则使用默认安装路径
if(DEFINED ENV{DXSDK_DIR})
    set(DXSDK_DIR_PATH $ENV{DXSDK_DIR})
else()
    set(DXSDK_DIR_PATH "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")
endif()

# 添加头文件包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/NES
    ${CMAKE_CURRENT_SOURCE_DIR}/NES/ApuEX
    ${CMAKE_CURRENT_SOURCE_DIR}/NES/Mapper
    ${CMAKE_CURRENT_SOURCE_DIR}/NES/PadEX
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib
    "${DXSDK_DIR_PATH}/Include"
)

# 添加库文件搜索目录
link_directories(
    "${DXSDK_DIR_PATH}/Lib/x86"
)

# 递归收集所有的源代码文件和资源文件
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.rc"
)

# 添加可执行文件目标，WIN32 参数指明这是一个 GUI 程序，运行时不显示控制台窗口
add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES})

# 注入编译宏定义，相当于原 .dsp 文件中的 /D "_DEBUGOUT" /D "NDEBUG" 等参数
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    WIN32 
    _WINDOWS 
    _DEBUGOUT 
    NDEBUG
    _CRT_SECURE_NO_WARNINGS # 屏蔽老代码中的 strcpy 等安全警告
)

# 链接 Windows 系统库和 DirectX 相关静态/动态库
target_link_libraries(${PROJECT_NAME}
    ddraw
    dsound
    dinput8
    dxguid
    winmm
    comctl32
)
