//
// Created by transwarp on 2021/6/23.
//

#ifndef ERRORS_H
#define ERRORS_H
#include <csignal>
#include <cstdio>
#include <cstring>
#include <cerrno>

#ifndef FILE_ERR
#define FILE_ERR(source) (fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
perror(source), kill(0, SIGKILL),                                         \
exit(EXIT_FAILURE))
#endif

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) \
({                                     \
long int result_;                      \
do result_ = (long int) (expression);  \
while(result_ == -1L && errno == EINTR); \
result_;                               \
})
#endif

#ifndef CHECK_ERROR
#define CHECK_ERROR(expression) \
({                              \
if (-1 == expression){          \
fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); \
kill(0, SIGKILL);               \
exit(EXIT_FAILURE);             \
})
#endif

#ifndef TEMP_FAILURE_RETRY_WHEN_NULL
#define TEMP_FAILURE_RETRY_WHEN_NULL(expression) \
({ \
  void* _result; \
  do _result = (void*) (expression); \
  while (_result == NULL && errno == EINTR); \
  _result; \
})
#endif

#ifndef CHECK_ERROR_WHEN_NULL
#define CHECK_ERROR_WHEN_NULL(expression) \
({ \
  if(NULL == (void*)(expression) ){ \
    fprintf(stderr,"%s:%d\n",__FILE__,__LINE__); \
    kill(0,SIGKILL); \
    exit(EXIT_FAILURE);                     \
  } \
})
#endif

#endif //ERRORS_H
