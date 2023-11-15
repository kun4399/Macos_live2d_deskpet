#ifndef LOG_UTIL_H
#define LOG_UTIL_H

#include <qdebug.h>

#ifdef CF_DEBUG
#define CF_LOG_DEBUG(fmt, ...) qDebug("[%s,%d] DEBUG:" fmt "\n",__PRETTY_FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define QF_LOG_DEBUG(fmt, ...)
#endif

#define CF_LOG_INFO(fmt, ...) qDebug("[%s,%d] INFO:" fmt "\n",__PRETTY_FUNCTION__,__LINE__,##__VA_ARGS__)
#define CF_LOG_ERROR(fmt, ...) qDebug("[%s,%d] ERROR:" fmt "\n",__PRETTY_FUNCTION__,__LINE__,##__VA_ARGS__)

#endif //  LOG_UTIL_H
