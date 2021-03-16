// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "Logging.h"

#include <cxxreact/ReactMarker.h>
#include <chrono>
#include "../Chakra/ChakraPlatform.h"

#define DELAYIMP_INSECURE_WRITABLE_HOOKS 1

#include <Windows.h>
#include <delayimp.h>

const bool fLoadJsiChakra = true;

// Install the handler for delay-loading mso binaries
extern PfnDliHook __pfnDliNotifyHook2 = nullptr; // MsoDelayLoadHook;

// Depending on the flight, load either reac-native-win32.dll or reac-native-win32.chakra.dll
FARPROC WINAPI ChakraDelayLoadHook(unsigned dliNotify, DelayLoadInfo* pdli)
{
  if (fLoadJsiChakra && dliNotify == dliNotePreLoadLibrary)
  {
    if (0 == ::_stricmp(pdli->szDll, "JSI.Desktop.ChakraCore.dll"))
    {
      return reinterpret_cast<FARPROC>(LoadLibrary(L"JSI.Desktop.Chakra.dll"));
    }
  }

  return nullptr; // MsoDelayLoadHook(dliNotify, pdli);
}

namespace facebook {
namespace react {

namespace {

NativeLoggingHook g_nativeLogHook;

void LogHook(RCTLogLevel logLevel, const char *message) {
  g_nativeLogHook(logLevel, message);
}

static double nativePerformanceNow() {
  return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

void logMarker(const ReactMarker::ReactMarkerId /*id*/, const char * /*tag*/) {}
} // end anonymous namespace

void InitializeLogging(NativeLoggingHook &&hook) {
  g_nativeLogHook = std::move(hook);

  __pfnDliNotifyHook2 = ChakraDelayLoadHook;

  JSNativeHooks::SetLoggingHook(LogHook);
  JSNativeHooks::SetNowHook(nativePerformanceNow);

  // This logMarker has another copy in jsi.desktop.chakra(core).dll,
  // so we need to call the exported function instead.
  // ReactMarker::logTaggedMarker = logMarker;
  JSNativeHooks::SetLogTaggedMarker(logMarker);
}

} // namespace react
} // namespace facebook
