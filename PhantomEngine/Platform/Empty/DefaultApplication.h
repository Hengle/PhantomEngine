#pragma once
// include the basic windows header file
#include "BaseApplication.h"
#include "GraphicsManager.h"
#include "GfxConfiguration.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Phantom {
    class DefaultApplication : public BaseApplication
    {
    public:
        DefaultApplication(GfxConfiguration& config);

        virtual int Initialize();
        virtual void Finalize();
        // One cycle of the main loop
        virtual void Tick();

    private:
            GLFWwindow *m_Window;
            static void windowResize(GLFWwindow *window, int width, int height);
            static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

    };
}

