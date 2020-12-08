#include <Corrade/Utility/DebugStl.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Angle.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Trade/MeshData.h>
#include <utility>

using namespace Magnum;

class PrimitiveExample: public Platform::Application
{
public:
    explicit PrimitiveExample(const Arguments& arguments);

private:
    void drawEvent() override;
    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;

private:
    GL::Mesh _mesh;
    Shaders::Phong _shader;
    Matrix4 _transformation;
    Matrix4 _projection;
    Color3 _color;
};

PrimitiveExample::PrimitiveExample(const Arguments& arguments)
    : Platform::Application{arguments, Configuration{}.setTitle("Primitive Example")}
{
    using namespace Math::Literals;
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::setClearColor(0xdead69_rgbf);

    Trade::MeshData cube = Primitives::cubeSolid();

    GL::Buffer vertices;
    vertices.setData(MeshTools::interleave(cube.positions3DAsArray(), cube.normalsAsArray()));

    auto [idx_buffer, idx_type] = MeshTools::compressIndices(cube.indicesAsArray());
    GL::Buffer indices;
    indices.setData(idx_buffer);

    _mesh
        .setPrimitive(cube.primitive())
        .setCount(cube.indexCount())
        .addVertexBuffer(std::move(vertices), 0, Shaders::Phong::Position{}, Shaders::Phong::Normal{})
        .setIndexBuffer(std::move(indices), 0, idx_type);

    _transformation = Matrix4::rotationX(30.0_degf) * Matrix4::rotationY(40.0_degf);
    _projection = Matrix4::perspectiveProjection(35.0_degf, Vector2{windowSize()}.aspectRatio(), 0.01f, 100.0f)
        * Matrix4::translation(Vector3::zAxis(-10.0f));
    _color = Color3::fromHsv({35.0_degf, 1.0f, 1.0f});
}

void PrimitiveExample::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    _shader
        .setLightPositions({{7.0f, 5.0f, 2.5f, 0.0f}})
        .setDiffuseColor(_color)
        .setAmbientColor(Color3::fromHsv({_color.hue(), 1.0f, 0.3f}))
        .setTransformationMatrix(_transformation)
        .setNormalMatrix(_transformation.normalMatrix())
        .setProjectionMatrix(_projection)
        .draw(_mesh);

    swapBuffers();
}

void PrimitiveExample::mousePressEvent(MouseEvent& event)
{
    if (event.button() != MouseEvent::Button::Left) {
        return;
    }

    event.setAccepted();
}

void PrimitiveExample::mouseReleaseEvent(MouseEvent& event)
{
    using namespace Math::Literals;
    _color = Color3::fromHsv({_color.hue() + 50.0_degf, 1.0f, 1.0f});

    event.setAccepted();
    redraw();
}

void PrimitiveExample::mouseMoveEvent(MouseMoveEvent& event)
{
    if (!(event.buttons() & MouseMoveEvent::Button::Left)) {
        return;
    }

    Vector2 delta = 3.0f*Vector2{event.relativePosition()}/Vector2{windowSize()};
    _transformation = Matrix4::rotationX(Rad{delta.y()}) * _transformation * Matrix4::rotationY(Rad{delta.x()});

    event.setAccepted();
    redraw();
}

MAGNUM_APPLICATION_MAIN(PrimitiveExample)
