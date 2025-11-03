#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "krill.hpp"
#include "Stl.hpp"
#include <iostream>
#include <optional>
#include <ctime>
#include <cassert>

int main(int argc, const char *argv[])
{
    // We _need_ an OpenGL rendering context for compute shaders to work
    // In theory you could do a hidden/offscreen rendering context, but this is "good enough"
    SDL_Init(SDL_INIT_VIDEO);
    SDL_DisplayID *displayIds = SDL_GetDisplays(nullptr);
    const SDL_DisplayMode *dm = SDL_GetCurrentDisplayMode(displayIds[0]);

    // create a 1x1 rendering context :)
    SDL_Window *window = SDL_CreateWindow(nullptr, 1, 1, SDL_WINDOW_OPENGL);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    glewInit();

    ComputeShader cs(std::filesystem::path("compute.glsl"));
    ShaderProgram program(cs);
    program.use();
    // https://wikis.khronos.org/opengl/Shader_Storage_Buffer_Object
    ShaderStorageBuffer ssbo;
    std::vector<float> values = {1, 2, 3};
    ssbo.bind();
    ssbo.bindBase(0);
    ssbo.setData(values, GL_DYNAMIC_DRAW);

    glDispatchCompute(3, 1, 1);
    // in theory jsut need to do the barrier for shader storage buffer objects :)
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, values.size() * sizeof(float), values.data());
    std::cout << values[0] << ',' << values[1] << ',' << values[2] << std::endl;

    return 0;
}

