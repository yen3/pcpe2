#include <gtest/gtest.h>

#include <unistd.h>

#include <iostream>
#include <algorithm>
#include <memory>
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

TEST(simple_task, basic_useage_unique_ptr) {
  std::vector<std::unique_ptr<AssignIntTask>> tasks;

  for (uint32_t i = 0; i < 100; ++i)
    tasks.push_back(std::unique_ptr<AssignIntTask>(new AssignIntTask(i+1)));

  RunSimpleTasks(tasks);

  for (const auto& task : tasks)
    ASSERT_EQ(task->task_id, task->result_id);
}

TEST(simple_task, basic_useage_shared_ptr) {
  std::vector<std::shared_ptr<AssignIntTask>> tasks;

  for (uint32_t i = 0; i < 100; ++i)
    tasks.push_back(std::make_shared<AssignIntTask>(i+1));

  RunSimpleTasks(tasks);

  for (const auto& task : tasks)
    ASSERT_EQ(task->task_id, task->result_id);
}

TEST(simple_task, GetStepsToNumberRegular_less_than) {
  {
    std::vector<std::size_t> steps;
    GetStepsToNumber(10, 10, steps);

    ASSERT_EQ(steps.size(), 2);
    ASSERT_EQ(steps[0], 0);
    ASSERT_EQ(steps[1], 10);

  }

  {
    std::vector<std::size_t> steps;
    GetStepsToNumber(7, 10, steps);

    ASSERT_EQ(steps.size(), 2);
    ASSERT_EQ(steps[0], 0);
    ASSERT_EQ(steps[1], 7);
  }
}

TEST(simple_task, GetStepsToNumberRegular) {
  {
    std::vector<std::size_t> steps;
    GetStepsToNumber(100, 10, steps);

    std::vector<std::size_t> ans{0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    ASSERT_EQ(steps.size(), ans.size());

    for (std::size_t i = 0; i < ans.size(); ++i)
      ASSERT_EQ(steps[i], ans[i]);
  }
  {
    std::vector<std::size_t> steps;
    GetStepsToNumber(10, 1, steps);

    std::vector<std::size_t> ans{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    ASSERT_EQ(steps.size(), ans.size());

    for (std::size_t i = 0; i < ans.size(); ++i)
      ASSERT_EQ(steps[i], ans[i]);
  }
}

} // namespace pcpe
