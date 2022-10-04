/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Vulkan and the Vulkan logo are registered trademarks of the Khronos Group
 * Inc.
 * 
 * This file includes Vulkan.hpp using the right configuration.
 */

#pragma once

#ifndef VULKAN_HPP

/**
 * Disabling spaceship operator<=> since it is improperly implemented
 * (by either the vulkan.hpp authors or my compiler). It gives warnings
 * about TransformMatrixKHR operator<=> which is defaulted. Given that
 * the transform-matrix is implemented as an float[][] 2d-array, which
 * doesn't have a comparator defined, the implicit comparison subsituation
 * fails, leaving the operator deleted.
 */
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR

/**
 * We're using VkResult instead of exceptions.
 */
#define VULKAN_HPP_NO_EXCEPTIONS

/**
 * Don't assert(result == success-like)
 */
#define VULKAN_HPP_ASSERT_ON_RESULT(...) ((void)0)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wshadow"
#include <vulkan/vulkan.hpp>
#pragma GCC diagnostic pop

#define VKE_RESULT(code) static_cast<VkResult>(vk::Result::code)

#endif // VULKAN_HPP
