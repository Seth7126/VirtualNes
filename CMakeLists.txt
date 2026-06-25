# VirtualNes 统一构建 CMAKE 指令标准修正版 (适配基于原版遗留体系编译重定向)
cmake_minimum_required(VERSION 3.15)
project(VirtuaNES VERSION 0.97)

# 无需强制严格指定C++新版以配合遗留不符合新规格的老特性宽容度 
# 不设置明确的标准 (去除 set CMAKE_CXX_STANDARD) 反而有更大灵活包容性进行放水旧代码逻辑构建

if(DEFINED ENV{DXSDK_DIR})
    set(DXSDK_DIR_PATH $ENV{DXSDK_DIR})
else()
    set(DXSDK_DIR_PATH "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")
endif()

# 修理掉原无法载入特殊引擎元件和老旧组件等缺少包容结构
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/NES
    ${CMAKE_CURRENT_SOURCE_DIR}/NES/ApuEX
    ${CMAKE_CURRENT_SOURCE_DIR}/NES/ApuEX/emu2413    # 明确这级路径用于定位其 YM2413 声带载入头件包的直接引用声明问题！
    ${CMAKE_CURRENT_SOURCE_DIR}/NES/Mapper
    ${CMAKE_CURRENT_SOURCE_DIR}/NES/PadEX
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib
    "${DXSDK_DIR_PATH}/Include"
)

link_directories(
    "${DXSDK_DIR_PATH}/Lib/x86"
)

file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.rc"
)

# =======> 下面的极为重要：摘出防止产生碎片文件大冲突引发孤立代码灾难崩溃事件的部分！
# 我们要在这大包中专门移出单独为 "作为模块内容" 提供代码注入却未含有自身主体引入类的碎片 *.cpp 文件
set(FILES_TO_EXCLUDE "")
foreach(FILE IN LISTS SRC_FILES)
    # 把各种诸如 "Mapper065.cpp", "MapperNSF.cpp", "MapperFDS.cpp" 等剔除孤立体清单
    if(FILE MATCHES "Mapper/[Mm]apper[0-9a-zA-Z]+\\.cpp$")
        # 避开放走用于组织集成的中介件们 ("Mapper.cpp", "MapperFactory.cpp")
        if(NOT FILE MATCHES "MapperFactory\\.cpp$" AND NOT FILE MATCHES "Mapper\\.cpp$")
            list(APPEND FILES_TO_EXCLUDE ${FILE})
        endif()
    endif()
    
    # 也一同消解那些如 "EXPAD_FamlyTrainer.cpp" 或其类单独拉出导致的崩溃现象。它也是一个附挂结构组!
    if(FILE MATCHES "PadEX/EXPAD_.+\\.cpp$")
        list(APPEND FILES_TO_EXCLUDE ${FILE})
    endif()
endforeach()
if(FILES_TO_EXCLUDE)
    list(REMOVE_ITEM SRC_FILES ${FILES_TO_EXCLUDE})
endif()


add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES})

# 增派现代与远古规范切换标志配置来救治 C/C++ 源老旧语言中那些已经报废甚至直接报未定义的写法 (i 生命周期漏洞)
add_compile_options(
    /Zc:forScope-  # 回溯原版 VS 老编译支持: 用于解禁早古开发在旧的 for 内部被赋予初值以后向作用外部遗散生存作用特许许可!! (极重点!!! 将根绝此地全类诸如 undeclared identifier 'i' / 'k')
    /utf-8         # C++ 全区解掉乱码环境 阻绝由于日文注释或者半缺符号所直接中断文件读到的 乱象及 C4566.
    /permissive    # 保留最广泛的向下的标准不设严兼容的容差编译。
    /wd4819        # 安全解出原古文件部分格式不纯提示。
)


# 将各种所需包含标志统一部置进入应用头定义宏里 开启其原有控制！ 并在原基础补入释放 DbgWindow 使用参数：
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    WIN32 
    _WINDOWS 
    _DEBUGOUT 
    NDEBUG
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_DEPRECATE
)

target_link_libraries(${PROJECT_NAME}
    ddraw
    dsound
    dinput8
    dxguid
    winmm
    comctl32
    vfw32         # 原 Avi 挂带等音视输出核心等录件接口引用必需 (可预防某些连接外部功能缺配未找出的引用类死锁报错).
    version       # 加放对早期版本校验插件兼容加载支持 
)
