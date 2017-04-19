#pragma once

#include <string>
#include <fstream>
#include <sstream>

namespace pcpe {

enum class LoggingLevel {
  kNone = 0,
  kFatal = 1,
  kError = 2,
  kInfo = 3,
  kDebug = 4,
};

class LogMessage {
 public:
  LogMessage(LoggingLevel level, const char* filename, uint32_t lineno);
  ~LogMessage();
  std::ostringstream& stream();

 private:
  LoggingLevel level_;
  std::string filename_;
  uint32_t line_;
  std::ostringstream out_;
};

void InitLogging(LoggingLevel default_level);
void InitLogging(const std::string& filename, LoggingLevel default_level);
void InitLogging(const char* filename, LoggingLevel default_level);

#define LOG(level) LogMessage(level, __FILE__, __LINE__).stream()

#define LOG_FATAL() LOG(LoggingLevel::kFatal)
#define LOG_ERROR() LOG(LoggingLevel::kError)
#define LOG_INFO() LOG(LoggingLevel::kInfo)
#define LOG_DEBUG() LOG(LoggingLevel::kDebug)

} // namespace pcpe

