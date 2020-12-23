#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Magnum/ImageView.h>
#include <Magnum/Mesh.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <memory>
#include <stdexcept>

using namespace Magnum;
using namespace Math::Literals;

using Object3D = SceneGraph::Object<SceneGraph::MatrixTransformation3D>;
using Scene3D = SceneGraph::Scene<SceneGraph::MatrixTransformation3D>;

class ColoredDrawable: public SceneGraph::Drawable3D {
    public:
        explicit ColoredDrawable(
            Object3D& object,
            Shaders::Phong& shader,
            GL::Mesh& mesh,
            const Color4& color,
            SceneGraph::DrawableGroup3D& group
        ) : SceneGraph::Drawable3D{object, &group}, _shader(shader), _mesh(mesh), _color{color} {}
        
    private:
        void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

        Shaders::Phong& _shader;
        GL::Mesh& _mesh;
        Color4 _color;
};

void ColoredDrawable::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) {
    _shader
        .setDiffuseColor(_color)
        .setLightPositions({
            {camera.cameraMatrix().transformPoint({-3.0f, 10.0f, 10.0f}), 0.0f}
        })
        .setTransformationMatrix(transformationMatrix)
        .setNormalMatrix(transformationMatrix.normalMatrix())
        .setProjectionMatrix(camera.projectionMatrix())
        .draw(_mesh);
}

class TexturedDrawable: public SceneGraph::Drawable3D {
    public:
        explicit TexturedDrawable(
            Object3D& object,
            Shaders::Phong& shader,
            GL::Mesh& mesh,
            GL::Texture2D& texture,
            SceneGraph::DrawableGroup3D& group
        ): SceneGraph::Drawable3D{object, &group}, _shader(shader), _mesh(mesh), _texture(texture) {}

    private:
        void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

        Shaders::Phong& _shader;
        GL::Mesh& _mesh;
        GL::Texture2D& _texture;
};

void TexturedDrawable::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) {
    _shader
        .bindDiffuseTexture(_texture)
        .setLightPositions({
            {camera.cameraMatrix().transformPoint({-3.0f, 10.0f, 10.0f}), 0.0f}
        })
        .setTransformationMatrix(transformationMatrix)
        .setNormalMatrix(transformationMatrix.normalMatrix())
        .setProjectionMatrix(camera.projectionMatrix())
        .draw(_mesh);
}

class ViewerExample: public Platform::Application
{
    public:
        explicit ViewerExample(const Arguments& arguments);

    private:
        void drawEvent() override;
        void viewportEvent(ViewportEvent& event) override;
        void keyPressEvent(KeyEvent& event) override;
        void keyReleaseEvent(KeyEvent& event) override;
        void mousePressEvent(MouseEvent& event) override;
        void mouseReleaseEvent(MouseEvent& event) override;
        void mouseMoveEvent(MouseMoveEvent& event) override;
        void mouseScrollEvent(MouseScrollEvent& event) override;
        void textInputEvent(TextInputEvent& event) override;

        Vector3 positionOnSphere(const Vector2i& position) const;

        void addObject(
            Trade::AbstractImporter& importer,
            Containers::ArrayView<const Containers::Optional<Trade::PhongMaterialData>> materials,
            Object3D& parent,
            UnsignedInt i);

    private:
        Shaders::Phong _coloredShader;
        Shaders::Phong _texturedShader{Shaders::Phong::Flag::DiffuseTexture};
        Containers::Array<Containers::Optional<GL::Mesh>> _meshes;
        Containers::Array<Containers::Optional<GL::Texture2D>> _textures;

        ImGuiIntegration::Context _imgui{NoCreate};
        bool _showDemoWindow = true;
        bool _showAnotherWindow = false;
        Color4 _clearColor = 0x72909aff_rgbaf;
        Float _floatValue = 0.0f;

        Scene3D _scene;
        Object3D _manipulator;
        Object3D _cameraObject;
        std::unique_ptr<SceneGraph::Camera3D> _camera;
        SceneGraph::DrawableGroup3D _drawables;
        Vector3 _previousPosition;
};

ViewerExample::ViewerExample(const Arguments& arguments):
    Platform::Application{arguments, Configuration{}
        .setTitle("Magnum Viewer Example")
        .setWindowFlags(Configuration::WindowFlag::Resizable)},
    _imgui{ImGuiIntegration::Context(Vector2{windowSize()}/dpiScaling(), windowSize(), framebufferSize())}
{
    Utility::Arguments args;
    args.addArgument("file").setHelp("file", "file to load")
        .addOption("importer", "AnySceneImporter").setHelp("importer", "importer plugin to use")
        .addSkippedPrefix("magnum", "engine-specific options")
        .setGlobalHelp("Displays a 3D scene file provided on command line.")
        .parse(arguments.argc, arguments.argv);

    /* Set up proper blending to be used by ImGui. There's a great chance
       you'll need this exact behavior for the rest of your scene. If not, set
       this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha, GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    /* Every scene needs a camera */
    _cameraObject
        .setParent(&_scene)
        .translate(Vector3::zAxis(5.0f));
    _camera = std::make_unique<SceneGraph::Camera3D>(_cameraObject);
    (*_camera)
        .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
        .setViewport(GL::defaultFramebuffer.viewport().size());

    /* Base object, parent of all (for easy manipulation) */
    _manipulator.setParent(&_scene);

    /* Setup renderer and shader defaults */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    _coloredShader
        .setAmbientColor(0x111111_rgbf)
        .setSpecularColor(0xffffff_rgbf)
        .setShininess(80.0f);
    _texturedShader
        .setAmbientColor(0x111111_rgbf)
        .setSpecularColor(0x111111_rgbf)
        .setShininess(80.0f);

    /* Load a scene importer plugin */
    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate(args.value("importer"));
    if (!importer) {
        throw std::runtime_error{"Failed to load importer plugin"};
    }

    Debug{} << "Opening file" << args.value("file");

    /* Load file */
    if (!importer->openFile(args.value("file"))) {
        throw std::runtime_error{"Failed to load file"};
    }

    /* Load all textures. Textures that fail to load will be NullOpt. */
    _textures = Containers::Array<Containers::Optional<GL::Texture2D>>{importer->textureCount()};
    for (UnsignedInt i = 0; i != importer->textureCount(); ++i) {
        Debug{} << "Importing texture" << i << importer->textureName(i);

        Containers::Optional<Trade::TextureData> textureData = importer->texture(i);
        if (!textureData || textureData->type() != Trade::TextureData::Type::Texture2D) {
            Warning{} << "Cannot load texture properties, skipping";
            continue;
        }

        Debug{} << "Importing image" << textureData->image() << importer->image2DName(textureData->image());

        Containers::Optional<Trade::ImageData2D> imageData = importer->image2D(textureData->image());
        GL::TextureFormat format;
        if (imageData && imageData->format() == PixelFormat::RGB8Unorm) {
            format = GL::TextureFormat::RGB8;
        } else if (imageData && imageData->format() == PixelFormat::RGBA8Unorm) {
            format = GL::TextureFormat::RGBA8;
        } else {
            Warning{} << "Cannot load texture image, skipping";
            continue;
        }

        /* Configure the texture */
        GL::Texture2D texture;
        texture
            .setMagnificationFilter(textureData->magnificationFilter())
            .setMinificationFilter(textureData->minificationFilter(), textureData->mipmapFilter())
            .setWrapping(textureData->wrapping().xy())
            .setStorage(Math::log2(imageData->size().max()) + 1, format, imageData->size())
            .setSubImage(0, {}, *imageData)
            .generateMipmap();

        _textures[i] = std::move(texture);
    }

    /* Load all materials. Materials that fail to load will be NullOpt. The
       data will be stored directly in objects later, so save them only
       temporarily. */
    Containers::Array<Containers::Optional<Trade::PhongMaterialData>> materials{importer->materialCount()};
    for (UnsignedInt i = 0; i != importer->materialCount(); ++i) {
        Debug{} << "Importing material" << i << importer->materialName(i);

        Containers::Optional<Trade::MaterialData> materialData = importer->material(i);
        if (!materialData || !(materialData->types() & Trade::MaterialType::Phong)) {
            Warning{} << "Cannot load material, skipping";
            continue;
        }

        materials[i] = std::move(static_cast<Trade::PhongMaterialData&>(*materialData));
    }

    /* Load all meshes. Meshes that fail to load will be NullOpt. */
    _meshes = Containers::Array<Containers::Optional<GL::Mesh>>{importer->meshCount()};
    for (UnsignedInt i = 0; i != importer->meshCount(); ++i) {
        Debug{} << "Importing mesh" << i << importer->meshName(i);

        Containers::Optional<Trade::MeshData> meshData = importer->mesh(i);
        if (!meshData || !meshData->hasAttribute(Trade::MeshAttribute::Normal) || meshData->primitive() != MeshPrimitive::Triangles) {
            Warning{} << "Cannot load the mesh, skipping";
            continue;
        }

        /* Compile the mesh */
        _meshes[i] = MeshTools::compile(*meshData);
    }

    /* Load the scene */
    if (importer->defaultScene() != -1) {
        Debug{} << "Adding default scene" << importer->sceneName(importer->defaultScene());

        Containers::Optional<Trade::SceneData> sceneData = importer->scene(importer->defaultScene());
        if (!sceneData) {
            Error{} << "Cannot load scene, exiting";
            return;
        }

        /* Recursively add all children */
        for (UnsignedInt objectId : sceneData->children3D()) {
            addObject(*importer, materials, _manipulator, objectId);
        }

    /* The format has no scene support, display just the first loaded mesh with
       a default material and be done with it */
    } else if (!_meshes.empty() && _meshes[0]) {
        new ColoredDrawable{_manipulator, _coloredShader, *_meshes[0], 0xffffff_rgbf, _drawables};
    }
}

void ViewerExample::addObject(
    Trade::AbstractImporter& importer,
    Containers::ArrayView<const Containers::Optional<Trade::PhongMaterialData>> materials,
    Object3D& parent,
    UnsignedInt i
) {
    Debug{} << "Importing object" << i << importer.object3DName(i);
    Containers::Pointer<Trade::ObjectData3D> objectData = importer.object3D(i);
    if (!objectData) {
        Error{} << "Cannot import object, skipping";
        return;
    }

    /* Add the object to the scene and set its transformation */
    auto* object = new Object3D{&parent};
    object->setTransformation(objectData->transformation());

    /* Add a drawable if the object has a mesh and the mesh is loaded */
    if (objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh && objectData->instance() != -1 && _meshes[objectData->instance()]) {
        const Int materialId = static_cast<Trade::MeshObjectData3D*>(objectData.get())->material();

        /* Material not available / not loaded, use a default material */
        if (materialId == -1 || !materials[materialId]) {
            new ColoredDrawable{*object, _coloredShader, *_meshes[objectData->instance()], 0xffffff_rgbf, _drawables};

        /* Textured material. If the texture failed to load, again just use a
           default colored material. */
        } else if (materials[materialId]->hasAttribute(Trade::MaterialAttribute::DiffuseTexture)) {
            Containers::Optional<GL::Texture2D>& texture = _textures[materials[materialId]->diffuseTexture()];
            if (texture) {
                new TexturedDrawable{*object, _texturedShader, *_meshes[objectData->instance()], *texture, _drawables};
            } else {
                new ColoredDrawable{*object, _coloredShader, *_meshes[objectData->instance()], 0xffffff_rgbf, _drawables};
            }

        /* Color-only material */
        } else {
            new ColoredDrawable{*object, _coloredShader, *_meshes[objectData->instance()], materials[materialId]->diffuseColor(), _drawables};
        }
    }

    /* Recursively add children */
    for (std::size_t id : objectData->children()) {
        addObject(importer, materials, *object, id);
    }
}

void ViewerExample::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    _camera->draw(_drawables);

    _imgui.newFrame();

    /* Enable text input, if needed */
    if (ImGui::GetIO().WantTextInput && !isTextInputActive()) {
        startTextInput();
    } else if (!ImGui::GetIO().WantTextInput && isTextInputActive()) {
        stopTextInput();
    }

    /* 1. Show a simple window.
       Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appear in
       a window called "Debug" automatically */
    {
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("Float", &_floatValue, 0.0f, 1.0f);
        if (ImGui::ColorEdit3("Clear Color", _clearColor.data())) {
            GL::Renderer::setClearColor(_clearColor);
        }
        if (ImGui::Button("Test Window")) {
            _showDemoWindow ^= true;
        }
        if (ImGui::Button("Another Window")) {
            _showAnotherWindow ^= true;
        }
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0/Double(ImGui::GetIO().Framerate), Double(ImGui::GetIO().Framerate));
    }

    /* 2. Show another simple window, now using an explicit Begin/End pair */
    if (_showAnotherWindow) {
        ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Another Window", &_showAnotherWindow);
        ImGui::Text("Hello");
        ImGui::End();
    }

    /* 3. Show the ImGui demo window. Most of the sample code is in
       ImGui::ShowDemoWindow() */
    if (_showDemoWindow) {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow();
    }

    /* Update application cursor */
    _imgui.updateApplicationCursor(*this);

    /* Set appropriate states. If you only draw ImGui, it is sufficient to
       just enable blending and scissor test in the constructor. */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);

    _imgui.drawFrame();

    /* Reset state. Only needed if you want to draw something else with
       different state after. */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    redraw();

    swapBuffers();
}

void ViewerExample::viewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    _imgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(), event.windowSize(), event.framebufferSize());
    _camera->setViewport(event.windowSize());
}

void ViewerExample::keyPressEvent(KeyEvent& event) {
    if (_imgui.handleKeyPressEvent(event)) {
        return;
    }
}

void ViewerExample::keyReleaseEvent(KeyEvent& event) {
    if (_imgui.handleKeyReleaseEvent(event)) {
        return;
    }
}

void ViewerExample::mousePressEvent(MouseEvent& event) {
    if (_imgui.handleMousePressEvent(event)) {
        return;
    }
    if (event.button() == MouseEvent::Button::Left) {
        _previousPosition = positionOnSphere(event.position());
    }
}

void ViewerExample::mouseReleaseEvent(MouseEvent& event) {
    if (_imgui.handleMouseReleaseEvent(event)) {
        return;
    }
    if (event.button() == MouseEvent::Button::Left) {
        _previousPosition = Vector3();
    }
}

void ViewerExample::mouseScrollEvent(MouseScrollEvent& event) {
    if (_imgui.handleMouseScrollEvent(event)) {
        /* Prevent scrolling the page */
        event.setAccepted();
        return;
    }
    if (!event.offset().y()) {
        return;
    }

    /* Distance to origin */
    const Float distance = _cameraObject.transformation().translation().z();

    /* Move 15% of the distance back or forward */
    _cameraObject.translate(Vector3::zAxis(
        distance * (1.0f - (event.offset().y() > 0 ? 1/0.85f : 0.85f))));

    redraw();
}

Vector3 ViewerExample::positionOnSphere(const Vector2i& position) const {
    const Vector2 positionNormalized = Vector2{position}/Vector2{_camera->viewport()} - Vector2{0.5f};
    const Float length = positionNormalized.length();
    const Vector3 result{positionNormalized, std::max(1.0f - length, 0.0f)};
    return (result * Vector3::yScale(-1.0f)).normalized();
}

void ViewerExample::mouseMoveEvent(MouseMoveEvent& event) {
    if (_imgui.handleMouseMoveEvent(event)) {
        return;
    }
    if (!(event.buttons() & MouseMoveEvent::Button::Left)) {
        return;
    }

    const Vector3 currentPosition = positionOnSphere(event.position());
    const Vector3 axis = Math::cross(_previousPosition, currentPosition);

    if (_previousPosition.length() < 0.001f || axis.length() < 0.001f) {
        return;
    }

    _manipulator.rotate(Math::angle(_previousPosition, currentPosition), axis.normalized());
    _previousPosition = currentPosition;

    redraw();
}

void ViewerExample::textInputEvent(TextInputEvent& event) {
    if (_imgui.handleTextInputEvent(event)) {
        return;
    }
}

MAGNUM_APPLICATION_MAIN(ViewerExample)
