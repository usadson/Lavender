/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "DropTarget.hpp"

#include <optional>

#include <ShlObj_core.h>

#include <cassert>
#include <cwchar>

#include "Source/Base/ScopeGuard.hpp"

#ifdef LAVENDER_WIN32_HAS_OLE

namespace platform::win32 {

    template<typename Callback>
    [[nodiscard]] static base::Error
    iterateFormats(IDataObject *dataObject, Callback callback) noexcept {
        base::FunctionErrorGenerator errors{"Win32Core", "OLE::DataObject"};

        LPENUMFORMATETC formatEnumerator = nullptr;
        base::ScopeGuard guard{[formatEnumerator]() {
            if (formatEnumerator != nullptr)
                formatEnumerator->Release();
        }};

        if (auto hr = dataObject->EnumFormatEtc(DATADIR_GET, &formatEnumerator); FAILED(hr)) {
            return errors.error("IDataObject->EnumFormatEtc", ole::oleStatusToString(hr));
        }

        if (auto hr = formatEnumerator->Reset(); FAILED(hr)) {
            return errors.error("IEnumFORMATETC->Reset", ole::oleStatusToString(hr));
        }

        FORMATETC fEtc;
        ULONG ulFetched = 0L;
        while (true) {
            if (auto hr = formatEnumerator->Next(1, &fEtc, &ulFetched); FAILED(hr)) {
                return errors.error("IEnumFORMATETC->Next", ole::oleStatusToString(hr));
            }

            if (ulFetched == 0)
                break;

            callback(fEtc);
        }

        return base::Error::success();
    }

    DropTarget::DropTarget() noexcept = default;
    DropTarget::~DropTarget() noexcept = default;

    // https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-queryinterface(refiid_void)
    HRESULT STDMETHODCALLTYPE
    DropTarget::QueryInterface(const IID &referenceIdentifier, LPVOID *ppvObject) {
        if (ppvObject == nullptr)
            return E_POINTER;

        *ppvObject = nullptr;
        if (IsEqualIID(referenceIdentifier, IID_IDropTarget)
                || IsEqualIID(referenceIdentifier, IID_IUnknown))
            *ppvObject = this;

        if (*ppvObject != nullptr) {
            static_cast<LPUNKNOWN>(*ppvObject)->AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE
    DropTarget::AddRef() {
        return ++m_referenceCount;
    }

    ULONG STDMETHODCALLTYPE
    DropTarget::Release() {
        return --m_referenceCount;
    }

    [[nodiscard]] HWND
    getWindow(IDataObject *dataObject, POINTL pt) {
        LPENUMFORMATETC formatEnumerator = nullptr;
        base::ScopeGuard guard{[formatEnumerator]() {
            if (formatEnumerator != nullptr)
                formatEnumerator->Release();
        }};

        if (FAILED(dataObject->EnumFormatEtc(DATADIR_GET, &formatEnumerator))) {
            std::puts("failed to EnumFormatEtc");
            return nullptr;
        }

        if (FAILED(formatEnumerator->Reset())) {
            std::puts("failed to formatEnumerator->Reset");
            return nullptr;
        }

        FORMATETC fEtc;
        ULONG ulFetched = 0L;
        while (true) {
            auto hr = formatEnumerator->Next(1, &fEtc, &ulFetched);
            if (FAILED(hr)) {
                std::printf("failed to hr %llu\n", static_cast<long long unsigned>(hr));
                return nullptr;
            }
            if (ulFetched == 0) {
                std::puts("ulFetched == 0");
                return nullptr;
            }

            fEtc.cfFormat = CF_HDROP;
            fEtc.dwAspect = DVASPECT_CONTENT;
            fEtc.lindex = -1;
            fEtc.ptd = nullptr;
            fEtc.tymed = TYMED_HGLOBAL;

            STGMEDIUM stgM;
            hr = dataObject->GetData(&fEtc, &stgM);
            if (FAILED(hr)) {
                std::printf("failed GetData %llx\n", static_cast<unsigned long long>(hr));
#define CHECK_HR(t) if (hr == t) { std::puts("it's: " #t); continue; }
                CHECK_HR(DV_E_LINDEX)
                CHECK_HR(DV_E_FORMATETC)
                CHECK_HR(DV_E_TYMED)
                CHECK_HR(DV_E_DVASPECT)
                CHECK_HR(OLE_E_NOTRUNNING)
                CHECK_HR(STG_E_MEDIUMFULL)
                CHECK_HR(E_UNEXPECTED)
                CHECK_HR(E_INVALIDARG)
                CHECK_HR(E_OUTOFMEMORY)
                CHECK_HR(E_FAIL)
                continue;
            }

            if (stgM.tymed != TYMED_HGLOBAL) {
                std::puts("failed tymed");
                continue;
            }

            auto fileCount = DragQueryFile(static_cast<HDROP>(stgM.hGlobal), 0xFFFFFFFF, nullptr, 0);
            if (fileCount == 0)
                continue;

            POINT point{pt.x, pt.y};
            auto hwndTV = WindowFromPoint(point);
            auto hwndDlg = GetParent(hwndTV);

            if (IsWindow(hwndDlg)) {
                return hwndDlg;
            }
            return hwndTV;
        }
    }

    HRESULT STDMETHODCALLTYPE 
    DropTarget::DragEnter([[maybe_unused]] IDataObject __RPC_FAR *dataObject,
                          [[maybe_unused]] DWORD grfKeyState, 
                          POINTL point, 
                          DWORD __RPC_FAR *pdwEffect) {
        input::DragEnterEvent event{input::DragObject::createWithFileLocations({}), math::Vector2f::from(point.x, point.y)};

        m_dataObject = dataObject;
        std::printf(__FUNCSIG__);
        iterateFormats(dataObject, [&](const FORMATETC &format) {
            std::printf("Format> %x\n", format.cfFormat);
            ole::debugPrintClipFormat(format.cfFormat);
        }).displayErrorMessageBox();

        if (auto error = onDragEnter.invoke(event))
            error.displayErrorMessageBox();

        if (event.cancelStatus() == event::CancelStatus::NOT_CANCELLED) {
            *pdwEffect = DROPEFFECT_NONE;
        } else {
            *pdwEffect = DROPEFFECT_COPY;
        }

	    return S_OK;
    }

    HRESULT STDMETHODCALLTYPE 
    DropTarget::DragOver([[maybe_unused]] DWORD grfKeyState, POINTL point, DWORD __RPC_FAR *pdwEffect) {
        std::optional<input::DragObject> dragObject;
        
        assert(m_dataObject != nullptr);
        iterateFormats(m_dataObject, [&](const FORMATETC &format) {
            if (format.cfFormat == CF_HDROP) {
                STGMEDIUM stgM;

                auto format2 = format;
                if (auto hr = m_dataObject->GetData(&format2, &stgM); FAILED(hr)) {
                    std::printf("GetData error: %s\n", ole::oleStatusToString(hr).data());
                    return;
                }

                assert(format.tymed == TYMED_HGLOBAL);
                auto *dropFiles = static_cast<DROPFILES *>(GlobalLock(stgM.hGlobal));
                base::ScopeGuard unlocker{[&]() { GlobalUnlock(stgM.hGlobal); }};

                std::vector<std::string> files;
                if (!dropFiles->fWide) {
                    auto it = reinterpret_cast<char *>(dropFiles) + dropFiles->pFiles;
                    while (auto len = std::strlen(it)) {
                        files.emplace_back(it, len);
                        it += len + 1;
                    }
                } else {
                    auto it = reinterpret_cast<wchar_t *>(reinterpret_cast<std::uint8_t *>(dropFiles) + dropFiles->pFiles);
                    while (auto len = std::wcslen(it)) {
                        files.emplace_back();
                        for (wchar_t character : std::wstring_view{it, len})
                            files.back() += static_cast<char>(character);
                        it += len + 1;
                    }
                }

                assert(!dragObject.has_value());
                dragObject = input::DragObject::createWithFileLocations(std::move(files));
            }
        }).displayErrorMessageBox();

        if (!dragObject.has_value()) {
            std::printf("[Warning] Couldn't figure out DragObject\n");
            return S_OK;
        }

        input::DragOverEvent event{std::move(dragObject.value()), math::Vector2f::from(point.x, point.y)};

        *pdwEffect = DROPEFFECT_NONE;
        if (auto error = onDragOver.invoke(event)) {
            error.displayErrorMessageBox();
            return E_UNEXPECTED;
        }

        assert(event.cancelStatus() == event::CancelStatus::NOT_CANCELLED);
        assert(event.isDropAllowed());
        if (event.cancelStatus() == event::CancelStatus::NOT_CANCELLED && event.isDropAllowed()) {
            *pdwEffect = DROPEFFECT_COPY;
        }
	    return S_OK;
    }

    HRESULT STDMETHODCALLTYPE 
    DropTarget::DragLeave() {
        event::Event event{};

        if (auto error = onDragLeave.invoke(event)) {
            error.displayErrorMessageBox();
            return E_UNEXPECTED;
        }

	    return S_OK;
    }

    // https://learn.microsoft.com/en-us/windows/win32/api/oleidl/nf-oleidl-idroptarget-drop
    HRESULT STDMETHODCALLTYPE
    DropTarget::Drop(IDataObject *dataObject, [[maybe_unused]] DWORD keyboardModifierState, POINTL point,DWORD *dropEffect) {
        std::optional<input::DragObject> dragObject;

        std::printf(__FUNCSIG__);
        iterateFormats(dataObject, [&](const FORMATETC &format) {
            ole::debugPrintClipFormat(format.cfFormat);

            if (format.cfFormat == CF_HDROP) {
                STGMEDIUM stgM;

                auto format2 = format;
                if (auto hr = dataObject->GetData(&format2, &stgM); FAILED(hr)) {
                    std::printf("GetData error: %s\n", ole::oleStatusToString(hr).data());
                    return;
                }

                assert(format.tymed == TYMED_HGLOBAL);
                auto *dropFiles = static_cast<DROPFILES *>(GlobalLock(stgM.hGlobal));
                base::ScopeGuard unlocker{[&]() { GlobalUnlock(stgM.hGlobal); }};

                assert(dropFiles != nullptr);

                std::vector<std::string> files;
                if (!dropFiles->fWide) {
                    auto it = reinterpret_cast<char *>(dropFiles) + dropFiles->pFiles;
                    while (auto len = std::strlen(it)) {
                        files.emplace_back(it, len);
                        it += len + 1;
                    }
                } else {
                    auto it = reinterpret_cast<wchar_t *>(reinterpret_cast<std::uint8_t *>(dropFiles) + dropFiles->pFiles);
                    while (auto len = std::wcslen(it)) {
                        files.emplace_back();
                        for (wchar_t character : std::wstring_view{it, len})
                            files.back() += static_cast<char>(character);
                        it += len + 1;
                    }
                }

                assert(!dragObject.has_value());
                dragObject = input::DragObject::createWithFileLocations(std::move(files));
            }
        }).displayErrorMessageBox();

        if (!dragObject.has_value()) {
            std::printf("[Warning] Couldn't figure out DragObject\n");
            return S_OK;
        }

        input::DragEnterEvent event{std::move(dragObject.value()), math::Vector2f::from(point.x, point.y)};

        *dropEffect = DROPEFFECT_NONE;
        if (auto error = onDrop.invoke(event)) {
            error.displayErrorMessageBox();
            return E_UNEXPECTED;
        }

        if (event.cancelStatus() == event::CancelStatus::NOT_CANCELLED && event.isDropAllowed())
            *dropEffect = DROPEFFECT_COPY;

        return S_OK;
    }


} // namespace platform::win32

#endif // defined(LAVENDER_WIN32_HAS_OLE)
