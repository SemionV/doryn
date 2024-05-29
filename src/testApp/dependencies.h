#pragma once

#include "base/dependencies.h"
#include "openGL/dependencies.h"

#ifdef WIN32
#include "base/win32/dependencies.h"
#endif

#ifdef __unix__
#include "base/unix/dependencies.h"
#endif

#include "base/serviceContainer.h"
#include "base/domain/configuration.h"
#include "base/domain/events/hub.h"
#include "base/domain/entity.h"
#include "base/domain/entityRepository.h"
#include "base/domain/engine.h"
#include "base/domain/services/frameService.h"
#include "base/domain/services/viewService.h"
#include "base/domain/devices/terminalDevice.h"
#include "base/domain/services/scriptService.h"
#include "base/domain/services/configurationService.h"
#include "base/domain/services/logService.h"
#include "openGL/glfwWindow.h"
#include "openGL/glfwWindowController.h"
#include "openGL/viewControllerOpenGL.h"
#include "openGL/services/shaderService.h"
#include "openGL/renderer.h"
#include "openGL/windowService.h"

#ifdef WIN32
#include "base/domain/devices/standartIoDeviceWin32.h"
#endif

#ifdef __unix__
#include "base/domain/devices/standartIoDeviceUnix.h"
#endif