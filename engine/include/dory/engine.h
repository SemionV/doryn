#pragma once

#include <dory/engine/dependencies.h>
#include <dory/engine/typeComponents.h>
#include <dory/engine/module.h>

#include <dory/engine/domain/configuration.h>
#include <dory/engine/domain/controller.h>
#include <dory/engine/domain/dataContext.h>
#include <dory/engine/domain/engine.h>
#include <dory/engine/domain/entity.h>
#include <dory/engine/domain/entityRepository.h>
#include <dory/engine/domain/object.h>
#include <dory/engine/domain/types.h>

#include <dory/engine/domain/devices/device.h>
#ifdef __unix__
#include <dory/engine/domain/devices/standartIoDeviceUnix.h>
#endif
#ifdef WIN32
#include <dory/engine/domain/devices/standartIoDeviceWin32.h>
#endif
#include <dory/engine/domain/devices/terminalDevice.h>

#include <dory/engine/domain/repositories/pipelineRepository.h>

#include <dory/engine/domain/managers/viewManager.h>
#include <dory/engine/domain/managers/pipelineManager.h>