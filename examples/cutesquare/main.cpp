#include <iostream>
#include <optional>

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "krill.hpp"

ShaderProgram createProgram()
{
    VertexShader basicVert(std::filesystem::path("shaders/passthru.vert"));
    FragmentShader redTri(std::filesystem::path("shaders/red.frag"));
    return ShaderProgram(basicVert, redTri);
}

struct vec3 {
    float x, y, z, r;
};

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Example", 640, 480, SDL_WINDOW_OPENGL);
    bool shouldExit = false;

    if(window == nullptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    glewInit();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    std::optional<ShaderProgram> redProgramOpt;
    try {
        redProgramOpt = createProgram();
    } catch(const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch(...) {
        std::cerr << "Unknown error." << std::endl;
        return 1;
    }
    std::vector<vec3> vertices = {
        {0.5, 0.5, 0.0, .5},
        {0.5, -0.5, 0.0, .2},
        {-0.5, -0.5, 0.0, .3},
        {-0.5, 0.5, 0.0, .4}
    };
    std::vector<unsigned int> indices = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    ArrayBuffer vbo;
    ElementArrayBuffer ebo;
    VAO vao;

    vbo.bind();
    vbo.setData(vertices, GL_STATIC_DRAW);

    ebo.bind();
    ebo.setData(indices, GL_STATIC_DRAW);

    VertexDefinition vd;
    vd.addVec3();
    vd.addFloat();
    vbo.useVertexDefinition(vd);

    ShaderProgram redProgram = redProgramOpt.value();
    redProgram.use();
    redProgram.set("green", 0.4f);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);

    while(!shouldExit) {
        SDL_Event event;

        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT) {
                shouldExit = true;
            }
        }
    }

    return 0;
}
