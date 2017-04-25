#include "logging.h"
#include "simple_task.h"

namespace pcpe {

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
