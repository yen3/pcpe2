#include <gtest/gtest.h>

#include <string>
#include <fstream>

#include "logging.h"

namespace pcpe {

TEST(logging, test_log_message) {
  InitLogging("testoutput/log.txt", LoggingLevel::kDebug);

  std::string test_str("test");
  LogMessage lm(LoggingLevel::kInfo, "test_logging.cc", 0);
  lm.stream() << test_str << test_str << "\n";

  ASSERT_EQ(test_str + test_str + std::string("\n"), lm.stream().str());
}

TEST(logging, test_log_file) {
  // Close the previous log file
  InitLogging("testoutput/log2.txt", LoggingLevel::kDebug);

  // Test the file content is the same as the test_str
  std::ifstream in("testoutput/log.txt", std::ifstream::in);
  std::string log_result;
  std::getline(in, log_result);

  ASSERT_EQ(std::string("[test_logging.cc:0]: testtest"), log_result);

  LOG_INFO() << "Just for test" << std::endl;
}

TEST(logging, test_macro) {
  // Close the previous log file
  InitLogging("testoutput/log3.txt", LoggingLevel::kDebug);

  // Test the file content is the same as the test_str
  std::ifstream in("testoutput/log2.txt", std::ifstream::in);
  std::string log_result;
  std::getline(in, log_result);

  ASSERT_NE(log_result.find("Just for test"), std::string::npos);
}

TEST(logging, clear_the_environment) {
  InitLogging("testoutput/log.txt", LoggingLevel::kDebug);
}

} // namespace pcpe
