#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "krill.hpp"
#include "Stl.hpp"
#include <iostream>
#include <optional>
#include <ctime>
#include <cassert>
using namespace::krill;
ShaderProgram createProgram()
{
    VertexShader basicVert(std::filesystem::path("vertex.glsl"));
    FragmentShader redTri(std::filesystem::path("fragment.glsl"));
    return ShaderProgram(basicVert, redTri);
}

struct f3 {
    float x, y, z;
};
std::vector<f3> getNormalizedModel(const char *path);

int main(int argc, char *argv[])
{
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <model filepath>" << std::endl;
        return 1;
    }

    auto tris = getNormalizedModel(argv[1]);
    std::cout << tris.size() / 9 << " triangles" << std::endl;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_DisplayID *displayIds = SDL_GetDisplays(nullptr);
    const SDL_DisplayMode *dm = SDL_GetCurrentDisplayMode(displayIds[0]);
    char title[50];
    snprintf(title, 50, "modelview: %s", argv[1]);
    SDL_Window *window = SDL_CreateWindow(title, 800, 800, SDL_WINDOW_OPENGL);
    if(window == nullptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderProgram program = createProgram();
    ArrayBuffer vbo;
    VAO vao;
    vao.bind();
    vbo.bind();
    vbo.setData(tris, GL_STATIC_DRAW);
    VertexDefinition vd;
    vd.addVec3();
    vbo.useVertexDefinition(vd);
    program.use();
    glDrawArrays(GL_TRIANGLES, 0, tris.size());
    SDL_GL_SwapWindow(window);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    bool shouldExit = false;
    while(!shouldExit) {
        SDL_Event event;
        program.use();
        program.set("ticks", ((float)SDL_GetTicks()) / 1000.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, tris.size());
        SDL_GL_SwapWindow(window);

        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT) {
                shouldExit = true;
            }
        }
    }
    return 0;

}

std::vector<f3> getNormalizedModel(const char *path)
{
    StlModel model = StlModel(std::filesystem::path(path));
    std::vector<Triangle> tris = model.getSimpleTriangles();
    BoundingBox bb(tris);
    normalizeTriangles(tris, bb);
    std::vector<f3> results;
    for(Triangle &t : tris) {
        for(Vertex3d &v : t.vertices) {
            f3 out;
            out.x = v.x;
            out.y = v.y;
            out.z = v.z;
            results.push_back(out);
        }
    }
    return results;
}

