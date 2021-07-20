//
// Created by transwarp on 2021/6/22.
//

#ifndef COLUMN_H
#define COLUMN_H

#include "data_type.h"

#include <cstddef>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

struct RawData {
  char *raw_data_;
  size_t raw_size_;
};

class Column {

public:
  Column(DataType dataType) : type_(dataType), name_(""), decimal_(6) { init(0); }

  Column(DataType dataType, int decimal) : type_(dataType), name_(""), decimal_(decimal) { init(0); }

  Column(DataType dataType, std::string name) : type_(dataType), name_(name), decimal_(6) { init(0); }

  Column(DataType dataType, size_t initialSize) : type_(dataType), name_(""), decimal_(6) { init(initialSize); }

  Column(DataType dataType, size_t initialSize, int decimal) : type_(dataType), name_(""), decimal_(decimal) {
    init(initialSize);
  }

  Column(DataType dataType, size_t initialSize, std::string name, int decimal) :
          type_(dataType), name_(std::move(name)), decimal_(decimal) {
    init(initialSize);
  }

  ~Column() {
    delete[] data_;
  }

  Column(const Column &other) :
          type_(other.type_),
          actual_size_(other.actual_size_),
          data_size_(other.data_size_),
          allocated_size_(other.allocated_size_),
          decimal_(other.decimal_) {
    data_ = new char[allocated_size_];
    memcpy(data_, other.data_, actual_size_);
  }

public:
  std::string GetName() const {
    return name_;
  }

  size_t GetSize() const {
    return actual_size_;
  }

  size_t GetDataSize() const {
    return data_size_;
  }

  DataType GetType() const {
    return type_;
  }

  char *GetData() const {
    return data_;
  }

  int GetDecimal() const {
    return decimal_;
  }

  void SetName(std::string name) {
    name_ = std::move(name);
  }

  void SetDecimal(int decimal) {
    decimal_ = decimal;
  }

  template<typename T>
  T GetValue(size_t index) {
    checkIndex(index);
    /* Check if type of data is correct */
    if (GetDataType<T>() != type_) {
      throw std::runtime_error(wrong_data_type_error_message_);
    }
    /* return data */
    T *actual_data_ = (T *) data_;
    return data_ == nullptr ? '\0' : actual_data_[index];
  }

  RawData GetRaw(size_t index) {
    checkIndex(index);
    size_t actual_index = index * data_size_;
    return RawData{data_ + actual_index, data_size_};
  }

  template<typename T>
  std::string ToStringWithPrecision(T value) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(decimal_) << value;
    return ss.str();
  }

  std::string GetStringValue(size_t index) {
    time_t time;
    std::string time_string{};
    switch (type_) {
      case DataType::d_time:
        time = GetValue<time_t>(index);
        time_string = ctime(const_cast<const time_t *>(&time));
        time_string.erase(time_string.length() - 1);
        return time_string;
      case DataType::d_char:
        return boost::lexical_cast<std::string>(GetValue<char>(index));
      case DataType::d_int:
        return boost::lexical_cast<std::string>(GetValue<int>(index));
      case DataType::d_unsigned:
        return boost::lexical_cast<std::string>(GetValue<unsigned>(index));
      case DataType::d_float:
        return ToStringWithPrecision(GetValue<float>(index));
      case DataType::d_double:
        return ToStringWithPrecision(GetValue<double>(index));
      case DataType::d_boolean:
        return boost::lexical_cast<std::string>(GetValue<bool>(index));
      case DataType::d_short:
        return boost::lexical_cast<std::string>(GetValue<short>(index));
      default:
        return time_string;
    }
  }

  template<typename T>
  T GetCastedValue(size_t index) {
    checkIndex(index);
    size_t actual_index = index * data_size_;
    switch (type_) {
      case DataType::d_time:
        return static_cast<T>(*((time_t *) (data_ + actual_index)));
      case DataType::d_char:
        return static_cast<T>(*(data_ + actual_index));
      case DataType::d_int:
        return static_cast<T>(*((int *) (data_ + actual_index)));
      case DataType::d_unsigned:
        return static_cast<T>(*((unsigned *) (data_ + actual_index)));
      case DataType::d_float:
        return static_cast<T>(*((float *) (data_ + actual_index)));
      case DataType::d_double:
        return static_cast<T>(*((double *) (data_ + actual_index)));
      case DataType::d_boolean:
        return static_cast<T>(*((bool *) (data_ + actual_index)));
      case DataType::d_short:
        return static_cast<T>(*((short *) (data_ + actual_index)));
    }
  }

  template<typename T>
  void AddValue(T value) {
    /* check if type of data is correct */
    if (GetDataType<T>() != type_) {
      throw std::runtime_error(wrong_data_type_error_message_);
    }
    /* if we don't have enough storage - expand the array */
    if (allocated_size_ < actual_size_ + data_size_) {
      expand();
    }

    /* set next value */
    memcpy(data_ + actual_size_, &value, data_size_);
    actual_size_ += data_size_;
  }

  void AddStringValue(std::string &value) {
    std::tm time{};
    switch (type_) {
      case DataType::d_time:
        strptime(value.c_str(), "%c", &time);
        time.tm_isdst = -1;
        AddValue<time_t>(mktime(&time));
        break;
      case DataType::d_char:
        AddValue<char>(boost::lexical_cast<char>(value));
        break;
      case DataType::d_int:
        AddValue<int>(boost::lexical_cast<int>(value));
        break;
      case DataType::d_unsigned:
        AddValue<unsigned>(boost::lexical_cast<unsigned>(value));
        break;
      case DataType::d_float:
        AddValue<float>(boost::lexical_cast<float>(value));
        break;
      case DataType::d_double:
        AddValue<double>(boost::lexical_cast<double>(value));
        break;
      case DataType::d_boolean:
        AddValue<bool>(boost::lexical_cast<bool>(value));
        break;
      case DataType::d_short:
        AddValue<short>(boost::lexical_cast<short>(value));
        break;
    }
  }

  size_t AddRawValue(char *data) {
    if (allocated_size_ < actual_size_ + data_size_) {
      expand();
    }
    memcpy(data_ + actual_size_, data_, data_size_);
    actual_size_ += data_size_;
    return data_size_;
  }

  bool Compare(const Column &other) {
    if (type_ != other.GetType() || actual_size_ != other.GetSize())
      return false;
    auto result = std::memcmp(data_, other.GetData(), actual_size_);
    return result == 0;
  }

  void ReserveSize(size_t size) {
    if (actual_size_ > 0)
      delete[] data_;
    data_ = new char[size];
    allocated_size_ = size;
    actual_size_ = size;
  }

private:
  void init(size_t initSize) {
    if (allocated_size_ > 0) {
      allocated_size_ = initSize;
      data_ = new char[initSize];
      actual_size_ = 0;
      data_size_ = GetDataTypeSize(type_);
    }
  }

  void expand() {
    auto old_size = allocated_size_;
    auto old_data = data_;
    allocated_size_ = 2 * allocated_size_ + 4 * data_size_;
    memcpy(data_, old_data, old_size);
    delete[] old_data;
  }

  void checkIndex(size_t index) {
    if ((index * data_size_) > actual_size_) {
      throw std::runtime_error(index_out_of_bounds_error_message_);
    }
  }

private:
  size_t allocated_size_;
  size_t data_size_;
  size_t actual_size_;
  char *data_;
  DataType type_;
  std::string name_;
  int decimal_;

private:
  const std::string wrong_data_type_error_message_ =
          "Data type of column is not compatible with requested type";
  const std::string index_out_of_bounds_error_message_ =
          "There is no data on the position-index is too big";
};

#endif // COLUMN_H
