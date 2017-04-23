#include "logging.h"
#include "simple_task.h"

namespace pcpe {

void GetNumberSteps(const std::size_t n,
                    const std::size_t step_size,
                    std::vector<std::size_t>& steps) {

  if (n == 0) {
    LOG_WARNING() << "The end step is 0." << std::endl;
    return;
  }

  if (step_size > n) {
    for (std::size_t i = 0; i <= n; ++i)
      steps.push_back(i);

    return;
  }
  else {
    std::vector<std::size_t> diffs(step_size, n / step_size);
    std::size_t remain_steps = n % step_size;
    for (std::size_t i = 0; i < remain_steps; ++i)
      diffs[i] += 1;

    for (std::size_t acc = 0, i = 0; i < diffs.size(); ++i) {
      steps.push_back(acc);
      acc += diffs[i];
    }
    steps.push_back(n);
  }
}

void GetStepsToNumber(const std::size_t n,
                      const std::size_t step,
                      std::vector<std::size_t>& steps) {
  if (n == 0) {
    LOG_WARNING() << "The end step is 0." << std::endl;
    return;
  }

  for (std::size_t curr_step = 0; curr_step < n; curr_step += step)
    steps.push_back(curr_step);

  steps.push_back(n);
}

} // namespace pcpe
