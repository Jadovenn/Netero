/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>

#include <Netero/Graphics/Texture.hpp>

#include <>

namespace Netero::Gfx {

class Mesh;

Texture* LoadTextureFromFile(const std::string& aFilePath, TextureSamplingMode aSamplingMode);
Mesh*    LoadMeshFromFile(const std::string& aFilePath);
Shader*  LoadShaderFromFile(const std::string& aFilePath)

} // namespace Netero::Gfx
