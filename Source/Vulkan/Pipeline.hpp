/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <vector>

#include "Source/Vulkan/IncludeVulkan.hpp"

namespace vke {

    class Core;

    class Pipeline {
        Core *m_core{nullptr}; 

        vk::RenderPass m_renderPass{};

        vk::ShaderModule m_vertexShaderModule{};
        vk::ShaderModule m_fragmentShaderModule{};

        vk::PipelineLayout m_pipelineLayout{};
        vk::Pipeline m_pipeline;

        //
        // Subroutines from create()
        //

        [[nodiscard]] bool
        createGraphicsPipeline() noexcept;

        [[nodiscard]] bool
        createPipelineLayout() noexcept;

        [[nodiscard]] bool
        createRenderPass() noexcept;

        [[nodiscard]] bool
        createShaderModules() noexcept;

        //
        // Helpers of subroutines
        //

		[[nodiscard]] vk::ShaderModule
		createShaderModule(const std::vector<char8_t> &data) noexcept;

    public:
        [[nodiscard]] inline
        Pipeline(Core *core) noexcept
            : m_core(core) {
        }

        ~Pipeline() noexcept;

        [[nodiscard]] bool
        create() noexcept;

        [[nodiscard]] inline constexpr vk::RenderPass
        renderPass() const noexcept {
            return m_renderPass;
        }
    };

} // namespace vke
