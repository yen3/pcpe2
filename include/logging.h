#pragma once

#include <fstream>
#include <sstream>
#include <string>

namespace pcpe {

enum class LoggingLevel {
  kNone = 0,
  kFatal = 1,
  kError = 2,
  kWarning = 3,
  kInfo = 4,
  kDebug = 5,
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

#define LOG(level) pcpe::LogMessage(level, __FILE__, __LINE__).stream()

#define LOG_FATAL() LOG(pcpe::LoggingLevel::kFatal)
#define LOG_ERROR() LOG(pcpe::LoggingLevel::kError)
#define LOG_WARNING() LOG(pcpe::LoggingLevel::kWarning)
#define LOG_INFO() LOG(pcpe::LoggingLevel::kInfo)
#define LOG_DEBUG() LOG(pcpe::LoggingLevel::kDebug)

}  // namespace pcpe
