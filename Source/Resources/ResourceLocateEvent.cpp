/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#include "ResourceLocateEvent.hpp"

#include <filesystem>

namespace resources {

    ResourceLocateEvent::ResourceLocateEvent(ResourceType resourceType, std::string_view resourceName) noexcept
            : m_resourceType(resourceType)
            , m_resourceName(resourceName)
            , m_directoryHints{std::filesystem::current_path().string()} {
    }

    ResourceLocateEvent::~ResourceLocateEvent() noexcept = default;


} // namespace resources
