#ifndef ORDO_RUNNER_OPTION_WRAPPER_H
#define ORDO_RUNNER_OPTION_WRAPPER_H

#include <memory>

namespace ordo {
template<class T>
class RunnerOptionWrapper {
 public:
  template<typename... Args>
  static RunnerOptionWrapper<T> create(Args&... args) {
    return RunnerOptionWrapper(std::make_shared<T>(args...));
  }

  void operator()(mir::Server& server) {
    m_option->operator()(server);
  }

  const T& operator*() const {
    return *m_option;
  }

  T& operator*() {
    return *m_option;
  }

 private:
  std::shared_ptr<T> m_option;

  RunnerOptionWrapper(std::shared_ptr<T> option) : m_option{option} {}
};
} // namespace ordo


#endif // ORDO_RUNNER_OPTION_WRAPPER_H