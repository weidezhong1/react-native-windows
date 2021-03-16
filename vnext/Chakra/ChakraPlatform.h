// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include <memory>
#include <string>

#include <cxxreact/JSExecutor.h>
#include <cxxreact/MessageQueueThread.h>
#include <cxxreact/ReactMarker.h>

#include "Logging.h"

#if defined(USE_EDGEMODE_JSRT)
#include <jsrt.h>
#else
#include <ChakraCore.h>
#endif

#define CHAKRA_FAILED(jsErrorCode) ((jsErrorCode) != ::JsErrorCode::JsNoError)

#ifdef NDEBUG
#define CHAKRA_ASSERTDO(jsErrorCodeExpr) (jsErrorCodeExpr)
#else
#define CHAKRA_ASSERTDO(jsErrorCodeExpr) \
  do {                                   \
    JsErrorCode ec = (jsErrorCodeExpr);  \
    assert(!CHAKRA_FAILED(ec));          \
  } while (false)
#endif

namespace facebook {
namespace react {

namespace JSNativeHooks {

__declspec(dllexport) void SetLogTaggedMarker(facebook::react::ReactMarker::LogTaggedMarker logTaggedMarkerIn);

using LoggingHook = void (*)(RCTLogLevel level, const char *);
extern LoggingHook loggingHook;
__declspec(dllexport) void SetLoggingHook(LoggingHook myLoggingHook);

using NowHook = double (*)();
extern NowHook nowHook;
__declspec(dllexport) void SetNowHook(NowHook myNowHook);

JsValueRef __stdcall nowHookJNF(
    JsValueRef function,
    bool isConstructCall,
    JsValueRef arguments[],
    unsigned short argumentCount,
    void *callbackState);

} // namespace JSNativeHooks

} // namespace react
} // namespace facebook
