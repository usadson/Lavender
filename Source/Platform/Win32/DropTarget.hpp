/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include "Source/Platform/Win32/OLE/OLE.hpp"

#ifdef LAVENDER_WIN32_HAS_OLE

#include "Source/Event/EventHandler.hpp"
#include "Source/Input/DragAndDropEvents.hpp"

namespace platform::win32 {

    interface DropTarget final
            : public IDropTarget {

        event::EventHandler<input::DragEnterEvent> onDragEnter{};
        event::EventHandler<input::DragOverEvent> onDragOver{};
        event::EventHandler<input::DropEvent> onDrop{};
        event::EventHandler<input::DragLeaveEvent> onDragLeave{};

        [[nodiscard]]
        DropTarget() noexcept;

        virtual
        ~DropTarget() noexcept;

        virtual HRESULT STDMETHODCALLTYPE
        QueryInterface(REFIID riid, LPVOID FAR *ppvObject);

        virtual ULONG STDMETHODCALLTYPE
        AddRef();

        virtual ULONG STDMETHODCALLTYPE
        Release();

        virtual HRESULT STDMETHODCALLTYPE
        DragEnter(IDataObject __RPC_FAR *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect);

        virtual HRESULT STDMETHODCALLTYPE
        DragOver(DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect);

        virtual HRESULT STDMETHODCALLTYPE
        DragLeave(void);

        virtual HRESULT STDMETHODCALLTYPE
        Drop(IDataObject __RPC_FAR *pDataObj, DWORD grfKeyState, POINTL pt, _In_ _Out_ DWORD __RPC_FAR *pdwEffect);


    private:
        ULONG m_referenceCount{};
        HWND m_currentWindow{};
        IDataObject *m_dataObject{nullptr};
    };


} // namespace platform::win32

#endif // LAVENDER_WIN32_HAS_OLE
