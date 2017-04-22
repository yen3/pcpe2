#include <gtest/gtest.h>

#include <unistd.h>

#include <iostream>
#include <cstdint>

#include "simple_task.h"
#include "logging.h"

namespace pcpe {

class AssignIntTask {
 public:
  explicit AssignIntTask(uint32_t tid): task_id(tid), result_id(0) {}

  void exec() {
    usleep(100); // Make sure not only one thread to run the task.
    result_id = task_id;
  }

  uint32_t task_id;
  uint32_t result_id;
};

TEST(simple_task, basic_usage) {
  InitLogging(LoggingLevel::kDebug);

  std::vector<AssignIntTask*> tasks(100, nullptr);

  for (uint32_t i = 0; i < tasks.size(); ++i)
    tasks[i] = new AssignIntTask(i+1);

  RunSimpleTasks(tasks);

  for (auto task : tasks)
    ASSERT_EQ(task->task_id, task->result_id);

  for (auto& task : tasks) {
    delete task;
    task = nullptr;
  }
}

} // namespace pcpe
