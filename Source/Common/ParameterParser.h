// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <map>
#include <string>

class ParameterParser
{
public:
  void AddCommand(const std::string& name);
  void AddFlag(const std::string& name);
  void AddString(const std::string& name);

  bool Parse(int argc, char** argv);

  bool CheckCommand(const std::string& name);
  bool CheckFlag(const std::string& name);
  const std::string GetString(const std::string& name);

private:
  struct Parameter {
    enum class Type { COMMAND, FLAG, STRING };
    Type type;
    std::string value = "";
  };

  std::map<std::string, Parameter> m_parameters;
};
