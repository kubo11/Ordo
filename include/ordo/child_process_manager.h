#ifndef ORDO_CHILD_PROCESS_MANAGER_H
#define ORDO_CHILD_PROCESS_MANAGER_H

#include <miral/runner.h>
#include <miral/external_client.h>
#include <miral/wayland_extensions.h>
#include <mutex>
#include <vector>

namespace ordo {
using namespace miral;

class ChildProcessManager {
 public:
  ChildProcessManager(MirRunner& runner);
  ~ChildProcessManager();
  void operator()(mir::Server& server);
  void run(const std::string& cmd);
  void run_privileged(const std::string& cmd);
  void enable_extension_for_privilaged(WaylandExtensions& extensions, std::string const& protocol);

 private:
  MirRunner& m_runner;
  ExternalClientLauncher m_launcher;
  std::vector<pid_t> m_privilaged_children_pids;
  mutable std::mutex m_privilaged_children_pids_mtx;

  bool should_enable_extension_for_privilaged_callback(const WaylandExtensions::EnableInfo& info);
  bool is_privilaged(pid_t pid);
  void add_privilaged(pid_t pid);
  void shutdown_pivilaged_children();
  void remove_exited_privilaged_children_pids();
};
} // namespace ordo


#endif // ORDO_CHILD_PROCESS_MANAGER_H