//
// Created by transwarp on 2021/6/22.
//

#include "time_series_reader.h"
#include <gtest/gtest.h>

TimeSeries GenerateFakeTimeSeries(const std::string &name, int N) {
  TimeSeries result(name);
  result.init(std::vector<DataType>{DataType::d_time, DataType::d_float});
  result.SetColumnNames(std::vector<std::string>{"time", "value"});

  size_t size = result.GetRecordSize();
  char *data = new char[size];

  for (time_t i = 0; i < N; ++i) {
    float value = (float) i * (i % 3);
    memcpy(data, &i, sizeof(time_t));
    memcpy(data + sizeof(time_t), &value, sizeof(float));
    result.AddRecord(data);
  }
  return result;
}
/*
 * class TimeSeriesTest_ReadWrite_CSV_Data_ToFile_Test : public ::testing::Test {
     public:
      TimeSeriesTest_ReadWrite_CSV_Data_ToFile_Test() {}
     private:
      virtual void TestBody();
      static ::testing::TestInfo *const test_info_ __attribute__((unused));
      TimeSeriesTest_ReadWrite_CSV_Data_ToFile_Test(TimeSeriesTest_ReadWrite_CSV_Data_ToFile_Test const &);
      void operator=(TimeSeriesTest_ReadWrite_CSV_Data_ToFile_Test const &);
    };
    ::testing::TestInfo *const TimeSeriesTest_ReadWrite_CSV_Data_ToFile_Test::test_info_ =
    ::testing::internal::MakeAndRegisterTestInfo("TimeSeriesTest",
                                                   "ReadWrite_CSV_Data_ToFile",
                                                   __null,
                                                   __null,
                                                   ::testing::internal::CodeLocation("_file_name_", 25),
                                                   (::testing::internal::GetTestTypeId()),
                                                   ::testing::Test::SetUpTestCase,
                                                   ::testing::Test::TearDownTestCase,
                                                   new ::testing::internal::TestFactoryImpl<
                                                       TimeSeriesTest_ReadWrite_CSV_Data_ToFile_Test>);
void TimeSeriesTest_ReadWrite_CSV_Data_ToFile_Test::TestBody()
 */
TEST(TimeSeriesTest, ReadWrite_CSV_Data_ToFile) {
  auto test_file = File::GetTempFile();
  TimeSeries fake = GenerateFakeTimeSeries("fake", 1000);
  CsvFileDefinition file_definition;
  file_definition.Header = std::vector<std::string>{"time", "value"};
  file_definition.Columns = std::vector<DataType>{DataType::d_time, DataType::d_float};
  TimeSeriesReaderCSV reader_csv(file_definition);

  reader_csv.Write(test_file, fake);
  auto result = reader_csv.Read(test_file, INT32_MAX);
  EXPECT_TRUE(result->Compare(fake));
  test_file.Delete();
}

TEST(TimeSeriesTest, ReadWrite_CSV_Data_FromFile) {
  File realDataFile("../test/data/info.log");
  File testFile = File::GetTempFile();
  File testFile2 = File::GetTempFile();

  CsvFileDefinition file_definition;
  file_definition.Columns = std::vector<DataType>{
      DataType::d_time,
      DataType::d_float,
      DataType::d_float,
      DataType::d_float
  };

  auto data = TimeSeriesReaderCSV(file_definition).Read(realDataFile, INT32_MAX);
  TimeSeriesReaderCSV(file_definition).Write(testFile, *data);
  auto data2 = TimeSeriesReaderCSV(file_definition).Read(testFile, INT32_MAX);
  TimeSeriesReaderCSV(file_definition).Write(testFile2, *data2);
  EXPECT_TRUE(testFile.Compare(testFile2));
  testFile.Delete();
  testFile2.Delete();
}

TEST(TimeSeries, ReadWrite_Binary_Data_ToFile) {
  auto test_file = File::GetTempFile();
  TimeSeries fake = GenerateFakeTimeSeries("fake", 1000);

  BinaryFileDefinition fileDefinition;
  fileDefinition.Header = std::vector<std::string>{"time", "value"};
  fileDefinition.Columns = std::vector<DataType>{DataType::d_time, DataType::d_float};
  TimeSeriesReaderBinary(fileDefinition).Write(test_file, fake);
  auto result = TimeSeriesReaderBinary(fileDefinition).Read(test_file, INT32_MAX);
  EXPECT_TRUE(result->Compare(fake));
  test_file.Delete();
}

TEST(TimeSeries, Read_CSV_Data_InMultipleParts_FromFile_CheckWithOnePartRead) {
  File read_data_file("../test/data/info.log");
  CsvFileDefinition file_definition;
  file_definition.Columns = std::vector<DataType>{
      DataType::d_time,
      DataType::d_float,
      DataType::d_float,
      DataType::d_float
  };

  /* read whole data 1000 rows */
  int all_rows = 1000;
  auto whole_data = TimeSeriesReaderCSV(file_definition).Read(read_data_file, all_rows);

  /* compare with reading 5 times 200 rows */
  int part_no = 5;
  int part_row_cnt = all_rows / part_no;
  TimeSeriesReaderCSV reader_csv(file_definition);
  for (int i = 0; i < part_no; ++i) {
    auto part_data = reader_csv.Read(read_data_file, part_row_cnt);
    EXPECT_EQ(part_data->GetRecordAsStrings(0)[0], whole_data->GetRecordAsStrings(part_row_cnt * i)[0]);
  }

}