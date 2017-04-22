#pragma once

/**
 * The purpose of the RunSimpleTasks is to keep eyerything as simple as
 * possible.  The project is not a large project so just to implement a simple
 * thread pool to execute all tasks.
 * */

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

#include <logging.h>

namespace pcpe {

/**
 * Run all tasks `exec()` function in parallel.
 *
 * @param[in] tasks The list of tasks
 *
 * */
template<typename Task>
void RunSimpleTasks(std::vector<Task*>& tasks);

template<typename Task>
void RunSimpleTasksInternal(
    std::vector<Task*>& tasks,
    std::atomic_uint& curr_max_index);

template<typename Task>
void RunSimpleTasksInternal(
    std::vector<Task*>& tasks,
    std::atomic_uint& curr_max_index) {

  std::size_t tasks_size = tasks.size();
  std::size_t curr_index = 0;

  while ((curr_index = curr_max_index.fetch_add(1)) < tasks_size) {
    Task* task = tasks[curr_index];
    task->exec();
  }
}

template<typename Task>
void RunSimpleTasks(std::vector<Task*>& tasks) {
  std::vector<std::thread> ts(std::thread::hardware_concurrency());
  std::atomic_uint task_index(0);

  for (auto& t: ts)
    t = std::thread(RunSimpleTasksInternal<Task>,
                    std::ref(tasks),
                    std::ref(task_index));

  for (auto& t: ts)
    t.join();
}

} // namespace pcpe
