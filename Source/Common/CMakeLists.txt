add_library(Common
  File.h
  File.cpp
  String.h
  Logger.h
  Logger.cpp
  ParameterParser.h
  ParameterParser.cpp
  Types.h)

SET_TARGET_PROPERTIES(Common PROPERTIES LINKER_LANGUAGE CXX)
