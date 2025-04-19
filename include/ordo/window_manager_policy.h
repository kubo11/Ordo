#ifndef ORDO_WINDOW_MANAGER_POLICY_H
#define ORDO_WINDOW_MANAGER_POLICY_H

#include <memory>
#include <miral/application_info.h>
#include <miral/minimal_window_manager.h>
#include <ordo/command_manager.h>
#include <ordo/runner_option_wrapper.h>
#include <vector>

namespace ordo {

class WindowManagerPolicy : public miral::MinimalWindowManager {
 public:
  WindowManagerPolicy(const miral::WindowManagerTools& tools, RunnerOptionWrapper<CommandManager>& wrapped_command_manager);
  ~WindowManagerPolicy();

  virtual miral::WindowSpecification place_new_window(
    const miral::ApplicationInfo& info, const miral::WindowSpecification& request_parameters) override;
  void advise_new_window(const miral::WindowInfo& window_info) override;
  void handle_modify_window(miral::WindowInfo& info, const miral::WindowSpecification& modifications) override;

 private:
  inline static const unsigned int s_workspace_count = 10u;
  unsigned int m_active_workspace_idx = 1u;
  std::vector<std::shared_ptr<miral::Workspace>> m_workspaces{};
  std::unordered_map<std::shared_ptr<miral::Workspace>, miral::Window> m_active_windows{};

  std::shared_ptr<miral::Workspace> active_workspace();
  
  void setup_shortcuts(CommandManager& command_manager);

  void advise_adding_to_workspace(
    const std::shared_ptr<miral::Workspace>& workspace,
    const std::vector<miral::Window>& windows) override;
  void toggle_window_state(MirWindowState state);
  void set_workspace(
    unsigned int workspace_idx,
    const miral::Window& window = miral::Window{});
  void set_workspace_visible(const miral::Window& window);
  void set_workspace_hidden(const miral::Window& window);
  void close_app();
  void kill_app();
};

}

#endif // ORDO_WINDOW_MANAGER_POLICY_H