//
// Created by transwarp on 2021/6/23.
//

#ifndef PARSE_H
#define PARSE_H

#include <sstream>

#ifdef DDJ_TIME_SERIES_READER_USE_BOOST
#include <boost/lexical_cast.hpp>
#endif

#ifndef DDJ_TIME_SERIES_READER_USE_BOOST

template<typename T>
T ParseString(const std::string &value) {
  std::istringstream stream(value);
  T result;
  stream >> result;
  return result;
}

#else
template<typename T>
T ParseString(std::string value)
{
    return boost::lexical_cast<T>(value);
}
#endif

#ifndef DDJ_TIME_SERIES_READER_USE_BOOST

template<typename T>
std::string ParseType(const T &value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

#else
template<typename T>
std::string ParseType(T value)
{
    return boost::lexical_cast<std::string>(value);
}
#endif

#endif //PARSE_H
