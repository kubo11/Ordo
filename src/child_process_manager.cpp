#include "ordo/child_process_manager.h"

#include <sys/wait.h>
#include <errno.h>

namespace ordo {
ChildProcessManager::ChildProcessManager(MirRunner& runner) : m_runner{runner}, m_launcher{}, m_privilaged_children_pids{} {
  runner.add_start_callback([&] { runner.register_signal_handler({SIGCHLD}, std::bind(&ChildProcessManager::remove_exited_privilaged_children_pids, this)); });
  runner.add_stop_callback(std::bind(&ChildProcessManager::shutdown_pivilaged_children, this));
}

ChildProcessManager::~ChildProcessManager() = default;

void ChildProcessManager::operator()(mir::Server& server) {
  m_launcher(server);
}

void ChildProcessManager::run(const std::string& cmd) {
  m_launcher.launch(cmd);
}

void ChildProcessManager::run_privileged(const std::string& cmd) {
  add_privilaged(m_launcher.launch(cmd));
}

void ChildProcessManager::enable_extension_for_privilaged(WaylandExtensions& extensions, std::string const& protocol) {
  extensions.conditionally_enable(protocol, std::bind(&ChildProcessManager::should_enable_extension_for_privilaged_callback, this, std::placeholders::_1));
}

bool ChildProcessManager::should_enable_extension_for_privilaged_callback(const WaylandExtensions::EnableInfo& info) {
  auto pid = pid_of(info.app());
  return is_privilaged(pid) || info.user_preference().value_or(false);
}

bool ChildProcessManager::is_privilaged(pid_t pid) {
  std::lock_guard lock{m_privilaged_children_pids_mtx};
  return std::find(m_privilaged_children_pids.begin(), m_privilaged_children_pids.end(), pid) != m_privilaged_children_pids.end();
}

void ChildProcessManager::add_privilaged(pid_t pid) {
  std::lock_guard lock{m_privilaged_children_pids_mtx};
  m_privilaged_children_pids.push_back(pid);
}

void ChildProcessManager::shutdown_pivilaged_children() {
  std::lock_guard lock{m_privilaged_children_pids_mtx};
  for (const auto& pid : m_privilaged_children_pids) {
    kill(pid, SIGTERM);
  }
  m_privilaged_children_pids.clear();
}

void ChildProcessManager::remove_exited_privilaged_children_pids() {
  std::vector<pid_t> active_children{};
  int status = 0;
  std::lock_guard lock{m_privilaged_children_pids_mtx};
  for (const auto& pid : m_privilaged_children_pids) {
    auto res = waitpid(pid, &status, WNOHANG);
    if (res == 0 && !(WIFEXITED(status) || WIFSIGNALED(status))) active_children.push_back(pid);
  }
  m_privilaged_children_pids.swap(active_children);
}
} // namespace ordo
