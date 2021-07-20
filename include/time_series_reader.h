//
// Created by transwarp on 2021/6/23.
//

#ifndef TIME_SERIES_READER_H
#define TIME_SERIES_READER_H

#include "time_series.h"
#include "file.h"
#include "ts_file_definition.h"

#include <memory>

class TimeSeriesReader;

using SharedTimeSeriesReaderPtr = std::shared_ptr<TimeSeriesReader>;

class TimeSeriesReader {
public:
  TimeSeriesReader() : last_file_position_(0) {}

  virtual ~TimeSeriesReader() = default;

  TimeSeriesReader(const TimeSeriesReader &other) :
          last_file_position_(other.last_file_position_) {}

public:
  virtual SharedTimeSeriesPtr Read(File &file, int max_rows) = 0;

  virtual void Write(File &file, TimeSeries &time_series) = 0;

public:
  static FileDefinition ReadFileDefinition(File &file);

  static void WriteFileDefinition(File &file, FileDefinition &definition);

protected:
  size_t last_file_position_;
};

class TimeSeriesReaderCSV : public TimeSeriesReader {
public:
  TimeSeriesReaderCSV() = default;

  explicit TimeSeriesReaderCSV(CsvFileDefinition &definition) : definition_(definition) {}

  TimeSeriesReaderCSV(const TimeSeriesReaderCSV &other) : definition_(other.definition_) {}

  virtual ~TimeSeriesReaderCSV() {}

public:
  SharedTimeSeriesPtr Read(File &file, int max_rows) override;

  void Write(File &file, TimeSeries &time_series) override;

public:
  static std::shared_ptr<TimeSeriesReaderCSV> make_shared(CsvFileDefinition definition) {
    return std::make_shared<TimeSeriesReaderCSV>(definition);
  }

private:
  CsvFileDefinition definition_;
};


class TimeSeriesReaderBinary : public TimeSeriesReader {
public:
  TimeSeriesReaderBinary() : alignment_(0) {}

  explicit TimeSeriesReaderBinary(const BinaryFileDefinition &definition, size_t alignment = 0) :
          definition_(definition), alignment_(alignment) {}

  TimeSeriesReaderBinary(const TimeSeriesReaderBinary &other) :
          definition_(other.definition_), alignment_(other.alignment_) {}

  virtual ~TimeSeriesReaderBinary() override {}

public:
  SharedTimeSeriesPtr Read(File &file, const int max_rows) override;

  void Write(File &file, TimeSeries &time_series) override;

public:
  static std::shared_ptr<TimeSeriesReaderBinary>
  make_shared(const BinaryFileDefinition &definition, size_t alignment = 0) {
    return std::make_shared<TimeSeriesReaderBinary>(definition, alignment);
  }

private:
  BinaryFileDefinition definition_;
  size_t alignment_;
};

#endif //TIME_SERIES_READER_H
