#pragma once

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Attribute.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Context.h>
#include <Corrade/Containers/ReferenceStl.h>
#include <stdexcept>
#include <initializer_list>

using namespace Magnum;

class TexturedTriangleShader: public GL::AbstractShaderProgram {
    public:
        using Position           = GL::Attribute<0, Vector2>;
        using TextureCoordinates = GL::Attribute<1, Vector2>;

        explicit TexturedTriangleShader();

        TexturedTriangleShader& setColor(const Color3& color) {
            setUniform(_colorUniform, color);
            return *this;
        }

        TexturedTriangleShader& bindTexture(GL::Texture2D& texture) {
            texture.bind(TextureUnit);
            return *this;
        }

    private:
        enum: Int {
            TextureUnit = 0,
        };

        Int _colorUniform;
};

TexturedTriangleShader::TexturedTriangleShader() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

    const Utility::Resource rs{"textured-triangle-data"};

    GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL330, GL::Shader::Type::Fragment};

    vert.addSource(rs.get("TexturedTriangleShader.vert"));
    frag.addSource(rs.get("TexturedTriangleShader.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile(std::initializer_list<Corrade::Containers::Reference<GL::Shader>>({vert, frag})));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    _colorUniform = uniformLocation("color");

    setUniform(uniformLocation("textureData"), TextureUnit);
}
