/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <set>

#include <Vulkan/Context/Context.hpp>

#include <Netero/Graphics/Errors.hpp>
#include <Netero/Graphics/PublicDrawable.hpp>
#include <Netero/Graphics/Renderer.hpp>

#include "Swapchain/Swapchain.hpp"

namespace Netero::Gfx {

class Camera;
class Drawable;

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
    GfxResult Update(Frame&);

    GfxResult RegisterDrawable(std::shared_ptr<PublicDrawable> anObject) final;
    GfxResult UnRegisterDrawable(std::shared_ptr<PublicDrawable> anObject) final;

    void AttachCamera(Camera*) final;
    void DetachCamera() final;

    GfxResult PresentFrame() final;

    bool                   IsSwapchainOutOfDate() const;
    [[nodiscard]] Context& GetContext() const { return myContext; }

    private:
    Context&  myContext;
    Swapchain mySwapchain;
    Camera*   myCamera;

    std::set<std::shared_ptr<Drawable>> myDrawables;
};

} // namespace Netero::Gfx
