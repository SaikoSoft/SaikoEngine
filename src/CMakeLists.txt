add_executable(engine main.cpp)
target_link_libraries(engine PRIVATE ${CONAN_LIBS})
target_link_libraries(engine PRIVATE
  Magnum::Application
  Magnum::GL
  Magnum::Magnum
  Magnum::MeshTools
  Magnum::SceneGraph
  Magnum::Shaders
  Magnum::Trade
  Magnum::AnyImageImporter
  Magnum::AnySceneImporter
  Magnum::ObjImporter
  MagnumPlugins::StbImageImporter
)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)
set_property(TARGET engine PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET engine PROPERTY CXX_EXTENSIONS FALSE)

add_executable(tests core/tst/test.cpp)
add_test(NAME tests COMMAND tests)
target_link_libraries(tests ${CONAN_LIBS})
