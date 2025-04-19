#include "ordo/child_process_manager.h"
#include "ordo/command_manager.h"
#include "ordo/runner_option_wrapper.h"
#include "ordo/shortcut_manager.h"
#include "ordo/window_manager_policy.h"

#include <functional>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <miral/append_event_filter.h>
#include <miral/display_configuration.h>
#include <miral/keymap.h>
#include <miral/runner.h>
#include <miral/set_window_management_policy.h>
#include <miral/toolkit_event.h>
#include <miral/wayland_extensions.h>
#include <miral/x11_support.h>

using namespace ordo;
using namespace miral;
using namespace miral::toolkit;

inline void enable_extensions(WaylandExtensions& extensions, const std::vector<const char*>& list) {
  for (const auto& protocol : list) {
    extensions.conditionally_enable(protocol, [&](WaylandExtensions::EnableInfo const& info) {
      return info.user_preference().value_or(true);
    });
  }
}

inline void enable_privilaged_extensions(WaylandExtensions& extensions, ChildProcessManager& proc_manager, const std::vector<const char*>& list) {
  for (auto const& protocol : list) {
    proc_manager.enable_extension_for_privilaged(extensions, protocol);
  }
}

int main(int argc, char const* argv[]){
  MirRunner runner{argc, argv};

  WaylandExtensions extensions{};
  enable_extensions(extensions, {
    WaylandExtensions::zwlr_screencopy_manager_v1,
    WaylandExtensions::zxdg_output_manager_v1,
    "zwp_idle_inhibit_manager_v1",
    "zwp_pointer_constraints_v1",
    "zwp_relative_pointer_manager_v1"
  });

  ShortcutManager shortcut_manager{};

  auto child_process_manager = RunnerOptionWrapper<ChildProcessManager>::create(runner);
  enable_privilaged_extensions(extensions, *child_process_manager, {
    WaylandExtensions::zwlr_layer_shell_v1,
    WaylandExtensions::zwlr_foreign_toplevel_manager_v1
  });

  auto command_manager = RunnerOptionWrapper<CommandManager>::create(*child_process_manager, shortcut_manager);

  // Add shortcut for exiting compositor in debug mode
#ifndef NDEBUG
  MirInputEventModifiers mods = mir_input_event_modifier_alt | mir_input_event_modifier_ctrl;
  shortcut_manager.add_shortcut(std::move(ordo::KeyCombo{mods, XKB_KEY_BackSpace}), std::move(ordo::Command{"exit", true, false}));
  (*command_manager).add_command("exit", [&runner](){ runner.stop(); });
#endif // NDEBUG

  return runner.run_with({
    X11Support{},
    Keymap{},
    DisplayConfiguration{runner},
    extensions,
    child_process_manager,
    shortcut_manager,
    AppendEventFilter{[&](const MirEvent* kev){return (*command_manager).handle_event(kev);}},
    set_window_management_policy<WindowManagerPolicy>(command_manager)
  });
}