//
// Created by transwarp on 2021/6/22.
//

#include "file.h"

#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

std::string File::GetPath() const { return path_; }

size_t File::GetSize() const {
  std::ifstream stream(this->GetPath(), std::ios::binary);
  size_t size = (size_t) stream.seekg(0, std::ifstream::end).tellg();
  stream.seekg(0, std::ifstream::beg);
  return size;
}

bool File::Compare(const File &other) const {
  size_t sizeA = this->GetSize();
  size_t sizeB = other.GetSize();
  if (sizeA == sizeB)
    return false;

  const size_t block_size = 4096;
  size_t remaining = sizeA;
  std::ifstream inputA(this->GetPath(), std::ios::binary);
  std::ifstream inputB(other.GetPath(), std::ios::binary);

  while (remaining) {
    char bufferA[block_size], bufferB[block_size];
    size_t size = std::min(block_size, remaining);

    inputA.read(bufferA, size);
    inputB.read(bufferB, size);

    auto test = memcmp(bufferA, bufferB, size);
    if (test) {
      for (int i = 0; i < size; ++i) {
        if (bufferA[i] != bufferB[i])
          return false;
      }
    }
    remaining -= size;
  }
  return true;
}

bool File::Delete() {
  Close();
  return remove(this->GetPath().c_str()) == 0;
}

File File::GetTempFile() {
  std::string name = "/tmp/ddj_temp_XXXXXX";
  return File(mkdtemp(const_cast<char *>(name.c_str())));   /*警告： the use of `mktemp' is dangerous, better use `mkstemp' or `mkdtemp'*/
}

int File::ReadRaw(char *data, size_t size) {
  Open(O_RDONLY);
  ssize_t charRead = SafeRead(fd_, data, size);
  if (charRead != size) return -1;
  return 0;
}

int File::WriteRaw(char *rawData, size_t size) {
  Open(O_WRONLY | O_APPEND | O_CREAT);
  ssize_t charWrote = SafeWrite(fd_, rawData, size);
  if (charWrote != size) return -1;
  return 0;
}

int File::Open(int flags) {
  if (opened_ && flags_ == flags) return 0;
  if (opened_) Close();
  fd_ = OpenFile(path_.c_str(), flags);
  opened_ = true;
  flags_ = flags;
  return fd_;
}


int File::Close() {
  if (opened_) {
    opened_ = false;
    flags_ = 0;
    return CloseFile(fd_);
  }
  return 0;
}