/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "VulkanFence.h"

#include <igl/vulkan/Common.h>
#include <utility> // std::swap

namespace igl {
namespace vulkan {

VulkanFence::VulkanFence(const VulkanFunctionTable& vf,
                         VkDevice device,
                         VkFlags flags,
                         const char* debugName) :
  vf_(&vf), device_(device) {
  IGL_PROFILER_FUNCTION_COLOR(IGL_PROFILER_COLOR_CREATE);

  VK_ASSERT(ivkCreateFence(vf_, device_, flags, &vkFence_));
  VK_ASSERT(
      ivkSetDebugObjectName(vf_, device_, VK_OBJECT_TYPE_FENCE, (uint64_t)vkFence_, debugName));
}

VulkanFence ::~VulkanFence() {
  IGL_PROFILER_FUNCTION_COLOR(IGL_PROFILER_COLOR_DESTROY);

  if (device_ != VK_NULL_HANDLE) {
    // lifetimes of all VkFence objects are managed explicitly
    // we do not use deferredTask() for them
    vf_->vkDestroyFence(device_, vkFence_, nullptr);
  }
}

VulkanFence::VulkanFence(VulkanFence&& other) noexcept {
  std::swap(vf_, other.vf_);
  std::swap(device_, other.device_);
  std::swap(vkFence_, other.vkFence_);
}

VulkanFence& VulkanFence::operator=(VulkanFence&& other) noexcept {
  VulkanFence tmp(std::move(other));
  std::swap(vf_, tmp.vf_);
  std::swap(device_, tmp.device_);
  std::swap(vkFence_, tmp.vkFence_);
  return *this;
}

} // namespace vulkan
} // namespace igl
