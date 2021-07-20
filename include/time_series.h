//
// Created by transwarp on 2021/6/23.
//

#ifndef TIME_SERIES_H
#define TIME_SERIES_H

#include "column.h"
#include "ts_file_definition.h"

#include <string>
#include <utility>
#include <vector>
#include <limits>
#include <algorithm>
//#include <boost/make_shared.hpp>
#include <memory>

class TimeSeries;

using SharedTimeSeriesPtr = std::shared_ptr<TimeSeries>;

class TimeSeries {
public:
  TimeSeries() : name_(""), records_cnt_(0) {}

  explicit TimeSeries(std::string name) : name_(std::move(name)), records_cnt_(0) {}

  ~TimeSeries() = default;

  TimeSeries(const TimeSeries &other) : name_(other.name_),
                                        columns_(other.columns_),
                                        records_cnt_(other.records_cnt_) {}

  TimeSeries(TimeSeries &&other)  noexcept : name_(std::move(other.name_)),
                                   columns_(std::move(other.columns_)),
                                   records_cnt_(other.records_cnt_) {}

public:
  void init(const std::vector<DataType> &column_types) {
    for (auto &type : column_types) {
      columns_.emplace_back(type);
    }
  }

  void SetDecimals(std::vector<int> column_decimals) {
    size_t len = std::min(column_decimals.size(), columns_.size());
    for (int i = 0; i < len; ++i) {
      columns_[i].SetDecimal(column_decimals[i]);
    }
  }

  Column &GetColumn(size_t col_idx) {
    return columns_[col_idx];
  }

  std::string GetName() {
    return name_;
  }

  size_t GetColumnsNumber() {
    return columns_.size();
  }

  size_t GetRecordsCount() const {
    return records_cnt_;
  }

  void UpdateRecordsCnt() {
    records_cnt_ = columns_[0].GetSize() / columns_[0].GetDataSize();
  }

  void SetName(std::string name) {
    name_ = std::move(name);
  }

  void SetColumnNames(std::vector<std::string> names) {
    for (int i = 0; i < names.size(); ++i) {
      columns_[i].SetName(names[i]);
    }
  }

  std::vector<std::string> GetColumnNames() {
    std::vector<std::string> result;
    for (auto &column : columns_)
      result.push_back(column.GetName());
    return result;
  }

  size_t GetRecordSize() {
    size_t size = 0;
    for (auto &column : columns_)
      size += column.GetDataSize();
    return size;
  }

  void AddRecord(char *data) {
    size_t offset = 0;
    for (auto &column : columns_)
      offset += column.AddRawValue(data + offset);
    records_cnt_++;
  }

  void AddRecord(std::vector<std::string> &record) {
    if (record.size() != columns_.size())
      throw std::runtime_error(record_size_error_msg);

    for (int i = 0; i < record.size(); i++)
      columns_[i].AddStringValue(record[i]);
    records_cnt_++;
  }

  std::vector<RawData> GetRawRecordData(size_t row_idx) {
    std::vector<RawData> result;
    for (auto &column : columns_)
      result.push_back(column.GetRaw(row_idx));
    return result;
  }

  std::vector<std::string> GetRecordAsStrings(size_t row_idx) {
    std::vector<std::string> result;
    for (auto &column : columns_)
      result.push_back(column.GetStringValue(row_idx));
    return result;
  }

  bool Compare(TimeSeries &other) {
    if (records_cnt_ != other.GetRecordsCount())
      return false;
    for (int i = 0; i < GetColumnsNumber(); i++)
      if (!columns_[i].Compare(other.GetColumn(i)))
        return false;
    return true;
  }

  void Print(int n) {
    printf("TimeSeries %s:\n", GetName().c_str());
    for (int i = 0; i < n && i < GetRecordsCount(); i++) {
      printf("Row %d: ", i);
      for (auto &row : GetRecordAsStrings(i))
        printf("%s, ", row.c_str());
      printf("\n");
    }
    printf("--------\n");
  }

public:
  static SharedTimeSeriesPtr make_shared(FileDefinition &def) {
    std::shared_ptr<TimeSeries> ts = std::make_shared<TimeSeries>();
    ts->init(def.Columns);
    ts->SetColumnNames(def.Header);
    return ts;
  }

private:
  std::string name_;
  std::vector<Column> columns_;
  size_t records_cnt_;
private:
  const char *record_size_error_msg = "Record size does not equal number of columns";
};


#endif // TIME_SERIES_H
