/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Vulkan/Context/Context.hpp>

#include <Netero/Graphics/Renderer.hpp>

namespace Netero::Gfx {

class RendererImpl final: public Renderer {
    private:
    explicit RendererImpl(Context& aContext);

    public:
    ~RendererImpl() final = default;

    enum class RtCode {
        SUCCESS
    };

    RtCode Build();
    RtCode Teardown();
    RtCode Update();

    void RegisterDrawable() final;
    void UnRegisterDrawable() final;

    private:
    Context& myContext;
};

}
