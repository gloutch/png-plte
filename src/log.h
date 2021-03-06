/**
 * @file log.h
 * @brief Log levels and log functions
 * @details Log functions are actually macros defined at compile time thanks to LOG_LEVEL
 */

#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>


/*
 * Log values
 */

/** @brief level logeverything */
#define ALL    (0)
/** @brief every details (basically a printf in log) */
#define TRACE  (1)
/** @brief diagnostic, program data like pointers */
#define DEBUG  (2)
/** @brief special for dynamic memory and free */
#define ALLOC  (3)
/** @brief normal behavior and milestones */ 
#define INFO   (4)
/** @brief what might be a problem */
#define WARN   (5)
/** @brief error occurs but the program limps along */
#define ERROR  (6)
/** @brief juste before aborting */
#define FATAL  (7)
/** @brief quiet, no log at all */
#define NONE   (8)

/** @brief default log level */
#ifndef LOG_LEVEL
  #define LOG_LEVEL NONE
#endif

/** @brief prefix format for one liner log */
#define LOG_PREFIX(level) (printf("%-5s %s %d %s() \t ", level, __FILE__, __LINE__, __func__))


/*
 * Log functions
 * Cannot find a way to factorize this
 */

/** @brief compile trace log macro */
#if (LOG_LEVEL <= TRACE)
  #define LOG_TRACE(fmt, ...) LOG_PREFIX("TRACE"); printf((fmt), ##__VA_ARGS__); puts("")
#else
  #define LOG_TRACE(...)
#endif

/** @brief compiled debug log macro */
#if (LOG_LEVEL <= DEBUG)
  #define LOG_DEBUG(fmt, ...) LOG_PREFIX("DEBUG"); printf((fmt), ##__VA_ARGS__); puts("")
#else
  #define LOG_DEBUG(...)
#endif

/** @brief compiled debug log macro */
#if (LOG_LEVEL <= ALLOC)
  #define LOG_ALLOC(fmt, ...) LOG_PREFIX("ALLOC"); printf((fmt), ##__VA_ARGS__); puts("")
#else
  #define LOG_ALLOC(...)
#endif
  
/** @brief compiled info log macro */
#if (LOG_LEVEL <= INFO)
  #define LOG_INFO(fmt, ...) LOG_PREFIX("INFO"); printf((fmt), ##__VA_ARGS__); puts("")
#else
  #define LOG_INFO(...)
#endif

/** @brief compiled warning log macro */
#if (LOG_LEVEL <= WARN)
  #define LOG_WARN(fmt, ...) LOG_PREFIX("WARN"); printf((fmt), ##__VA_ARGS__); puts("")
#else
  #define LOG_WARN(...)
#endif

/** @brief compiled error log macro */
#if (LOG_LEVEL <= ERROR)
  #define LOG_ERROR(fmt, ...) LOG_PREFIX("ERROR"); printf((fmt), ##__VA_ARGS__); puts("")
#else
  #define LOG_ERROR(...)
#endif

/** @brief compiled fatal log macro */
#if (LOG_LEVEL <= FATAL)
  #define LOG_FATAL(fmt, ...) LOG_PREFIX("FATAL"); printf((fmt), ##__VA_ARGS__); puts("")
#else
  #define LOG_FATAL(...)
#endif


/**
 * @brief Log the LOG_LEVEL compiled with (as non classic log)
 */
#if (LOG_LEVEL == ALL)
  #define LOG_LOG_LEVEL() puts("Log level  ALL")

#elif (LOG_LEVEL == TRACE)
  #define LOG_LOG_LEVEL() puts("Log level  TRACE")

#elif (LOG_LEVEL == DEBUG)
  #define LOG_LOG_LEVEL() puts("Log level  DEBUG")

#elif (LOG_LEVEL == ALLOC)
  #define LOG_LOG_LEVEL() puts("Log level  ALLOC")

#elif (LOG_LEVEL == INFO)
  #define LOG_LOG_LEVEL() puts("Log level  INFO")

#elif (LOG_LEVEL == WARN)
  #define LOG_LOG_LEVEL() puts("Log level  WARNING")

#elif (LOG_LEVEL == ERROR)
  #define LOG_LOG_LEVEL() puts("Log level  ERROR")

#elif (LOG_LEVEL == FATAL)
  #define LOG_LOG_LEVEL() puts("Log level  FATAL")

#else // LOG_LEVEL == NONE
  #define LOG_LOG_LEVEL()
#endif
  


#endif // __LOG_H__
