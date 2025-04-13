#ifndef ORDO_WINDOW_MANAGER_POLICY_H
#define ORDO_WINDOW_MANAGER_POLICY_H

#include <miral/minimal_window_manager.h>

namespace ordo {

class WindowManagerPolicy : public miral::MinimalWindowManager {
 public:
  WindowManagerPolicy(miral::WindowManagerTools const& tools);
  ~WindowManagerPolicy();

  miral::WindowSpecification place_new_window(miral::ApplicationInfo const& app_info,
    miral::WindowSpecification const& request_parameters);
};

}

#endif // ORDO_WINDOW_MANAGER_POLICY_H