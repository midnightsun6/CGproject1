#pragma once

#include <Scene/MainScene.h>

namespace CG
{
	class ControlWindow
	{
	private:
		MainScene* targetScene;

		bool showDemoWindow;

	public:
		ControlWindow();

		auto Initialize() -> bool;
		void Display();

		void SetTargetScene(MainScene* scene)
		{
			targetScene = scene;
		}
	};
}
