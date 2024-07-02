#pragma once

#include "base/dependencies.h"
#include "base/module.h"
#include "base/domain/services/logService.h"
#include "base/domain/services/moduleService.h"
#include "base/domain/controller.h"
#include "base/domain/entityRepository.h"
#include "base/domain/dataContext.h"
#include "base/domain/events/event.h"
#include "base/domain/devices/terminalDevice.h"
#ifdef WIN32
#include "base/domain/devices/standartIoDeviceWin32.h"
#endif
#ifdef __unix__
#include "base/domain/devices/standartIoDeviceUnix.h"
#endif
#include "base/domain/repositories/pipelineRepository.h"
#include "base/domain/configuration.h"
#include "base/domain/events/hub.h"
#include "base/domain/entity.h"
#include "base/domain/engine.h"
#include "base/domain/services/frameService.h"
#include "base/domain/managers/viewManager.h"
#include "base/domain/services/scriptService.h"
#include "base/domain/services/configurationService.h"
#include "base/domain/services/logService.h"
#include "base/domain/services/fileService.h"
#include "base/domain/services/serializationService.h"
#include "base/domain/services/localizationService.h"
#include "base/domain/resources/localization.h"