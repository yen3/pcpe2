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


/**
 * Generate step numbers in the range from 0 to n .
 *
 * Example:
 *  Given n = 100, step_size = 10. The output is
 *  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100}
 *
 * @param[in] n The end of the number
 * @param[in] step_size the number of steps
 * @param[out] steps The steps from 0 to n
 *
 * */
void GetNumberSteps(const std::size_t n,
                    const std::size_t step_size,
                    std::vector<std::size_t>& steps);


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

    if (task == nullptr) {
      LOG_WARNING() << "The job " << curr_index << " is empty."
                    << std::endl;
      continue;
    }

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
