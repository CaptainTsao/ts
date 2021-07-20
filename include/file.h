//
// Created by transwarp on 2021/6/23.
//

#ifndef FILE_H
#define FILE_H

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include <dirent.h>

#if defined(WIN32)
#  define DIR_SEPARATOR '\\'
#else
#  define DIR_SEPARATOR '/'
#endif

class File {
public:
  File(const char *path) : path_(path), opened_(false) {}

  File(std::string path) : path_(std::move(path)), opened_(false) {}

  ~File() { Close(); }

  File(const File &other) : path_(other.path_), opened_(false) {}

  File(File &&other) : path_(std::move(other.path_)), opened_(false) {}

public:
  static File GetTempFile();

public:
  size_t GetSize() const;

  std::string GetPath() const;

  bool Compare(const File &other) const;

  bool Delete();

  int ReadRaw(char *rawData, size_t size);

  int WriteRaw(char *rawData, size_t size);

private:
  int Open(int flags);

  int Close();

private:
  static char *GetCurrentPath() ;

  static int OpenFile(const char *path, int flags) ;

  static int CreateDir(char *nameOfDirectory) ;

  static int CreateFile(char *nameOfFile) ;

  static int CloseFile(int fileDescriptor) ;

  int SetFileSizeWithTruncate(int fileDescriptor, off_t size) const;

  int SetFileSizeWithLseek(int fileDescriptor, off_t size) const;

  int CreateFileWithSizeUsingTruncate(char *nameOfFile, off_t size) const;

  int CreateFileWithSizeUsingLseek(char *nameOfFile, off_t size) const;

  int CreateSymbolicLink(char *nameOfSymbolicLink, char *pathToFile) const;

  int ChangeDirectory(char *path) const;

  static int SafeRead(int fd, char *buf, size_t size) ;

  static ssize_t SafeWrite(int fd, char *buf, size_t size) ;

  static ssize_t SafeWriteLine(int fd, char *buf, size_t size) ;

  char *ReadLine(int fd) const;

  static FILE *OpenStream(char *fileName, char *mode) ;

  static int CloseStream(FILE *file) ;

  DIR *OpenDirectory(char *path) const;

  int CloseDirectory(DIR *dirp) const;

  struct dirent *ReadDirectory(DIR *dirp) const;

  void PrintFileStats(struct dirent *dp) const;

  int LockWholeFile(int fileDesc) const;

  int UnlockWholeFile(int fileDesc) const;

  char *ReadLineFromStream(FILE *file) const;

  struct dirent *GetFileDirent(char *path, char *name) const;

  static char *AllocCombinedPath(const char *path1, const char *path2) ;

  static void CombinePaths(char *destination, const char *path1, const char *path2) ;

  int CreateDirInPathAndBackToCurrentDir(char *dirName, char *path, char *currentDir) const;

  int CreateDirInPath(char *dirName, char *path) const;

  static int CreateDirIfNotExists(char *dirName) ;

private:
  std::string path_;
  bool opened_;
  int flags_;
  int fd_;
};

#endif //TIME_SERIES_FILE_H
