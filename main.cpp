#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>

#include "game.h"
#include "common.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Maze 3D", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glewExperimental = GL_TRUE;
    glewInit();
    glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

    // uncomment this call to draw in wireframe polygons.
#ifdef DEBUG
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

    // render loop
    // -----------

    Game *game = Game::Instance();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        game->update(deltaTime);
        game->draw();
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    static const std::map<int, Game::InputKey> keysLookup = {
        { GLFW_KEY_W, Game::KEY_UP_1 },
        { GLFW_KEY_S, Game::KEY_DOWN_1 },
        { GLFW_KEY_A, Game::KEY_LEFT_1 },
        { GLFW_KEY_D, Game::KEY_RIGHT_1 },
        { GLFW_KEY_UP, Game::KEY_UP_2 },
        { GLFW_KEY_DOWN, Game::KEY_DOWN_2 },
        { GLFW_KEY_LEFT, Game::KEY_LEFT_2 },
        { GLFW_KEY_RIGHT, Game::KEY_RIGHT_2 },
        { GLFW_KEY_E, Game::KEY_MOVE_UP },
        { GLFW_KEY_Q, Game::KEY_MOVE_DOWN },
        { GLFW_KEY_M, Game::KEY_MINIMAP },
        { GLFW_KEY_R, Game::KEY_RESET }
    };
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    for (auto it = keysLookup.cbegin(); it != keysLookup.cend(); ++it) {
        if (glfwGetKey(window, it->first) == GLFW_PRESS) {
            Game::Instance()->processKeyInput(it->second, Game::KEY_STATE_PRESSED);
        }
        else if (glfwGetKey(window, it->first) == GLFW_RELEASE){
            Game::Instance()->processKeyInput(it->second, Game::KEY_STATE_RELEASED);
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    Game::Instance()->processMouseInput(xpos, ypos);
}