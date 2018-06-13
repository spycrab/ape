add_library(Common
  String.h
  Logger.h
  Logger.cpp
  Types.h)

SET_TARGET_PROPERTIES(Common PROPERTIES LINKER_LANGUAGE CXX)
