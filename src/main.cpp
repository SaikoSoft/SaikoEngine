#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/ImageView.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/GL/Renderer.h>

#include "TexturedTriangleShader.h"

#include <stdexcept>

using namespace Magnum;

class TexturedTriangleExample: public Platform::Application
{
public:
    explicit TexturedTriangleExample(const Arguments &arguments);

private:
    void drawEvent() override;

private:
    GL::Mesh _mesh;
    TexturedTriangleShader _shader;
    GL::Texture2D _texture;
};

TexturedTriangleExample::TexturedTriangleExample(const Arguments& arguments)
    : Platform::Application{arguments, Configuration{}.setTitle("Magnum Textured Triangle Example")}
{
    using namespace Math::Literals;

    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> tga_importer = manager.loadAndInstantiate("TgaImporter");
    if (!tga_importer) {
        throw std::runtime_error("Could not load TgaImporter plugin");
    }

    GL::Renderer::setClearColor(0xdead69_rgbf);

    struct TriangleVertex {
        Vector2 position;
        Vector2 textureCoordinates;
    };
    const TriangleVertex data[]{
        {{-0.5f, -0.5f}, {0.0f, 0.0f}}, // Left vertex
        {{ 0.5f, -0.5f}, {1.0f, 0.0f}}, // Right vertex
        {{ 0.0f,  0.5f}, {0.5f, 1.0f}}, // Top vertex
    };

    GL::Buffer buffer;
    buffer.setData(data);

    _mesh
        .setCount(std::size(data))
        .addVertexBuffer(std::move(buffer),
                         0, // start offset
                         TexturedTriangleShader::Position{},
                         TexturedTriangleShader::TextureCoordinates{});

    const Utility::Resource rs{"textured-triangle-data"};
    if (!tga_importer->openData(rs.getRaw("stone.tga"))) {
        throw std::runtime_error("Failed to load texture");
    }

    Containers::Optional<Trade::ImageData2D> image = tga_importer->image2D(0);
    CORRADE_INTERNAL_ASSERT(image);
    _texture
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setMinificationFilter(GL::SamplerFilter::Linear) // no mipmapping
        .setStorage(1, GL::textureFormat(image->format()), image->size())
        .setSubImage(0, {}, *image);
}

void TexturedTriangleExample::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    using namespace Math::Literals;
    _shader
        .setColor(0xffb2b2_rgbf)
        .bindTexture(_texture)
        .draw(_mesh);

    swapBuffers();
}

MAGNUM_APPLICATION_MAIN(TexturedTriangleExample)
