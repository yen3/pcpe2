#include "logging.h"

#include <iostream>
#include <memory>
#include <mutex>

namespace pcpe {

class LogObject {
public:
    virtual ~LogObject(){}
    virtual std::ostream& stream() = 0;
    virtual void close() = 0;
};

class LogStdErrObject : public LogObject {
public:
    LogStdErrObject(): LogObject(){}
    virtual ~LogStdErrObject(){}
    virtual std::ostream& stream() {  return std::cerr; }
    virtual void close() {}
};

class LogFileObject : public LogObject {
public:
    LogFileObject(const char* filename):
        LogObject(), out_(filename, std::ofstream::out), filename_(filename) {
    }
    virtual ~LogFileObject(){
        close();
    }
    virtual std::ostream& stream() { return out_; }
    virtual void close() {
      out_.flush();
      out_.close();
    }

private:
    std::ofstream out_;
    const std::string filename_;
};

class LogRecorder {
public:
  LogRecorder():
    out_obj_(nullptr), filter_level_(LoggingLevel::kInfo), mutex_() {
  }

  void initStdErr(LoggingLevel level=LoggingLevel::kInfo) {
    LogObject* obj = new LogStdErrObject();
    initInternal(obj, level);
  }

  void initFile(const char* filename, LoggingLevel level=LoggingLevel::kInfo) {
    LogObject* obj = new LogFileObject(filename);
    initInternal(obj, level);
  }

  void initNone() {
    initInternal(nullptr, LoggingLevel::kNone);
  }

  virtual ~LogRecorder(){
    if (out_obj_ != nullptr) {
      out_obj_->close();
      delete out_obj_;
    }
  }

  virtual void record(LoggingLevel level, const char* filename,
                      uint32_t lineno, const char* msg);

private:
  void initInternal(LogObject* out_obj, LoggingLevel level) {
    if (out_obj_ != nullptr) {
        out_obj_->close();
        delete out_obj_;
        out_obj_ = nullptr;
    }

    out_obj_ = out_obj;
    filter_level_ = level;
  }

  LogObject* out_obj_;
  LoggingLevel filter_level_;
  std::mutex mutex_;
};

namespace {
    LogRecorder gLogRecorder;
}

static void WriteLog(LoggingLevel level,
                     const char* filename,
                     uint32_t lineno,
                     const char* msg) {
    gLogRecorder.record(level, filename, lineno, msg);
}

void InitLogging(LoggingLevel default_level) {
    gLogRecorder.initStdErr(default_level);
}

void InitLogging(const std::string& filename,
                 LoggingLevel default_level) {
    gLogRecorder.initFile(filename.c_str(), default_level);
}

void InitLogging(const char* filename,
                 LoggingLevel default_level) {
    gLogRecorder.initFile(filename, default_level);
}

LogMessage::LogMessage(LoggingLevel level,
                       const char* filename,
                       uint32_t lineno):
    level_(level), filename_(filename), line_(lineno), out_() {
}

LogMessage::~LogMessage() {
    WriteLog(level_, filename_.c_str(), line_, out_.str().c_str());
}

std::ostringstream& LogMessage::stream() {
    return out_;
}

void LogRecorder::record(LoggingLevel level,
                         const char* filename,
                         uint32_t lineno,
                         const char* msg)
{
    if (level > filter_level_)
      return;

    if (out_obj_ != nullptr) {
      std::lock_guard<std::mutex> lock(mutex_);
      out_obj_->stream() << "[" << filename << ":" << lineno << "]: " << msg;
    }
}

} // namespace pcpe

