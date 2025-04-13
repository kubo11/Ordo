#ifndef ORDO_COMMAND_MANAGER_H
#define ORDO_COMMAND_MANAGER_H

#include "ordo/child_process_manager.h"
#include "ordo/shortcut_manager.h"

#include <functional>
#include <unordered_map>

namespace ordo {
class CommandManager {
 public:
  using CmdAction = std::function<void(void)>;
  CommandManager(ChildProcessManager& child_proc_manager, ShortcutManager& shortcut_manager);
  ~CommandManager();

  void add_command(std::string&& cmd, CmdAction&& action);
  bool handle_event(const MirEvent* event);
  bool handle_keyboard_event(const MirKeyboardEvent* kev);

 private:
  std::unordered_map<std::string, CmdAction> m_commands;
  ChildProcessManager& m_child_process_manager;
  ShortcutManager& m_shortcut_manager;
};
} // namespace ordo


#endif // ORDO_COMMAND_MANAGER_H