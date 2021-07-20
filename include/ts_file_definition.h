//
// Created by transwarp on 2021/6/23.
//

#ifndef TS_FILE_DEFINITION_H
#define TS_FILE_DEFINITION_H

#include <vector>
#include <string>

struct FileDefinition {
  std::vector<std::string> Header;
  std::vector<DataType> Columns;
  std::vector<int> Decimals;
};

struct CsvFileDefinition : public FileDefinition {
  bool HasHeader = true;
  std::string Separator = ",";

  CsvFileDefinition() = default;

  ~CsvFileDefinition() = default;

  CsvFileDefinition(const CsvFileDefinition &) = default;

  explicit CsvFileDefinition(const FileDefinition &def) : FileDefinition(def) {}
};

struct BinaryFileDefinition : public FileDefinition {
  BinaryFileDefinition() = default;

  ~BinaryFileDefinition() = default;

  BinaryFileDefinition(const BinaryFileDefinition &) = default;

  explicit BinaryFileDefinition(const FileDefinition &def) : FileDefinition(def) {}
};

#endif // TS_FILE_DEFINITION_H
