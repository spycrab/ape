// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Common/ParameterParser.h"

#include <iostream>

bool ParameterParser::Parse(int argc, char** argv)
{
  for (int i = 1; i < argc; i++) {
    std::string parameter = argv[i];
    std::string value;

    if (parameter.substr(0, 2) == "--")
      parameter = parameter.substr(2);

    if (parameter.find('=') != std::string::npos) {
      value = parameter.substr(parameter.find('=') + 1);
      parameter = parameter.substr(0, parameter.find('='));
    } else if (i + 1 < argc) {
      value = argv[++i];
    }

    if (m_parameters.count(parameter)) {

      auto& p = m_parameters[parameter];
      switch (p.type) {
      case Parameter::Type::COMMAND:
        if (value != "") {
          std::cerr << "Cannot provide value for command '" << parameter << "'"
                    << std::endl;
          return false;
        }
        p.value = "true";
        break;
      case Parameter::Type::FLAG:
        p.value = value == "" ? "true" : value;
        break;
      case Parameter::Type::STRING:
        if (value == "") {
          std::cerr << "Must provide value for string '" << parameter << "'"
                    << std::endl;
          return false;
        }
        p.value = value;
      }

      continue;
    }

    std::cerr << "Invalid parameter '" << parameter << "'" << std::endl;
    return false;
  }

  return true;
}

void ParameterParser::AddCommand(const std::string& name)
{
  m_parameters[name] = {Parameter::Type::COMMAND};
}

void ParameterParser::AddFlag(const std::string& name)
{
  m_parameters[name] = {Parameter::Type::FLAG};
}

void ParameterParser::AddString(const std::string& name)
{
  m_parameters[name] = {Parameter::Type::STRING};
}

bool ParameterParser::CheckCommand(const std::string& name)
{
  return CheckFlag(name);
}

bool ParameterParser::CheckFlag(const std::string& name)
{
  return m_parameters[name].value == "true";
}

const std::string ParameterParser::GetString(const std::string& name)
{
  return m_parameters[name].value;
}
