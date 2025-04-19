#include "ordo/command_manager.h"

namespace ordo {
CommandManager::CommandManager(ChildProcessManager& child_proc_manager, ShortcutManager& shortcut_manager)
  : m_child_process_manager{child_proc_manager}, m_shortcut_manager{shortcut_manager}, m_commands{} {}

CommandManager::~CommandManager() = default;

void CommandManager::add_command(std::string&& cmd, CmdAction&& action) {
  m_commands.emplace(std::move(cmd), std::move(action));
}

bool CommandManager::handle_event(const MirEvent* event) {
  if (mir_event_get_type(event) != mir_event_type_input)
    return false;

  auto const* input_event = mir_event_get_input_event(event);

  switch (mir_input_event_get_type(input_event)) {
  case mir_input_event_type_key:
    return handle_keyboard_event(mir_input_event_get_keyboard_event(input_event));

  default:
    return false;
  }
}

bool CommandManager::handle_keyboard_event(const MirKeyboardEvent* kev) {
  if (mir_keyboard_event_action(kev) != mir_keyboard_action_down)
    return false;

  MirInputEventModifiers mods = mir_keyboard_event_modifiers(kev);
  xkb_keysym_t key = mir_keyboard_event_keysym(kev);

  auto cmd = m_shortcut_manager.get_command(KeyCombo{mods, key});
  if (!cmd.has_value())
    return false;

  if (cmd.value().is_builtin) {
    if (m_commands.contains(cmd.value().value)) {
      m_commands.at(cmd.value().value)();
      return true;
    }
    else {
      return false;
    }
  }

  if (cmd.value().is_privileged) {
    m_child_process_manager.run_privileged(cmd.value().value);
  }
  else {
    m_child_process_manager.run(cmd.value().value);
  }
  return true;
}
} // namespace ordo