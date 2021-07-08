/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <memory>

#include "Source/Base/ExitStatus.hpp"
#include "Source/Window/WindowAPI.hpp"
#include "Source/GraphicsAPI.hpp"

class Lavender {
    std::unique_ptr<GraphicsAPI> m_graphicsAPI{nullptr};
    std::unique_ptr<WindowAPI> m_windowAPI{ nullptr };

public:
    [[nodiscard]] base::ExitStatus
    run();
};
