#include "ordo/window_manager_policy.h"

#include <csignal>
#include <miral/application_info.h>
#include <miral/window_info.h>
#include <miral/window_manager_tools.h>

namespace ordo {
using namespace miral;

struct WindowPolicyData {
  bool is_workspace_hidden = false;
  MirWindowState previous_state;
};

inline WindowPolicyData& policy_data_for(const WindowInfo& info) {
  return *std::static_pointer_cast<WindowPolicyData>(info.userdata());
}

WindowManagerPolicy::WindowManagerPolicy(
  const WindowManagerTools& tools, RunnerOptionWrapper<CommandManager>& wrapped_command_manager) :
  MinimalWindowManager(tools) {
  
  setup_shortcuts(*wrapped_command_manager);

  m_workspaces.reserve(s_workspace_count);
  for (unsigned int i = 0; i < s_workspace_count; ++i) {
    m_workspaces.push_back(this->tools.create_workspace());
  }
}

WindowManagerPolicy::~WindowManagerPolicy() = default;

void WindowManagerPolicy::setup_shortcuts(CommandManager& command_manager) {
  command_manager.add_command("set_workspace_1", [this](){ set_workspace(1); });
  command_manager.add_command("set_workspace_2", [this](){ set_workspace(2); });
  command_manager.add_command("set_workspace_3", [this](){ set_workspace(3); });
  command_manager.add_command("set_workspace_4", [this](){ set_workspace(4); });
  command_manager.add_command("set_workspace_5", [this](){ set_workspace(5); });
  command_manager.add_command("set_workspace_6", [this](){ set_workspace(6); });
  command_manager.add_command("set_workspace_7", [this](){ set_workspace(7); });
  command_manager.add_command("set_workspace_8", [this](){ set_workspace(8); });
  command_manager.add_command("set_workspace_9", [this](){ set_workspace(9); });
  command_manager.add_command("set_workspace_0", [this](){ set_workspace(0); });
  command_manager.add_command("set_previous_workspace", [this](){ set_workspace((m_active_workspace_idx-1)%s_workspace_count); });
  command_manager.add_command("set_next_workspace", [this](){ set_workspace((m_active_workspace_idx+1)%s_workspace_count); });
  command_manager.add_command("move_to_workspace_1", [this](){ set_workspace(1, tools.active_window()); });
  command_manager.add_command("move_to_workspace_2", [this](){ set_workspace(2, tools.active_window()); });
  command_manager.add_command("move_to_workspace_3", [this](){ set_workspace(3, tools.active_window()); });
  command_manager.add_command("move_to_workspace_4", [this](){ set_workspace(4, tools.active_window()); });
  command_manager.add_command("move_to_workspace_5", [this](){ set_workspace(5, tools.active_window()); });
  command_manager.add_command("move_to_workspace_6", [this](){ set_workspace(6, tools.active_window()); });
  command_manager.add_command("move_to_workspace_7", [this](){ set_workspace(7, tools.active_window()); });
  command_manager.add_command("move_to_workspace_8", [this](){ set_workspace(8, tools.active_window()); });
  command_manager.add_command("move_to_workspace_9", [this](){ set_workspace(9, tools.active_window()); });
  command_manager.add_command("move_to_workspace_0", [this](){ set_workspace(0, tools.active_window()); });
  command_manager.add_command("toggle_maximized", [this](){ toggle_window_state(mir_window_state_maximized); });
  command_manager.add_command("toggle_fullscreen", [this](){ toggle_window_state(mir_window_state_fullscreen); });
  command_manager.add_command("close_app", [this](){ close_app(); });
  command_manager.add_command("kill_app", [this](){ kill_app(); });
}

miral::WindowSpecification WindowManagerPolicy::place_new_window(
  const miral::ApplicationInfo& info, const miral::WindowSpecification& request_parameters) {
  auto parameters = MinimalWindowManager::place_new_window(info, request_parameters);

  parameters.userdata() = std::make_shared<WindowPolicyData>();
  return parameters;
}

void WindowManagerPolicy::advise_new_window(const miral::WindowInfo& info) {
  MinimalWindowManager::advise_new_window(info);

  auto parent = info.parent();

  if (!parent) {
    tools.add_tree_to_workspace(info.window(), active_workspace());
  }
  else if (policy_data_for(tools.info_for(parent)).is_workspace_hidden) {
    set_workspace_hidden(info.window());
  }
}

void WindowManagerPolicy::handle_modify_window(miral::WindowInfo& info, const miral::WindowSpecification& modifications) {
  auto mods_copy = modifications;
  auto& policy_data = policy_data_for(info);

  if (policy_data.is_workspace_hidden && mods_copy.state().is_set()) {
    policy_data.previous_state = mods_copy.state().consume();
  }

  MinimalWindowManager::handle_modify_window(info, mods_copy);
}

std::shared_ptr<miral::Workspace> WindowManagerPolicy::active_workspace() {
  return m_workspaces.at(m_active_workspace_idx);
}

void WindowManagerPolicy::toggle_window_state(MirWindowState state) {
  if (const auto window = tools.active_window()) {
    auto& info = tools.info_for(window);

    WindowSpecification modifications;

    modifications.state() = (info.state() == state) ? mir_window_state_restored : state;
    tools.place_and_size_for_state(modifications, info);
    tools.modify_window(info, modifications);
  }
}

void WindowManagerPolicy::advise_adding_to_workspace(
  const std::shared_ptr<miral::Workspace>& workspace,
  const std::vector<miral::Window>& windows) {
  if (windows.empty())
    return;

  for (auto const& window : windows) {
    if (workspace == active_workspace()) {
      set_workspace_visible(window);
    }
    else {
      set_workspace_hidden(window);
    }
  }
}

void WindowManagerPolicy::set_workspace(unsigned int workspace_idx, const miral::Window& window) {
  if (workspace_idx == m_active_workspace_idx)
    return;

  auto workspace = m_workspaces[workspace_idx];
  auto previous_workspace = active_workspace();
  m_active_workspace_idx = workspace_idx;

  auto previous_active_window = tools.active_window();

  if (!previous_active_window) {
    if (auto const ww = m_active_windows[workspace]) {
      tools.for_each_workspace_containing(ww, [&](const std::shared_ptr<miral::Workspace>& ws) {
        if (ws == workspace) {
          set_workspace_visible(ww);
        }
      });
    }
  }

  tools.remove_tree_from_workspace(window, previous_workspace);
  tools.add_tree_to_workspace(window, active_workspace());

  tools.for_each_window_in_workspace(active_workspace(), [&](const Window& window) {
    set_workspace_visible(window);
  });

  bool hide_previous_active_window = false;
  tools.for_each_window_in_workspace(previous_workspace, [&](const Window& window) {
    if (window == previous_active_window) {
      hide_previous_active_window = true;
      return;
    }

    set_workspace_hidden(window);
  });

  if (hide_previous_active_window) {
    set_workspace_hidden(previous_active_window);

    m_active_windows[previous_workspace] = previous_active_window;
  }
}

void WindowManagerPolicy::set_workspace_visible(const miral::Window& window) {
  auto& info = tools.info_for(window);
  auto& policy_data = policy_data_for(info);
  if (policy_data.is_workspace_hidden) {
    policy_data.is_workspace_hidden = false;
    WindowSpecification modifications;
    modifications.state() = policy_data.previous_state;
    tools.place_and_size_for_state(modifications, info);
    tools.modify_window(info.window(), modifications);
  }
}

void WindowManagerPolicy::set_workspace_hidden(const miral::Window& window) {
  auto& info = tools.info_for(window);
  auto& policy_data = policy_data_for(info);
  if (!policy_data.is_workspace_hidden) {
    policy_data.is_workspace_hidden = true;
    policy_data.previous_state = info.state();
    WindowSpecification modifications;
    modifications.state() = mir_window_state_hidden;
    tools.place_and_size_for_state(modifications, info);
    tools.modify_window(info.window(), modifications);
  }
}

void WindowManagerPolicy::close_app() {
  tools.ask_client_to_close(tools.active_window());
}

void WindowManagerPolicy::kill_app() {
  if (const auto& window = tools.active_window()) {
    kill(window.application(), SIGTERM);
  }
}
} // namespace ordo