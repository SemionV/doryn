#pragma once

#include <dory/engine/typeComponents.h>
#include <dory/engine/module.h>

#include <dory/engine/math/linearAlgebra.h>
#include <dory/engine/math/trigonometry.h>

#include <dory/engine/serialization/structures/dataLayout.h>
#include <dory/engine/serialization/binaryLayoutSerializer.h>
#include <dory/engine/serialization/jsonDeserializer.h>
#include <dory/engine/serialization/jsonSerializer.h>
#include <dory/engine/serialization/jsonPrinter.h>
#include <dory/engine/serialization/yamlDeserializer.h>
#include <dory/engine/serialization/yamlSerializer.h>

#include <dory/engine/concurrency/worker.h>
#include <dory/engine/concurrency/queue.h>
#include <dory/engine/concurrency/stack.h>

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

#include <dory/engine/domain/resources/localization.h>

#include <dory/engine/domain/services/configurationService.h>
#include <dory/engine/domain/services/fileService.h>
#include <dory/engine/domain/services/frameService.h>
#include <dory/engine/domain/services/localizationService.h>
#include <dory/engine/domain/services/logService.h>
#include <dory/engine/domain/services/moduleService.h>
#include <dory/engine/domain/services/scriptService.h>
#include <dory/engine/domain/services/serializationService.h>
#include <dory/engine/domain/services/windowService.h>

#include <dory/engine/rendering/opengl/windowService.h>
#include <dory/engine/rendering/opengl/viewControllerOpenGL.h>
#include <dory/engine/rendering/opengl/glfwWindowController.h>

#ifndef NDEBUG
#include <dory/engine/testing/atm_book.h>
#include <dory/engine/testing/dataGenerators.h>
#include <dory/engine/testing/messaging_book.h>
#include <dory/engine/testing/quickSort.h>
#include <dory/engine/testing/stack_lockfree.h>
#endif