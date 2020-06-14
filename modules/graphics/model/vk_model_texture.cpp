/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/graphics/model.hpp>


namespace netero::graphics {

    void Model::loadTexture(const std::string& path, TextureSamplingMode samplingMode) {
        this->_textures.loadTexture(path, samplingMode);
    }
}

