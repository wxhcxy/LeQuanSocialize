cmake_minimum_required(VERSION 3.27)

project(ServerTest LANGUAGES CXX)

add_executable(
  ServerTest
  main.cpp
  Socket.h
  TcpSocket.cpp
  TcpSocket.h
  Logger.cpp
  Logger.h
  SocketUtil.cpp
  SocketUtil.h
  TcpServer.h
  TcpServer.cpp
  EventLoop.cpp
  EventLoop.h
  EpollTaskScheduler.cpp
  EpollTaskScheduler.h
  Pipe.h
  Pipe.cpp
  TaskScheduler.cpp
  TaskScheduler.h
  RingBuffer.h
  Channel.h
  Timer.h
  Timer.cpp
  Acceptor.cpp
  Acceptor.h
  TcpConnection.cpp
  TcpConnection.h
  BufferWriter.cpp
  BufferReader.h
  BufferWriter.h
  BufferReader.cpp
  relationalbroker.h
  relationalbroker.cpp
  netizenbroker.h
  netizenbroker.cpp
  netizen.h
  netizen.cpp
  jottingbroker.h
  jottingbroker.cpp
  jotting.h
  jotting.cpp
  jottinginterface.h
  jottinginterface.cpp
  netizenproxy.h
  netizenproxy.cpp
  netizeninterface.h
  netizeninterface.cpp
  controller.h
  controller.cpp
  scanandcheckjottingcontroller.h
  scanandcheckjottingcontroller.cpp
  encodephoto.h
  encodephoto.cpp
  base64.h
  jottingproxy.h
  jottingproxy.cpp
  timeinfo.h
  materialbroker.h
  materialbroker.cpp
  material.h
  material.cpp
  materialinterface.h
  materialinterface.cpp
  materialproxy.h
  materialproxy.cpp
  commentbroker.h
  commentbroker.cpp
  comment.h
  comment.cpp
  commentinterface.h
  commentinterface.cpp
  commentproxy.h
  commentproxy.cpp
  controllerfactory.h
  controllerfactory.cpp
  cacheflush.cpp
  cacheflush.h
  mytimer.cpp
  mytimer.h
  initcontroller.cpp
  initcontroller.h
  messagecontroller.h
  messagecontroller.cpp
  messagesequence.cpp
  messagesequence.h
  jottingnotification.h
  jottingnotification.cpp
  publishjottingcontroller.cpp
  publishjottingcontroller.h
  snowflake_singleton.h
  snowflakeidworker.cpp
  snowflakeidworker.h
  snowflake_noncopyable.h
  test.h)

set(MARIADBCPP_LIBRARIES "/usr/local/lib/mariadb/libmariadbcpp.so")

set(BOOSTJSON_LIBRARIES "/usr/local/lib/libboost_json.a")
# 包含 Boost 头文件目录
include_directories(/usr/local/include/)

find_package(Threads REQUIRED) # 处理 pthread 依赖

target_link_libraries(${PROJECT_NAME} ${BOOSTJSON_LIBRARIES}
                      ${MARIADBCPP_LIBRARIES} pthread dl m)

target_compile_features(ServerTest PRIVATE cxx_std_20)

# include(GNUInstallDirs) install(TARGETS ServerTest LIBRARY DESTINATION
# ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )

# target_sources( ServerTest PRIVATE main.cpp
#
# PUBLIC
#
# )

# target_compile_options( ServerTest PRIVATE -fmodules-ts )

# target_include_directories( ServerTest PRIVATE /usr/include )

# target_link_directories( ServerTest PRIVATE /usr/lib )

# target_link_libraries( ServerTest PRIVATE m )

# set_target_properties( ServerTest PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS
# OFF etc.. ) 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
# set_target_properties(ServerTest PROPERTIES  CXX_STANDARD_REQUIRED on)

# --------------------------------------------------------------------------
# Adding build-requirements 源文件列表 target_sources( <target> PRIVATE
# <source-file>... )
#
# target_precompile_headers( <target> PRIVATE <header-file>... )
#
# 预处理头文件搜索路径列表，等价参数：-I target_include_directories( <target> PRIVATE
# <include-search-dir>... )
#
# 预编译宏 自动在指定的宏前添加，等价参数：-D target_compile_definitions( <target> PRIVATE
# <macro-definitions>... )
#
# 编译选项和特性 target_compile_options( <target> PRIVATE <compiler-option>... )
# target_compile_features( <target> PRIVATE <feature>... )
#
# 链接库 target_link_libraries( <target> PRIVATE <dependency>... )
#
# 链接选项 target_link_options( <target> PRIVATE <linker-option>... )
#
# 链接库搜索路径，等价参数：-L target_link_directories( <target> PRIVATE
# <linker-search-dir>... )

# execute_process(COMMAND ln -s /opt/gcc/gcm.cache  ./)

# Modern Cmake建议围绕Target和Property进行项目配置，竭力避免出现下面变量variable的set定义。
# Variable横行是典型CMake2.8时期的风格, Modern CMake则遵循了OO方法。其中，target表示待构建的对象，
# target相关的语句用于定义来约束link、compile等相关属性的scope。 set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 完整的project命令 project( ServerTest LANGUAGES CXX ) DESCRIPTION "Description of
# project" HOMEPAGE_URL "http://www.abc.com" VERSION 0.1.0 )
# execute_process(COMMAND g++ -std=c++20 -xc++-system-header -fmodules-ts #
# 如何要使用modules,该命令应该放在project命令之后 iostream string )
