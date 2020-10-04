/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Vulkan/Context/Context.hpp>

#include <Netero/Graphics/Errors.hpp>
#include <Netero/Graphics/Renderer.hpp>

#include <Swapchain/Swapchain.hpp>

namespace Netero::Gfx {

class RendererImpl final: public Renderer {
    public:
    explicit RendererImpl(Context& aContext);
    ~RendererImpl() final = default;

    // Initialization related
    GfxResult Initialize();
    GfxResult Teardown();

    // Runtime related
    GfxResult Build();
    GfxResult Release();
    GfxResult ReBuild();
    GfxResult Update();

    void RegisterDrawable() final;
    void UnRegisterDrawable() final;

    [[nodiscard]] Context& GetContext() const { return myContext; }

    private:
    Context& myContext;
};

} // namespace Netero::Gfx
