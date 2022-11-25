/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "OLE.hpp"
#ifdef LAVENDER_WIN32_HAS_OLE

#include <array>
#include <sstream>
#include <string>

namespace platform::win32::ole {

    void
    debugPrintClipFormat(CLIPFORMAT clipFormat) noexcept {
        std::string_view type;

        switch (clipFormat) {
#define LAVENDER_PLATFORM_WIN32_OLE_ITERATE_CLIP_FORMATS(REGISTER_CLIP_FORMAT) \
            REGISTER_CLIP_FORMAT(CF_TEXT) \
            REGISTER_CLIP_FORMAT(CF_BITMAP) \
            REGISTER_CLIP_FORMAT(CF_METAFILEPICT) \
            REGISTER_CLIP_FORMAT(CF_SYLK) \
            REGISTER_CLIP_FORMAT(CF_DIF) \
            REGISTER_CLIP_FORMAT(CF_TIFF) \
            REGISTER_CLIP_FORMAT(CF_OEMTEXT) \
            REGISTER_CLIP_FORMAT(CF_DIB) \
            REGISTER_CLIP_FORMAT(CF_PALETTE) \
            REGISTER_CLIP_FORMAT(CF_PENDATA) \
            REGISTER_CLIP_FORMAT(CF_RIFF) \
            REGISTER_CLIP_FORMAT(CF_WAVE) \
            REGISTER_CLIP_FORMAT(CF_UNICODETEXT) \
            REGISTER_CLIP_FORMAT(CF_ENHMETAFILE) \
            REGISTER_CLIP_FORMAT(CF_HDROP) \
            REGISTER_CLIP_FORMAT(CF_LOCALE) \
            REGISTER_CLIP_FORMAT(CF_DIBV5) \
            REGISTER_CLIP_FORMAT(CF_OWNERDISPLAY) \
            REGISTER_CLIP_FORMAT(CF_DSPTEXT) \
            REGISTER_CLIP_FORMAT(CF_DSPBITMAP) \
            REGISTER_CLIP_FORMAT(CF_DSPMETAFILEPICT) \
            REGISTER_CLIP_FORMAT(CF_DSPENHMETAFILE) \

#define REGISTER_CLIP_FORMAT(format) \
            case format: \
                type = #format; \
                break;
            LAVENDER_PLATFORM_WIN32_OLE_ITERATE_CLIP_FORMATS(REGISTER_CLIP_FORMAT)
#undef REGISTER_CLIP_FORMAT

        default:
            if (clipFormat >= CF_PRIVATEFIRST && clipFormat <= CF_PRIVATELAST)
                type = "CF_PRIVATE#";
            else if (clipFormat >= CF_GDIOBJFIRST && clipFormat <= CF_GDIOBJLAST)
                type = "CF_GDIOBJ#";
            else
                type = "(unknown)";
        }

        std::array<char, 128> buffer;
        GetClipboardFormatNameA(clipFormat, buffer.data(), static_cast<int>(buffer.size()));

        std::printf("OLE CLIPFORMAT type=%s value=0x%x name=%s\n", type.data(), clipFormat, buffer.data()   );
    }

    std::string_view
    oleStatusToString(HRESULT result) noexcept {
        switch (result) {
#define LAVENDER_PLATFORM_WIN32_OLE_ITERATE_KNOWN_HRESULTS(REGISTER_RESULT) \
            REGISTER_RESULT(S_OK) \
            REGISTER_RESULT(E_ABORT) \
            REGISTER_RESULT(E_ACCESSDENIED) \
            REGISTER_RESULT(E_FAIL) \
            REGISTER_RESULT(E_HANDLE) \
            REGISTER_RESULT(E_INVALIDARG) \
            REGISTER_RESULT(E_NOINTERFACE) \
            REGISTER_RESULT(E_NOTIMPL) \
            REGISTER_RESULT(E_OUTOFMEMORY) \
            REGISTER_RESULT(E_POINTER) \
            REGISTER_RESULT(E_UNEXPECTED) \
            REGISTER_RESULT(DV_E_DVASPECT) \
            REGISTER_RESULT(DV_E_FORMATETC) \
            REGISTER_RESULT(DV_E_LINDEX) \
            REGISTER_RESULT(DV_E_TYMED) \
            REGISTER_RESULT(OLE_E_NOTRUNNING) \
            REGISTER_RESULT(STG_E_MEDIUMFULL) \



#define REGISTER_RESULT(w) \
            case w: \
                return #w; \


            LAVENDER_PLATFORM_WIN32_OLE_ITERATE_KNOWN_HRESULTS(REGISTER_RESULT)

#undef REGISTER_RESULT

            default: {
                static std::string string;

                std::stringstream stream;
                stream << "Unknown(HRESULT=0x" << std::hex << result << ')';
                string = stream.str();
                return string;
            }
        }
    }

    std::string_view
    oleTymedToString(TYMED tymed) noexcept {
        switch (tymed) {
            case TYMED_HGLOBAL: return "TYMED_HGLOBAL";
            case TYMED_FILE: return "TYMED_FILE";
            case TYMED_ISTREAM: return "TYMED_ISTREAM";
            case TYMED_ISTORAGE: return "TYMED_ISTORAGE";
            case TYMED_GDI: return "TYMED_GDI";
            case TYMED_MFPICT: return "TYMED_MFPICT";
            case TYMED_ENHMF: return "TYMED_ENHMF";
            case TYMED_NULL: return "TYMED_NULL"; 
        }
        return "(invalid)";
    }


} // namespace platform::win32::ole

#endif // LAVENDER_WIN32_HAS_OLE
