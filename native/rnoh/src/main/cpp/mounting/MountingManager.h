#pragma once
#include <functional>

#include <react/renderer/components/root/RootShadowNode.h>
#include <react/renderer/core/LayoutableShadowNode.h>
#include <react/renderer/core/RawProps.h>
#include <react/renderer/core/ReactPrimitives.h>
#include <react/renderer/mounting/MountingCoordinator.h>
#include <react/renderer/mounting/TelemetryController.h>

#include "TaskExecutor/TaskExecutor.h"
#include "events/EventEmitterRegistry.h"

namespace rnoh {

using namespace facebook;

class MountingManager {
  public:
    using TriggerUICallback = std::function<void(facebook::react::ShadowViewMutationList const &mutations)>;

    MountingManager(TaskExecutor::Shared taskExecutor, EventEmitterRegistry::Shared eventEmitterRegistry, TriggerUICallback triggerUICallback)
        : taskExecutor(std::move(taskExecutor)), eventEmitterRegistry(std::move(eventEmitterRegistry)), triggerUICallback(std::move(triggerUICallback)) {}

    void performMountInstructions(react::ShadowViewMutationList const &mutations, react::SurfaceId surfaceId);

    void scheduleTransaction(react::MountingCoordinator::Shared const &mountingCoordinator);

    void performTransaction(facebook::react::MountingCoordinator::Shared const &mountingCoordinator);

  private:
    TaskExecutor::Shared taskExecutor;
    EventEmitterRegistry::Shared eventEmitterRegistry;
    TriggerUICallback triggerUICallback;
};

} // namespace rnoh