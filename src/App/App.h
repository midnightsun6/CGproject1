#pragma once

#include "Window/ControlWindow.h"

namespace CG
{
	class App
	{
    private:
        void Update(double dt);
        void Render();

        GLFWwindow* mainWindow;
        int width = 1280, height = 760;

        ControlWindow* controlWindow;
        bool showControlWindow;

        MainScene* mainScene;

        double timeNow = 0;
        double timeLast = 0;
        double timeDelta = 0;

        double limitFPS = 1.f / 60.f;
        double timeDeltaFPS = 0, timer = 0;
        int frames = 0, updates = 0;

    public:
        App();
        ~App();

        auto Initialize() -> bool;
        void Loop();
        void Terminate();
        MainScene* GetMainScene() const
        {
            return mainScene;
        }
	};
}

