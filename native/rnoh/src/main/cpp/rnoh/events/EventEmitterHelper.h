#pragma once
#include <napi/native_api.h>
#include <react/renderer/core/EventEmitter.h>

#include "RNOH/ArkJS.h"
#include "RNOH/events/EventEmitterRegistry.h"

namespace rnoh {

enum ReactEventKind {
    TOUCH = 0,
    TEXT_INPUT_CHANGE = 1
};

class EventEmitterHelper {
  public:
    EventEmitterHelper(ArkJS arkJs, EventEmitterRegistry::Shared eventEmitterRegistry)
        : arkJs(std::move(arkJs)), eventEmitterRegistry(std::move(eventEmitterRegistry)) {}

    void emitEvent(facebook::react::Tag tag, ReactEventKind eventKind, napi_value eventObject);


  private:
    ArkJS arkJs;
    EventEmitterRegistry::Shared eventEmitterRegistry;

    void emitTouchEvent(facebook::react::Tag tag, napi_value eventObject);

    void emitTextInputChangedEvent(facebook::react::Tag tag, napi_value eventObject);
};

} // namespace rnoh
