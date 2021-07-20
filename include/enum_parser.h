//
// Created by transwarp on 2021/6/22.
//

#ifndef ENUM_PARSER_H
#define ENUM_PARSER_H

#include <map>
#include <string>

template<typename T>
class EnumParser {
public:
  EnumParser();

  T Parse(const std::string &value) {
    auto iValue = enum_map_.find(value);
    if (iValue == enum_map_.end()) {
      throw std::runtime_error("Enum value cannot be parsed");
    }
    return iValue->second;
  }

private:
  std::map<std::string, T> enum_map_;
};

#endif // ENUM_PARSER_H
