find_package(Qt5 REQUIRED COMPONENTS Gui Widgets)

set(CMAKE_AUTOMOC ON)

add_executable(ApeQt
  Main.cpp
  MainWindow.h
  MainWindow.cpp
  TTYWidget.cpp
  TTYWidget.h)

target_link_libraries(ApeQt
PRIVATE
  Core
  Qt5::Widgets)
