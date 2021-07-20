//
// Created by transwarp on 2021/6/22.
//
#pragma once
#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include "enum_parser.h"

#include <cstring>
#include <ctime>
#include <string>

enum class DataType {
  d_time,
  d_char,
  d_int,
  d_unsigned,
  d_float,
  d_double,
  d_boolean,
  d_short
};

inline size_t GetDataTypeSize(DataType dataType) {
  switch (dataType) {
    case DataType::d_time:
      return sizeof(time_t);
    case DataType::d_char:
      return sizeof(char);
    case DataType::d_int:
      return sizeof(int);
    case DataType::d_unsigned:
      return sizeof(unsigned);
    case DataType::d_float:
      return sizeof(float);
    case DataType::d_double:
      return sizeof(double);
    case DataType::d_boolean:
      return sizeof(bool);
    case DataType::d_short:
      return sizeof(short);
    default:
      break;
  }
  return 0;
}

template<typename T>
inline DataType GetDataType();

template<>
inline DataType GetDataType<time_t>() {
  return DataType::d_time;
};

template<>
inline DataType GetDataType<char>() {
  return DataType::d_char;
};

template<>
inline DataType GetDataType<int>() {
  return DataType::d_int;
};

template<>
inline DataType GetDataType<unsigned>() {
  return DataType::d_unsigned;
};

template<>
inline DataType GetDataType<float>() {
  return DataType::d_float;
};

template<>
inline DataType GetDataType<double>() {
  return DataType::d_double;
};

template<>
inline DataType GetDataType<bool>() {
  return DataType::d_boolean;
};

template<>
inline DataType GetDataType<short>() {
  return DataType::d_short;
};

template<>
inline EnumParser<DataType>::EnumParser() {
  enum_map_["time"] = DataType::d_time;
  enum_map_["char"] = DataType::d_char;
  enum_map_["int"] = DataType::d_int;
  enum_map_["unsigned"] = DataType::d_unsigned;
  enum_map_["float"] = DataType::d_float;
  enum_map_["double"] = DataType::d_double;
  enum_map_["bool"] = DataType::d_boolean;
  enum_map_["short"] = DataType::d_short;
}

inline std::string GetDataTypeString(DataType dataType) {
  switch (dataType) {
    case DataType::d_time:
      return "time_t";
    case DataType::d_char:
      return "char";
    case DataType::d_int:
      return "int";
    case DataType::d_unsigned:
      return "unsigned";
    case DataType::d_float:
      return "float";
    case DataType::d_double:
      return "double";
    case DataType::d_boolean:
      return "bool";
    case DataType::d_short:
      return "short";
    default:
      return "None";
  }
}

#endif //DATA_TYPE_H
