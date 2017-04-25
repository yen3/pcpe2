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
#include <memory>

#include "logging.h"
#include "env.h"

namespace pcpe {

/**
 * Run all tasks `exec()` function in parallel.
 *
 * @param[in] tasks The list of tasks. The `Type` is pointer type. It could be
 *                  raw pointer, shared pointer or unique pointer.
 *
 * */
template<typename Type, typename AllocType,
  template <typename, typename> class ContainerType>
void RunSimpleTasks(ContainerType<Type, AllocType>& tasks);

/**
 * Generate all steps to the number.
 *
 * Example:
 *  Given n = 100, step = 10. The output is
 *  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100}
 *
 * @param[in] n The end of the number
 * @param[in] step The difference of each step
 * @param[out] steps The steps from 0 to n
 *
 * */
void GetStepsToNumber(const std::size_t n,
                      const std::size_t step,
                      std::vector<std::size_t>& steps);


template<typename Type, typename AllocType,
  template <typename, typename> class ContainerType>
void RunSimpleTasksInternal(ContainerType<Type, AllocType>& tasks,
    std::atomic_uint& curr_max_index) {

  using SizeType = typename ContainerType<Type, AllocType>::size_type;
  using PointerType = typename std::pointer_traits<Type>::pointer;

  SizeType tasks_size = tasks.size();
  SizeType curr_index = 0;

  while ((curr_index = curr_max_index.fetch_add(1)) < tasks_size) {
    PointerType& task = tasks[curr_index];

    if (task == nullptr) {
      LOG_WARNING() << "The job " << curr_index << " is empty."
                    << std::endl;
      continue;
    }

    task->exec();
  }
}

template<typename Type, typename AllocType,
  template <typename, typename> class ContainerType>
void RunSimpleTasks(ContainerType<Type, AllocType>& tasks) {
  std::vector<std::thread> ts(gEnv.getThreadsSize());
  std::atomic_uint task_index(0);

  for (auto& t: ts)
    t = std::thread(RunSimpleTasksInternal<Type, AllocType, ContainerType>,
                    std::ref(tasks),
                    std::ref(task_index));

  for (auto& t: ts)
    t.join();
}

} // namespace pcpe
