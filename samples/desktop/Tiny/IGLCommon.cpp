/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "IGLCommon.h"

namespace igl {

std::unique_ptr<IDevice> createIGLDevice(void* window,
                                         void* display,
                                         void* context,
                                         int width,
                                         int height,
                                         DeviceContextSettings ctxSettings) {
#if USE_OPENGL_BACKEND
#if IGL_PLATFORM_WIN
  // TODO: Not tested!
  auto ctx = std::make_unique<igl::opengl::wgl::Context>(GetDC((HWND)window), (HGLRC)context);
  return std::make_unique<igl::opengl::wgl::Device>(std::move(ctx));
#elif IGL_PLATFORM_LINUX
  auto ctx = std::make_unique<igl::opengl::glx::Context>(nullptr,
                                                         display,
                                                         (igl::opengl::glx::GLXDrawable)window,
                                                         (igl::opengl::glx::GLXContext)context);

  return std::make_unique<igl::opengl::glx::Device>(std::move(ctx));
#endif
#else // USE_OPENGL_BACKEND
  const igl::vulkan::VulkanContextConfig cfg{
      .terminateOnValidationError = true,
      .enableValidation = ctxSettings.enableValidation,
      .enableDescriptorIndexing = ctxSettings.enableDescriptorIndexing,
      .swapChainColorSpace = igl::ColorSpace::SRGB_LINEAR,
  };
#ifdef _WIN32
  auto ctx = vulkan::HWDevice::createContext(cfg, (void*)window);
#elif __APPLE__
  auto ctx = vulkan::HWDevice::createContext(cfg, (void*)window);
#elif defined(__linux__)
  auto ctx = vulkan::HWDevice::createContext(cfg, window, 0, nullptr, display);
#else
#error Unsupported OS
#endif

  std::vector<HWDeviceDesc> devices = vulkan::HWDevice::queryDevices(
      *ctx.get(), HWDeviceQueryDesc(HWDeviceType::DiscreteGpu), nullptr);
  if (devices.empty()) {
    devices = vulkan::HWDevice::queryDevices(
        *ctx.get(), HWDeviceQueryDesc(HWDeviceType::IntegratedGpu), nullptr);
  }

  return vulkan::HWDevice::create(std::move(ctx), devices[0], (uint32_t)width, (uint32_t)height);
#endif // USE_OPENGL_BACKEND
}

} // namespace igl
