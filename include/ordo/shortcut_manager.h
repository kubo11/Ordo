#ifndef ORDO_SHORTCUT_MANAGER_H
#define ORDO_SHORTCUT_MANAGER_H

#include "ordo/utils.h"

#include <mir_toolkit/events/enums.h>

#include <miral/configuration_option.h>
#include <miral/toolkit_event.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ordo {
struct KeyCombo {
  MirInputEventModifiers mods;
  xkb_keysym_t key;

  KeyCombo() : mods{}, key{} {}
  KeyCombo(const MirInputEventModifiers& mods, const xkb_keysym_t& key) : mods{mods}, key{key} {}
  bool operator==(const KeyCombo& other) const {
    return mods & other.mods && key == other.key;
  }
};

struct Command {
  std::string value;
  bool is_builtin;
  bool is_privileged;

  Command() : value{}, is_builtin{}, is_privileged{} {}
  Command(const std::string& value, bool is_builtin, bool is_privileged)
    : value{value}, is_builtin{is_builtin}, is_privileged{is_privileged} {}
  bool operator==(const Command& other) const {
    return value == other.value && is_builtin == other.is_builtin && is_privileged == other.is_privileged;
  }
};
} // namespace ordo

template<>
struct std::hash<ordo::KeyCombo> {
  std::size_t operator()(const ordo::KeyCombo& kc) const {
    std::size_t seed = 0;
    ordo::hash_combine(seed, kc.mods);
    ordo::hash_combine(seed, kc.key);
    return seed;
  }
};

template<>
struct std::hash<ordo::Command> {
  std::size_t operator()(const ordo::Command& cmd) const {
    std::size_t seed = 0;
    ordo::hash_combine(seed, cmd.value);
    ordo::hash_combine(seed, cmd.is_builtin);
    ordo::hash_combine(seed, cmd.is_privileged);
    return seed;
  }
};

namespace ordo {
using namespace miral;
using namespace miral::toolkit;

class ShortcutManager : public ConfigurationOption {
 public:
  ShortcutManager();
  ~ShortcutManager();

  using ConfigurationOption::operator();

  std::optional<Command> get_command(const KeyCombo& kc);
  void add_shortcut(KeyCombo&& kc, Command&& cmd);

 private:
  std::unordered_map<KeyCombo, Command> m_shortcuts;

  KeyCombo strip(const KeyCombo& kc);
  void load(const std::vector<std::string>& config);
  static xkb_keysym_t str_to_key(const std::string& str);
  static MirInputEventModifier str_to_mod(const std::string& str);
};
} // namespace ordo

#endif // ORDO_SHORTCUT_MANAGER_H