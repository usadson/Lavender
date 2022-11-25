/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (C) 2022 Tristan Gerritsen <tristan@thewoosh.org>
 * All Rights Reserved.
 */

#pragma once

#include <string_view>
#include <vector>
#include <memory>

#include "Source/Event/Event.hpp"
#include "Source/Resources/ResourceLocation.hpp"

namespace resources {

    /**
     * Find the location of the requested file.
     */
    struct ResourceLocateEvent
            : public event::Event {
        enum class ResourceType {
            UNKNOWN,
            IMAGE,
            BINARY,

            VENDOR_SPECIFIC = 0x1000,
        };

        [[nodiscard]]
        ResourceLocateEvent(ResourceType resourceType, std::string_view resourceName) noexcept;

        ~ResourceLocateEvent() noexcept override;

        [[nodiscard]] inline constexpr std::string_view
        resourceName() const noexcept {
            return m_resourceName;
        }

        [[nodiscard]] inline constexpr ResourceType
        resourceType() const noexcept {
            return m_resourceType;
        }

        [[nodiscard]] inline constexpr std::unique_ptr<ResourceLocation> &
        location() noexcept {
            return m_location;
        }

        // Suggest a directory for the event handler to look in.
        // These suggestions are checked in LIFO-order.
        constexpr void
        suggestDirectory(std::string &&directory) noexcept {
            m_directoryHints.push_back(std::move(directory));
        }

        // Suggest a directory for the event handler to look in.
        // These suggestions are checked in LIFO-order.
        constexpr void
        suggestDirectory(const std::string &directory) noexcept {
            m_directoryHints.push_back(directory);
        }

        // These suggestions MUST be checked in LIFO-order.
        [[nodiscard]] inline constexpr std::vector<std::string> &
        directoryHints() noexcept {
            return m_directoryHints;
        }

        // These suggestions MUST be checked in LIFO-order.
        [[nodiscard]] inline constexpr const std::vector<std::string> &
        directoryHints() const noexcept {
            return m_directoryHints;
        }

    private:
        ResourceType m_resourceType;
        std::string_view m_resourceName;
        std::unique_ptr<ResourceLocation> m_location;

        // The event handler can look first in these directories before
        // searching the current working directory or perform it's own
        // algorithms.
        //
        // Of course, when the event handler doesn't use the filesystem
        // these hints may be ignored.
        std::vector<std::string> m_directoryHints{};
    };

} // namespace resources
