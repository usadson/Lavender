/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Pipeline.hpp"

#include <array>
#include <filesystem>
#include <iostream>

#include <cstdio>

#include "Source/IO/FileInput.hpp"
#include "Source/Vulkan/VulkanCore.hpp"

namespace vke {

    Pipeline::~Pipeline() noexcept {
        const auto device = m_core->logicalDevice();
        const auto &allocator = m_core->allocator();

        if (!device)
            return;

        if (m_renderPass)
            device.destroyRenderPass(m_renderPass, allocator);

        if (m_vertexShaderModule)
            device.destroyShaderModule(m_vertexShaderModule, allocator);

        if (m_fragmentShaderModule)
            device.destroyShaderModule(m_fragmentShaderModule, allocator);

        if (m_pipeline)
            device.destroyPipeline(m_pipeline, allocator);

        if (m_pipelineLayout)
            device.destroyPipelineLayout(m_pipelineLayout, allocator);
    }

    bool
    Pipeline::create() noexcept {
        if (!createRenderPass())
            return false;

        if (!createShaderModules())
            return false;

        if (!createPipelineLayout())
            return false;

        if (!createGraphicsPipeline())
            return false;

        return true;
    }

    bool
    Pipeline::createGraphicsPipeline() noexcept {
        vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo{
            {},
            vk::ShaderStageFlagBits::eVertex,
            m_vertexShaderModule,
            "main"
        };

        const vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{
            {},
            vk::ShaderStageFlagBits::eFragment,
            m_fragmentShaderModule,
            "main"
        };

        const std::array shaderStages{vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo};

        const vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        const vk::PipelineInputAssemblyStateCreateInfo inputAssembly{{}, vk::PrimitiveTopology::eTriangleList, VK_FALSE};
        

        const auto &extent = m_core->swapChain().extent();

        const vk::Viewport viewport{
            0,
            0,
            static_cast<float>(extent.width),
            static_cast<float>(extent.height),
            0.0,
            1.0
        };

        const vk::Rect2D scissor{{0, 0}, extent};

        const vk::PipelineViewportStateCreateInfo viewportState{
            {},
            1, &viewport,
            1, &scissor
        };

        const vk::PipelineRasterizationStateCreateInfo rasterizer{
            {},
            VK_FALSE,
            VK_FALSE,
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eBack,
            vk::FrontFace::eClockwise,
            VK_FALSE,
            0.0,
            0.0,
            0.0,
            1.0
        };

        // disabled
        const vk::PipelineMultisampleStateCreateInfo mutlisampleCreateInfo{{}, vk::SampleCountFlagBits::e1, VK_FALSE};

        const vk::PipelineColorBlendAttachmentState colorBlendAttachment{
            VK_FALSE,
            vk::BlendFactor::eZero,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::BlendFactor::eZero,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::ColorComponentFlagBits::eR
                | vk::ColorComponentFlagBits::eG
                | vk::ColorComponentFlagBits::eB
                | vk::ColorComponentFlagBits::eA
        };

        const vk::PipelineColorBlendStateCreateInfo colorBlending{
            {},
            VK_FALSE,
            vk::LogicOp::eCopy,
            1, &colorBlendAttachment
        };

        constexpr const std::array dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eLineWidth};

        const vk::PipelineDynamicStateCreateInfo dynamicState{
            {},
            static_cast<std::uint32_t>(std::size(dynamicStates)),
            std::data(dynamicStates)
        };

        vk::GraphicsPipelineCreateInfo pipelineInfo{
            {},
            static_cast<std::uint32_t>(std::size(shaderStages)),
            std::data(shaderStages),
            &vertexInputInfo,
            &inputAssembly,
            nullptr, // tesselation
            &viewportState,
            &rasterizer,
            &mutlisampleCreateInfo,
            nullptr, // depth stencil
            &colorBlending,
            nullptr, // dynamic state

            m_pipelineLayout,

            // Render Pass
            m_renderPass,
            0,

            // Base Pipeline
            nullptr,
            -1
        };

        const auto creation = m_core->logicalDevice().createGraphicsPipelines(
            VK_NULL_HANDLE,
            {1, &pipelineInfo},
            m_core->allocator()
        );

        if (creation.result != vk::Result::eSuccess) {
            return false;
        }

        m_pipeline = creation.value[0];

        return true;
    }

    bool
    Pipeline::createPipelineLayout() noexcept {
        const vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};

        const auto pipelineLayoutCreation = m_core->logicalDevice().createPipelineLayout(pipelineLayoutCreateInfo, m_core->allocator());

        if (pipelineLayoutCreation.result != vk::Result::eSuccess) {
            return false;
        }

        m_pipelineLayout = pipelineLayoutCreation.value;
        return true;
    }

    bool
    Pipeline::createRenderPass() noexcept {
        const vk::AttachmentDescription colorAttachment{
            {},
            m_core->swapChain().imageFormat(),
            vk::SampleCountFlagBits::e1,
            
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            
            // stencil
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            
            // initial layout
            vk::ImageLayout::eUndefined,

            // final layout
            vk::ImageLayout::ePresentSrcKHR
        };

        constexpr const vk::AttachmentReference colorAttachmentRef{0, vk::ImageLayout::eColorAttachmentOptimal};
        
        const vk::SubpassDescription subpassDescription{
            {},
            vk::PipelineBindPoint::eGraphics,
            0, nullptr,
            1, &colorAttachmentRef
        };

        const vk::RenderPassCreateInfo renderPassCreateInfo{
            {},
            1, &colorAttachment,
            1, &subpassDescription,
        };

        const auto creation = m_core->logicalDevice().createRenderPass(renderPassCreateInfo, m_core->allocator());

        if (creation.result != vk::Result::eSuccess) {
            return false;
        }

        m_renderPass = creation.value;
        return true;
    }

    vk::ShaderModule
    Pipeline::createShaderModule(const std::vector<char8_t> &data) noexcept {
        vk::ShaderModule shaderModule{};

        vk::ShaderModuleCreateInfo createInfo{
            {},
            std::size(data),
            reinterpret_cast<const std::uint32_t *>(std::data(data))
        };

        const auto creation = m_core->logicalDevice().createShaderModule(createInfo, m_core->allocator());

        if (creation.result != vk::Result::eSuccess) { 
            return {};
        }

        return creation.value;
    }

    bool
    Pipeline::createShaderModules() noexcept {
        std::vector<char8_t> vertexShaderData{};
        std::vector<char8_t> fragmentShaderData{};

        {
            io::FileInput vertFile{"../Resources/Shaders/shader.vert.spv"};
            io::FileInput fragFile{"../Resources/Shaders/shader.frag.spv"};

            if (!vertFile || !fragFile) {
                std::printf("Failed to open file (%zu, %zu)\n", static_cast<std::size_t>(vertFile.error()),
                    static_cast<std::size_t>(fragFile.error()));
                std::cerr << std::filesystem::current_path() << std::endl;
                return false;
            }

            vertexShaderData = vertFile.read(std::size(vertFile));
            fragmentShaderData = fragFile.read(std::size(fragFile));

            if (!vertFile || !fragFile) {
                std::printf("Failed to read file (%zu, %zu)\n", static_cast<std::size_t>(vertFile.error()),
                    static_cast<std::size_t>(fragFile.error()));
                return false;
            }
        }

        m_vertexShaderModule = createShaderModule(vertexShaderData);
        m_fragmentShaderModule = createShaderModule(fragmentShaderData);

        if (!m_vertexShaderModule || !m_fragmentShaderModule) {
            return false;
        }

        return true;
    }

} // namespace vke
