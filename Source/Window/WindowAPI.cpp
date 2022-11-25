/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "WindowAPI.hpp"

using namespace window;

base::Error
WindowAPI::enableDragAndDrop([[maybe_unused]] WindowDragAndDropOption) noexcept {
    return base::Error("WindowAPI", "WindowAPI", "Enable drag & drop", "This window implementation does not support this feature.");
}

base::Error
WindowAPI::requestClose(CloseRequestedEvent::Reason reason) noexcept {
    CloseRequestedEvent event{this, reason};
    return onCloseRequested.invoke(event);
}
