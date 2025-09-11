#include <ctime>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <thread>
#include <thread> // Required for std::this_thread
#include <chrono> // Required for std::chrono::milliseconds


#include "krill.hpp"
#include "stb_image.h"
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

ShaderProgram createProgram()
{
    try {
        VertexShader basicVert(std::filesystem::path("shaders/analogclock.vert"));
        FragmentShader redTri(std::filesystem::path("shaders/analogclock.frag"));
        std::cout << "About to return\n";
        return ShaderProgram(basicVert, redTri);
    } catch(...) {
        std::cerr << "Exception creating program." << std::endl;
    }

    throw new std::runtime_error("..");
}

struct wallclock {
    // note that since these aare going to be
    // typedef'd chars, automatic stringification might break down
    float hours;
    float mins;
    float secs;
};

struct vertex { float x, y; };

wallclock get_wallclock()
{
    std::time_t t = std::time(nullptr);
    std::tm local_tm = *std::localtime(&t);

    wallclock wc;
    wc.hours = static_cast<int8_t>(local_tm.tm_hour % 12);
    wc.mins = local_tm.tm_min;
    wc.secs = local_tm.tm_sec;
    return wc;
}

int main()
{
    wallclock wc = get_wallclock();
    std::cout << static_cast<int16_t>(wc.hours) << ':' << static_cast<int16_t>(wc.mins) << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(1));

    int width, height;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *clockface = stbi_load("assets/clockface.png", &width, &height, nullptr, 4);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_DisplayID *displayIds = SDL_GetDisplays(nullptr);
    const SDL_DisplayMode *dm = SDL_GetCurrentDisplayMode(displayIds[0]);
    float target_heightf = dm->h / 1.1;
    float imgRatio = static_cast<float>(width) / static_cast<float>(height);
    int target_width = static_cast<int>(target_heightf * imgRatio);
    char title[50];
    SDL_Window *window = SDL_CreateWindow("Analog Clock", target_width, static_cast<int>(target_heightf), SDL_WINDOW_OPENGL);

    if(window == nullptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    glewInit();

    glClearColor(0,0,0,1);
        glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ArrayBuffer vbo;
    ElementArrayBuffer ebo;
    VAO vao;
    vao.bind();
    vbo.bind();
    std::vector<vertex> vertices = {{-1.0f, 1.0}, //topleft
        {1.0, 1.0}, //topright
        {-1.0, -1.0}, //botleft
        {1.0, -1.0}}; //botright
    vbo.setData(vertices, GL_STATIC_DRAW);

    std::vector<unsigned int> indices = {0, 1, 2, 3};
    ebo.bind();
    ebo.setData(indices, GL_STATIC_DRAW);

    VertexDefinition vd;
    vd.addVec2();
    vbo.useVertexDefinition(vd);

    glActiveTexture(GL_TEXTURE0);
    Texture clockTex = Texture(width, height, clockface);
    stbi_image_free(clockface);
    unsigned char *hours = stbi_load("assets/hourshand.png", &width, &height, nullptr, 4);

    ShaderProgram program = createProgram();
    program.use();
    program.set("clockTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    Texture hoursTex(width, height, hours);
    hoursTex.bind();

    stbi_image_free(hours);

    program.set("hoursHand", 1);
    program.set("hours", wc.hours);

    unsigned char *mins = stbi_load("assets/minsshand.png", &width, &height, nullptr, 4);
    glActiveTexture(GL_TEXTURE2);
    Texture minsTex(width, height, mins);
    minsTex.bind();

    stbi_image_free(mins);
    program.set("minutesHand", 2);
    program.set("mins", wc.mins);
    program.set("secs", wc.secs);


    SDL_GL_SwapWindow(window);

    bool shouldExit = false;
    while(!shouldExit) {
        SDL_Event event;

        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT) {
                shouldExit = true;
            }
        }
        wc = get_wallclock();
        program.set("hours", wc.hours);
        program.set("mins", wc.mins);
        program.set("secs", wc.secs);
        //        glClear(GL_COLOR_BUFFER_BIT);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);
        SDL_GL_SwapWindow(window);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return 0;
}
