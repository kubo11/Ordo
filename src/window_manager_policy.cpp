#include "ordo/window_manager_policy.h"

#include <miral/application_info.h>
#include <miral/window_info.h>
#include <miral/window_manager_tools.h>
#include <miral/zone.h>

using namespace miral;

ordo::WindowManagerPolicy::WindowManagerPolicy(WindowManagerTools const& tools) :
  MinimalWindowManager(tools) {}

ordo::WindowManagerPolicy::~WindowManagerPolicy() = default;

miral::WindowSpecification ordo::WindowManagerPolicy::place_new_window(
  miral::ApplicationInfo const& app_info, miral::WindowSpecification const& request_parameters) {
  return MinimalWindowManager::place_new_window(app_info, request_parameters);
}