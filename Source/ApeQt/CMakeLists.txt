find_package(Qt5 REQUIRED COMPONENTS Gui Widgets)
find_package(Threads REQUIRED)

set(CMAKE_AUTOMOC ON)

add_executable(ApeQt
  Debugger/CodeWidget.h
  Debugger/CodeWidget.cpp
  Debugger/RegisterWidget.h
  Debugger/RegisterWidget.cpp
  Main.cpp
  MainWindow.h
  MainWindow.cpp
  TTYWidget.cpp
  TTYWidget.h)

target_link_libraries(ApeQt
PRIVATE
  Core
  Qt5::Widgets
  Threads::Threads)
