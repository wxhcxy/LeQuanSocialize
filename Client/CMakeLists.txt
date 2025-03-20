cmake_minimum_required(VERSION 3.29)

project(
  ClientTestQmlRedBook
  VERSION 0.1
  LANGUAGES CXX)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Quick Widgets Gui QuickControls2
                                         Multimedia)

set(BOOSTJSON_LIBRARIES "/usr/local/lib/libboost_json.a")
# 包含 Boost 头文件目录
include_directories(/usr/local/include/)
include_directories(/usr/include/qt6/QtGui/6.7.1/)
include_directories(/usr/include/qt6/QtGui/6.7.1/QtGui/)

qt_standard_project_setup()

qt_add_executable(appClientTestQmlRedBook main.cpp)

qt_add_qml_module(
  appClientTestQmlRedBook
  URI
  ClientTestQmlRedBook
  VERSION
  1.0
  QML_FILES
  Main.qml
  SOURCES
  RESOURCES
  RESOURCES
  RESOURCES
  res.qrc
  QML_FILES
  InitInfoPage.qml
  SOURCES
  personaluicontrol.h
  personaluicontrol.cpp
  SOURCES
  base64.h
  base64.cpp
  SOURCES
  clientInfo.h
  SOURCES
  clientInfo.cpp
  client.h
  client.cpp
  SOURCES
  network.h
  network.cpp
  SOURCES
  imageprovider.h
  imageprovider.cpp
  SOURCES
  pushpagecontrol.cpp
  SOURCES
  pushpagecontrol.h
  QML_FILES
  PushPage.qml
  QML_FILES
  JotDetailPage.qml
  QML_FILES
  PersonalPage.qml
  QML_FILES
  PersonalRelationList.qml
  QML_FILES
  Toast.qml
  QML_FILES
  ProcessWidget.qml
  MessagePage.qml
  SOURCES
  messageuicontrol.cpp
  messageuicontrol.h
  QML_FILES
  VideoPage.qml
  SOURCES
  videopagecontrol.cpp
  videopagecontrol.h
  QML_FILES
  VideoPage.qml
  QML_FILES
  ChooseMaterialPage.qml
  QML_FILES
  PublishPage.qml
  QML_FILES
  mediaplayer/AudioControl.qml
  mediaplayer/PlaybackControl.qml
  mediaplayer/PlaybackRateControl.qml
  mediaplayer/PlaybackSeekControl.qml
  SOURCES
  publishpagecontrol.cpp
  publishpagecontrol.h
  SOURCES
  fileopendialog.cpp
  fileopendialog.h
  filesavedialog.cpp
  filesavedialog.h)

target_compile_features(appClientTestQmlRedBook PRIVATE cxx_std_20)

set_target_properties(
  appClientTestQmlRedBook
  PROPERTIES AUTORCC TRUE
             MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE)

target_link_libraries(
  appClientTestQmlRedBook
  PRIVATE Qt6::Core
          Qt6::Quick
          Qt6::Widgets
          Qt6::Gui
          Qt6::QuickControls2
          Qt6::Multimedia
          ${BOOSTJSON_LIBRARIES})

include(GNUInstallDirs)
install(
  TARGETS appClientTestQmlRedBook
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# qt_add_executable(ClientTestQmlRedBook)  //Creates and finalizes an
# application target of a platform-specific type. This command performs the
# following tasks: 1. Create a CMake target of the appropriate type for the
# target platform. 2. Link the target to the Qt::Core library. 3. Handle
# finalization of the CMake target.

# qt_standard_project_setup()     #set(CMAKE_AUTOUIC ON)  set(CMAKE_AUTOMOC ON)

# @if %{HasTranslation} find_package(Qt6 REQUIRED COMPONENTS Core Quick
# LinguistTools) @else find_package(Qt6 REQUIRED COMPONENTS Core Quick) @endif

# target_sources(appClientTestQmlRedBook PRIVATE main.cpp qml.qrc @if
# %{HasTranslation} %{TsFileName} @endif
#
# PUBLIC
#
# )

# qt_import_qml_plugins(ClientTestQmlRedBook)

# cmake_minimum_required(VERSION 3.16)
#
# project(ClientTestQmlRedBook VERSION 0.1 LANGUAGES CXX)
#
# @if !true set(CMAKE_AUTOMOC ON) @endif set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
# find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)
#
# @if true @if false qt_standard_project_setup(REQUIRES 6.5) @else
# qt_standard_project_setup() @endif @endif
#
# qt_add_executable(appClientTestQmlRedBook main.cpp )
#
# qt_add_qml_module(appClientTestQmlRedBook URI ClientTestQmlRedBook VERSION 1.0
# QML_FILES Main.qml )
#
# # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1. #
# If you are developing for iOS or macOS you should consider setting an #
# explicit, fixed bundle identifier manually though.
# set_target_properties(appClientTestQmlRedBook PROPERTIES #
# MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appClientTestQmlRedBook
# MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
# MACOSX_BUNDLE_SHORT_VERSION_STRING
# ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} MACOSX_BUNDLE TRUE
# WIN32_EXECUTABLE TRUE )
#
# target_link_libraries(appClientTestQmlRedBook PRIVATE Qt6::Quick )
#
# include(GNUInstallDirs) install(TARGETS appClientTestQmlRedBook BUNDLE
# DESTINATION . LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION
# ${CMAKE_INSTALL_BINDIR} )
