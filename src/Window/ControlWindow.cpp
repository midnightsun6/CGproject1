#include "ControlWindow.h"

#include <imgui.h>

#include <iostream>
#include <vector>
#include <string>

namespace CG
{
	ControlWindow::ControlWindow()
	{
		showDemoWindow = false;
	}

	auto ControlWindow::Initialize() -> bool
	{
		return true;
	}

	void ControlWindow::Display()
	{
		ImVec2 center;
		center.x = 50;
		center.y = 50;
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);
		ImGui::Begin("Control");
		{
			ImGui::Checkbox("Demo Window", &showDemoWindow);

			static int clicked = 0;
			if (ImGui::Button("Button"))
				clicked++;
			if (clicked & 1)
			{
				ImGui::SameLine();
				ImGui::Text("Thanks for clicking me!");
			}

			static int modeIndex = 0;
			std::vector<std::string> modes = { "Fill", "Line" };
			ImGui::Text("Mode: ");
			ImGui::SameLine(100);
			ImGui::SetNextItemWidth(150);
			if (ImGui::BeginCombo("##Mode", modes[modeIndex].c_str()))
			{
				for (int n = 0; n < modes.size(); n++)
				{
					const bool is_selected = (modeIndex == n);
					if (ImGui::Selectable(modes[n].c_str(), is_selected))
					{
						modeIndex = n;
						std::cout << "Set Mode " << modeIndex << std::endl;
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

		}
		ImGui::End();

		/* Load Object and Animation Window */
		center.x = 300;
		center.y = 50;
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);
		ImGui::Begin("Load Object and Animation");
		{
			static char path[1024], newName[1024], objname[1024], animation[1024];
			ImGui::InputText("Object Path", path, sizeof(path));
			ImGui::InputText("Name for Object", newName, sizeof(newName));
			if (ImGui::Button("Load Object")) {
				targetScene->loadModel(path, newName);
			}
			ImGui::NewLine();

			ImGui::InputText("Object Name", objname, sizeof(objname));
			ImGui::InputText("Animation Path", animation, sizeof(animation));
			if (ImGui::Button("Load Animation into Object")) {
				targetScene->loadAnimation(objname, animation);
			}
		}
		ImGui::End();

		/* Animation Window */
		center.x = 700;
		center.y = 50;
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);
		ImGui::Begin("Animation");
		{
			static char objname[1024], animation[1024];
			ImGui::InputText("Object Name", objname, sizeof(objname));
			ImGui::InputText("Animation Name", animation, sizeof(animation));
			if (ImGui::Button("Play Animation")) {
				targetScene->playAnimation(objname, animation);
			}
		}
		ImGui::End();

		// Show the big demo window or not (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
