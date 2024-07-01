#pragma once

#include "../../dependencies.h"
#include "../../module.h"
#include "../../domain/services/logService.h"
#include "../../domain/services/moduleService.h"
#include "../../domain/controller.h"
#include "../../domain/entityRepository.h"
#include "../../domain/dataContext.h"
#include "../../domain/events/event.h"
#include "../../domain/devices/terminalDevice.h"
#ifdef WIN32
#include "../../domain/devices/standartIoDeviceWin32.h"
#endif
#ifdef __unix__
#include "../../domain/devices/standartIoDeviceUnix.h"
#endif
#include "../../domain/repositories/pipelineRepository.h"
#include "../../domain/configuration.h"
#include "../../domain/events/hub.h"
#include "../../domain/entity.h"
#include "../../domain/engine.h"
#include "../../domain/services/frameService.h"
#include "../../domain/managers/viewManager.h"
#include "../../domain/services/scriptService.h"
#include "../../domain/services/configurationService.h"
#include "../../domain/services/logService.h"
#include "../../domain/services/fileService.h"
#include "../../domain/services/serializationService.h"
#include "../../domain/services/localizationService.h"
#include "../../domain/resources/localization.h"