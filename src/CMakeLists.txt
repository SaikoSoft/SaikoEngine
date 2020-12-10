corrade_add_resource(TexturedTriangle_RESOURCES resources.conf)

add_executable(engine main.cpp TexturedTriangleShader.h ${TexturedTriangle_RESOURCES})
target_link_libraries(engine PRIVATE ${CONAN_LIBS})
target_link_libraries(engine PRIVATE
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::Trade
)
add_dependencies(engine Magnum::TgaImporter)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)
set_property(TARGET engine PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET engine PROPERTY CXX_EXTENSIONS FALSE)

add_executable(tests core/tst/test.cpp)
add_test(NAME tests COMMAND tests)
target_link_libraries(tests ${CONAN_LIBS})
