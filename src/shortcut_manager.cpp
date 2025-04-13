#include "ordo/shortcut_manager.h"

#include <iostream>

namespace ordo {
ShortcutManager::ShortcutManager() : ConfigurationOption{
  [this](const std::vector<std::string>& config){ load(config); }, "keybind", "mods+key:command"} {}

ShortcutManager::~ShortcutManager() = default;

std::optional<Command> ShortcutManager::get_command(const KeyCombo& kc) {
  auto rkc = strip(kc);
  if (m_shortcuts.contains(rkc))
    return m_shortcuts.at(rkc);
  return {};
}

void ShortcutManager::add_shortcut(KeyCombo&& kc, Command&& cmd) {
  m_shortcuts.emplace(std::move(kc), std::move(cmd));
}

void ShortcutManager::load(const std::vector<std::string>& config) {
  for (const auto& command : config) {
    auto separator_idx = command.find(':');
    if (separator_idx < 1 || separator_idx >= command.size()) {
      std::cerr << "Could not process shortcut: " << command << std::endl;
      continue;
    }
    auto keys = command.substr(0, separator_idx);
    auto key_separator_idx = keys.rfind('+');
    if (key_separator_idx < 1 || key_separator_idx >= keys.size()) {
      std::cerr << "Could not process shortcut: " << command << std::endl;
      continue;
    }
    KeyCombo kc = {};
    kc.key = str_to_key(keys.substr(key_separator_idx+1));
    if (kc.key == XKB_KEY_NoSymbol) {
      std::cerr << "Could not process shortcut: " << command << std::endl;
      continue;
    }
    keys = keys.substr(0, key_separator_idx);
    while ((key_separator_idx = keys.find('+')) >= 1 && key_separator_idx < keys.size()) {
      kc.mods |= str_to_mod(keys.substr(0, key_separator_idx));
      keys = keys.substr(key_separator_idx+1);
    }
    kc.mods |= str_to_mod(keys);
    Command cmd = {};
    if (command[separator_idx+1] == '!') {
      cmd.value = command.substr(separator_idx+2);
      cmd.is_builtin = false;
      cmd.is_privileged = true;
    }
    else if (command[separator_idx+1] == '@') {
      cmd.value = command.substr(separator_idx+2);
      cmd.is_builtin = true;
      cmd.is_privileged = false;
    }
    else {
      cmd.value = command.substr(separator_idx+1);
      cmd.is_builtin = false;
      cmd.is_privileged = false;
    }
    m_shortcuts.emplace(std::move(kc), std::move(cmd));
  }
}

KeyCombo ShortcutManager::strip(const KeyCombo& kc) {
  auto copy = kc;
  copy.mods &= (mir_input_event_modifier_alt | mir_input_event_modifier_ctrl |
      mir_input_event_modifier_meta | mir_input_event_modifier_shift);
  return copy;
}

xkb_keysym_t ShortcutManager::str_to_key(const std::string& str) {
  return xkb_keysym_from_name(str.c_str(), XKB_KEYSYM_CASE_INSENSITIVE);
}

MirInputEventModifier ShortcutManager::str_to_mod(const std::string& str) {
  if (str == "alt") {
    return mir_input_event_modifier_alt;
  }
  else if (str == "ctrl") {
    return mir_input_event_modifier_ctrl;
  }
  else if (str == "meta") {
    return mir_input_event_modifier_meta;
  }
  else if (str == "shift") {
    return mir_input_event_modifier_shift;
  }

  return mir_input_event_modifier_none;
}

} // namespace ordo