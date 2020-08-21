/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Vulkan/Context/Context.hpp>

#include <Netero/Graphics/Renderer.hpp>

#include <Swapchain/Swapchain.hpp>

namespace Netero::Gfx {

class RendererImpl final: public Renderer {
    public:
    explicit RendererImpl(Context& aContext);
    ~RendererImpl() final = default;

    enum class RtCode { SUCCESS, SWAPCHAIN_OUT_OF_DATE };

    // Initialization related
    RtCode Initialize();
    RtCode Teardown();

    // Runtime related
    RtCode Build();
    RtCode Release();
    RtCode ReBuild();
    RtCode Update();

    void RegisterDrawable() final;
    void UnRegisterDrawable() final;

    private:
    Context& myContext;
};

} // namespace Netero::Gfx
