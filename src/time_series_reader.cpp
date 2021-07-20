//
// Created by transwarp on 2021/6/22.
//

#include "time_series_reader.h"
#include <iomanip>
#include <boost/lexical_cast.hpp>

std::vector<std::string> ReadHeader(std::ifstream &in_file, CsvFileDefinition &definition) {
  std::vector<std::string> result;
  std::string header_line;
  std::getline(in_file, header_line);
  size_t position = 0;
  do {
    position = header_line.find(definition.Separator);
    auto col_name = header_line.substr(0, position);
    header_line.erase(0, position + definition.Separator.length());
    result.push_back(col_name);
  } while (std::string::npos != position);
  definition.Header = result;
  return result;
}

SharedTimeSeriesPtr TimeSeriesReaderCSV::Read(File &file, const int max_rows) {
  /* Initialize time series */
  auto result = std::make_shared<TimeSeries>(file.GetPath());
  result->init(definition_.Columns);
  result->SetDecimals(definition_.Decimals);

  /* Open file and set last position */
  std::ifstream input_file(file.GetPath(), std::ios::in);
  input_file.seekg(last_file_position_, input_file.beg);

  /* Read header */
  std::vector<std::string> header;
  if (definition_.HasHeader && last_file_position_ == 0) {
    header = ReadHeader(input_file, definition_);
  } else {
    header = definition_.Header;
  }
  result->SetColumnNames(header);

  std::string line, token;
  size_t position = 0;
  int count = 0;
  while ((count++ < max_rows) && std::getline(input_file, line)) {
    std::vector<std::string> record;
    do {
      position = line.find(definition_.Separator);
      token = line.substr(0, position);
      line.erase(0, position + definition_.Separator.length());
      record.push_back(token);
    } while (position != std::string::npos);
    result->AddRecord(record);
    record.clear();
  }
  last_file_position_ = input_file.tellg();
  input_file.close();

  return result;
}

void WriteLine(std::ofstream &ofstm,
               std::vector<std::string> line_items,
               CsvFileDefinition &definition) {
  std::string line;
  for (int i = 0; i < line_items.size(); ++i) {
    line += line_items[i];
    if (i < line_items.size() - 1)
      line += definition.Separator;
  }
  ofstm << line << std::endl;
}

void TimeSeriesReaderCSV::Write(File &file, TimeSeries &time_series) {
  std::ofstream out_file(file.GetPath(), std::ios::app);
  /* Write header as columns names */
  if (definition_.HasHeader) {
    WriteLine(out_file, time_series.GetColumnNames(), definition_);
    definition_.HasHeader = false;
  }
  for (size_t i = 0; i < time_series.GetRecordsCount(); ++i) {
    WriteLine(out_file, time_series.GetRecordAsStrings(i), definition_);
  }
  out_file.close();
}

void TimeSeriesReader::WriteFileDefinition(File &file, FileDefinition &definition) {
  std::ofstream out_file(file.GetPath(), std::ios::out);

  for (int i = 0; i < definition.Columns.size(); ++i) {
    out_file << definition.Header[i] << ',';
    out_file << GetDataTypeSize(definition.Columns[i]) << ',';
    if (definition.Decimals.size() > i) {
      out_file << definition.Decimals[i];
    } else {
      out_file << "0";
    }
  }
  out_file.close();
}

FileDefinition TimeSeriesReader::ReadFileDefinition(File &file) {
  FileDefinition result;
  std::ifstream input_file(file.GetPath(), std::ios::in);
  std::string line, name, type, decimal;
  EnumParser<DataType> type_parser;

  size_t position = 0;

  while (std::getline(input_file, line)) {
    /* Get Name */
    position = line.find(',');
    name = line.substr(0, position);
    line.erase(0, position + 1);

    /* Get Type */
    position = line.find(',');
    type = line.substr(0, position);
    line.erase(0, position + 1);

    // GET DECIMAL
    decimal = line;

    result.Header.push_back(name);
    result.Columns.push_back(type_parser.Parse(type));
    result.Decimals.push_back(boost::lexical_cast<int>(decimal));
  }
  input_file.close();
  return result;
}

SharedTimeSeriesPtr TimeSeriesReaderBinary::Read(File &file, const int max_rows) {
  /* Initialize time series */
  auto result = std::make_shared<TimeSeries>(file.GetPath());
  result->init(definition_.Columns);
  result->SetDecimals(definition_.Decimals);

  size_t size = result->GetRecordSize();
  size += alignment_;
  result->SetColumnNames(definition_.Header);

  char *data = new char[size];
  int count = 0;
  while ((count++ <= max_rows) && (-1 != file.ReadRaw(data, size))) {
    result->AddRecord(data);
  }

  delete[] data;
  return result;
}

void TimeSeriesReaderBinary::Write(File &file, TimeSeries &time_series) {
  size_t size = time_series.GetRecordSize() + alignment_;
  char *data = new char[size];

  for (size_t i = 0; i < time_series.GetRecordsCount(); ++i) {
    size_t offset = 0;
    memset(data, 0, size);
    for (auto &raw_data:time_series.GetRawRecordData(i)) {
      memcpy(data + offset, raw_data.raw_data_, raw_data.raw_size_);
      offset += raw_data.raw_size_;
    }
    if (file.WriteRaw(data, size)) {
      throw std::runtime_error("Error while writing to a file.");
    }
  }

  delete[]data;
}
