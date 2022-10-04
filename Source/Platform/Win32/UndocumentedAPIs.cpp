/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "UndocumentedAPIs.hpp"

#include <cassert>
#include <cstdio>

namespace platform::win32::undocumented {
    
    using GetImmersiveColorFromColorSetExType = DWORD (WINAPI *)(UINT dwImmersiveColorSet, UINT dwImmersiveColorType, bool bIgnoreHighContrast, UINT dwHighContrastCacheMode);
    using GetImmersiveColorNamedTypeByIndexType = WCHAR **(WINAPI *)(UINT dwImmersiveColorType);
    using GetImmersiveColorSetCountType = int (WINAPI *)();
    using GetImmersiveColorTypeFromNameType = int (WINAPI *)(const WCHAR *name);
    using GetImmersiveUserColorSetPreferenceType = int (WINAPI *)(bool, bool);
    using SetConsoleIconType = bool (WINAPI *)(_In_ HICON);

#define ITERATE_METHODS_WITH_STRING_LOOKUP(REGISTER_METHOD) \
        REGISTER_METHOD(SetConsoleIcon, kernel32) \

#define ITERATE_METHODS_WITHOUT_STRING_LOOKUP(REGISTER_METHOD) \
        REGISTER_METHOD(GetImmersiveColorFromColorSetEx, MAKEINTRESOURCEA(95)) \
        REGISTER_METHOD(GetImmersiveColorNamedTypeByIndex, MAKEINTRESOURCEA(100)) \
        REGISTER_METHOD(GetImmersiveColorSetCount, MAKEINTRESOURCEA(94)) \
        REGISTER_METHOD(GetImmersiveColorTypeFromName, MAKEINTRESOURCEA(96)) \
        REGISTER_METHOD(GetImmersiveUserColorSetPreference, MAKEINTRESOURCEA(98)) \

#define ITERATE_METHODS(REGISTER_METHOD) \
        ITERATE_METHODS_WITH_STRING_LOOKUP(REGISTER_METHOD) \
        ITERATE_METHODS_WITHOUT_STRING_LOOKUP(REGISTER_METHOD) \

    static struct {

#define REGISTER_METHOD(function, _) function##Type func##function;
        ITERATE_METHODS(REGISTER_METHOD)
#undef REGISTER_METHOD
        
    } s_data{};

    void
    initialize() {
        static bool isInitialized { false};
        if (isInitialized)
            return;
        isInitialized = true;

        auto *kernel32 = GetModuleHandle(TEXT("kernel32.dll"));
        if (kernel32 == nullptr) {
            std::printf("\n\nWARNING: failed to get module handle of kernel\n\n\n");
            return;
        }

        auto *uxtheme = GetModuleHandle(TEXT("uxtheme.dll"));
        if (uxtheme == nullptr) {
            std::printf("\n\nWARNING: failed to get module handle of uxtheme.dll\n\n\n");
            return;
        }

#define REGISTER_METHOD(function, module) \
        s_data.func##function = reinterpret_cast<function##Type>(reinterpret_cast<void *>(GetProcAddress(module, #function))); \
        assert(s_data.func##function != nullptr);
        ITERATE_METHODS_WITH_STRING_LOOKUP(REGISTER_METHOD)
#undef REGISTER_METHOD


#define REGISTER_METHOD(function, lookup) \
        s_data.func##function = reinterpret_cast<function##Type>(reinterpret_cast<void *>(GetProcAddress(uxtheme, lookup))); \
        assert(s_data.func##function != nullptr);
        ITERATE_METHODS_WITHOUT_STRING_LOOKUP(REGISTER_METHOD)
#undef REGISTER_METHOD
    }

    DWORD
    getImmersiveColorFromColorSetEx(UINT colorSet, UINT colorType, bool ignoreHighContrast, UINT highContrastCacheMode) {
        initialize();

        return s_data.funcGetImmersiveColorFromColorSetEx(colorSet, colorType, ignoreHighContrast, highContrastCacheMode);
    }
    
    WCHAR **
    getImmersiveColorNamedTypeByIndex(UINT index) {
        initialize();

        return s_data.funcGetImmersiveColorNamedTypeByIndex(index);
    }

    int 
    getImmersiveColorSetCount() {
        initialize();

        return s_data.funcGetImmersiveColorSetCount();
    }

    int 
    getImmersiveColorTypeFromNameType(const WCHAR *name) {
        initialize();

        return s_data.funcGetImmersiveColorTypeFromName(name);
    }

    int 
    getImmersiveUserColorSetPreference(bool forceCheckRegistry, bool skipCheckOnFail) { 
        initialize();

        return s_data.funcGetImmersiveUserColorSetPreference(forceCheckRegistry, skipCheckOnFail);
    }

    bool 
    setConsoleIcon(HICON icon) noexcept {
        initialize();

        return s_data.funcSetConsoleIcon(icon);
    }

} // namespace platform::win32::undocumented
