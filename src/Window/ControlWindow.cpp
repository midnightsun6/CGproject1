﻿#include "ControlWindow.h"

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
		static std::vector<std::string> modelsName;
		static std::vector<const char*> modelsNameDisplay;
		static int modelsIdx = 0;

		static AnimationClip copyAnimation;
		static bool isCopyAnimation = false;

		ImVec2 center;
		center.x = 50;
		center.y = 50;
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);
		ImGui::Begin("Control");
		{
			/* Scene render type and object */
			static std::vector<const char*> renderSceneParticle = {
				"Normal", "Toon", "Mosaic", "Motion blur",
			};

			static std::vector<const char*> renderParticle = {
				"Kamehameha", "Fire", "None",
			};

			static std::vector<const char*> renderReflection = {
				"Mirror", "Water(sin wave)", "Water(height map)", "None",
			};

			static int renderSceneParticleIdx = 0;
			static int renderParticleIdx = 2;
			static int renderReflectionIdx = 3;
			static int modelAmount = 1;
			static float mosaicSize = 0.005, blurscale = 3.0;

			ImGui::Combo("Scene Particle", &renderSceneParticleIdx, renderSceneParticle.data(), renderSceneParticle.size());
			
			if (renderSceneParticleIdx == 0 || renderSceneParticleIdx == 1) {
				ImGui::Combo("Reflection Object", &renderReflectionIdx, renderReflection.data(), renderReflection.size());
				ImGui::Combo("Particle", &renderParticleIdx, renderParticle.data(), renderParticle.size());
				
				targetScene->setRenderReflection((RenderReflection)renderReflectionIdx);
				if (ImGui::Button("Play Particle")) {
					targetScene->setRenderParticle((RenderParticle)renderParticleIdx);
				}

				ImGui::InputInt("Model Amount", &modelAmount, 1, 1000);
				targetScene->setModelAmount(modelAmount);
			}
			else if (renderSceneParticleIdx == 2) {
				ImGui::SliderFloat("Mosaic Block Size", &mosaicSize, 0.001, 1.0, "%.4f");
				targetScene->setMosaicBlockSize(mosaicSize);
			}
			else if (renderSceneParticleIdx == 3) {
				ImGui::SliderFloat("Blur Scale", &blurscale, 0.1, 20.0, "%.3f");
				targetScene->setBlurScale(blurscale);
			}

			targetScene->setRenderSceneParticle((RenderSceneParticle)renderSceneParticleIdx);
			

			/* ImGui default window */
			{
				//ImGui::Checkbox("Demo Window", &showDemoWindow);

				//static int clicked = 0;
				//if (ImGui::Button("Button"))
				//	clicked++;
				//if (clicked & 1)
				//{
				//	ImGui::SameLine();
				//	ImGui::Text("Thanks for clicking me!");
				//}

				//static int modeIndex = 0;
				//std::vector<std::string> modes = { "Fill", "Line" };
				//ImGui::Text("Mode: ");
				//ImGui::SameLine(100);
				//ImGui::SetNextItemWidth(150);
				//if (ImGui::BeginCombo("##Mode", modes[modeIndex].c_str()))
				//{
				//	for (int n = 0; n < modes.size(); n++)
				//	{
				//		const bool is_selected = (modeIndex == n);
				//		if (ImGui::Selectable(modes[n].c_str(), is_selected))
				//		{
				//			modeIndex = n;
				//			std::cout << "Set Mode " << modeIndex << std::endl;
				//		}

				//		// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				//		if (is_selected)
				//		{
				//			ImGui::SetItemDefaultFocus();
				//		}
				//	}
				//	ImGui::EndCombo();
				//}
			}
		}
		ImGui::End();

		/* Load Object and Animation Window */
		center.x = 400;
		center.y = 50;
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);
		ImGui::Begin("Load Object and Animation");
		{
			static char path[1024], newName[1024], animation[1024];
			static float tra[3] = { 0, 0, 0 }, rot[3] = { 0, 0, 0 }, sca[3] = { 1, 1, 1 };

			// Loading Object model
			ImGui::InputText("Object Path", path, sizeof(path));
			ImGui::InputText("Name for Object", newName, sizeof(newName));
			if (ImGui::Button("Load Object")) {
				targetScene->loadModel(path, newName);
				modelsName.push_back(newName);
				modelsNameDisplay.clear();
				for (const auto& s : modelsName) {
					modelsNameDisplay.push_back(s.c_str());
				}
				memset(path, 0, sizeof(path));
				memset(newName, 0, sizeof(newName));
			}
			ImGui::NewLine();

			// Loading Animation of the model.
			ImGui::Combo("Model Name", &modelsIdx, modelsNameDisplay.data(), modelsNameDisplay.size());
			ImGui::InputText("Animation Path", animation, sizeof(animation));
			if (ImGui::Button("Load Animation into Object")) {
				targetScene->importAnimation(modelsNameDisplay[modelsIdx], animation);
			}
			ImGui::NewLine();

			// Parameters of model
			if (!modelsName.empty()) {
				ImGui::Text("Model Parameters");
				ImGui::InputFloat3("Translate", tra);
				ImGui::InputFloat3("Rotation", rot);
				ImGui::InputFloat3("Scale", sca);
				if (ImGui::Button("Apply")) {
					targetScene->transformateModel(modelsNameDisplay[modelsIdx], tra, rot, sca);
				}
			}
		}
		ImGui::End();

		/* Animation Player */
		center.x = 800;
		center.y = 50;
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);
		ImGui::Begin("Animation Player");
		{
			// Play Animation
			static char animation[1024];
			static std::string selectedAnimation;
			static std::unordered_map<std::string, AnimationClip> animations;

			// Dynamic update the animation table of the selected model
			ImGui::Combo("Choose a Model", &modelsIdx, modelsNameDisplay.data(), modelsNameDisplay.size());
			if (!modelsName.empty()) {
				animations = targetScene->getModel(modelsNameDisplay[modelsIdx]).getAnimator().getAnimations();
			}

			ImGui::BeginChild("Animations", ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);
			{
				static bool isExport = false, isDeleteAnimation = false;

				ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.6f, 0.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));

				static std::vector<std::string> remove;
				int i = 0;
				for (auto& [name, animation] : animations) {
					if (ImGui::Selectable(name.c_str())) {
						selectedAnimation = name;
					}
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
						selectedAnimation = name;
					}

					// Add Right-click menu
					std::string popID = "Animation Operation##" + std::to_string(i) + name;
					if (ImGui::BeginPopupContextItem(popID.c_str())) {
						if (ImGui::MenuItem("Copy to Animation Creator")) {
							isCopyAnimation = true;
							copyAnimation = animation;
						}
						if (ImGui::MenuItem("Delete Animation")) {
							isDeleteAnimation = true;
						}
						if (ImGui::MenuItem("Export Animation")) {
							isExport = true;
						}
						ImGui::EndPopup();
					}
					i++;
				}

				ImGui::PopStyleColor(3);

				if (isExport) {
					ImGui::Begin("Export Animation", &isExport);
					{
						static char exportName[1024];

						auto clearExportParameters = [&]() {
							isExport = false;
							memset(exportName, 0, sizeof(exportName));
						};

						ImGui::InputText("Export Filename", exportName, sizeof(exportName));
						if (ImGui::Button("Export")) {
							targetScene->exportAnimation(modelsNameDisplay[modelsIdx], exportName, selectedAnimation.c_str());
							clearExportParameters();
						}
						ImGui::SameLine(0, 30);
						if (ImGui::Button("Cancel")) {
							clearExportParameters();
						}
						
					}
					ImGui::End();
				}

				if (isDeleteAnimation) {
					ImGui::Begin("Delete Animation", &isDeleteAnimation);
					{
						ImGui::Text("Are you sure you want to delete?");

						float windowWidth = ImGui::GetContentRegionAvail().x;
						float itemWidth = ImGui::CalcTextSize("Yes").x;
						ImGui::NewLine();
						ImGui::SameLine((windowWidth - itemWidth) / 3);
						if (ImGui::Button("Yes")) {
							isDeleteAnimation = false;
							remove.push_back(selectedAnimation);
						}

						itemWidth = ImGui::CalcTextSize("No").x;
						ImGui::SameLine((windowWidth - itemWidth) / 3 * 2);
						if (ImGui::Button("No")) {
							isDeleteAnimation = false;
						}
						
					}
					ImGui::End();
				}

				// Delete the animations
				for (std::string& idx : remove) {
					animations.erase(idx);
					targetScene->deleteAnimation(modelsNameDisplay[modelsIdx], idx.c_str());
					if (idx == selectedAnimation) {
						selectedAnimation = "";
					}
				}
				remove.clear();
			}
			ImGui::EndChild();

			if (ImGui::Button("Play Animation")) {
				targetScene->playAnimation(modelsNameDisplay[modelsIdx], selectedAnimation.c_str());
			}

			std::string selectedLabel = "Selected Animation: " + selectedAnimation;
			float windowWidth = ImGui::GetContentRegionAvail().x;
			float itemWidth = ImGui::CalcTextSize(selectedLabel.c_str()).x;
			ImGui::SameLine(windowWidth - itemWidth);
			ImGui::Text(selectedLabel.c_str());
		}
		ImGui::End();

		/* Animation Creator */
		center.x = 1200;
		center.y = 50;
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);
		ImGui::Begin("Animation Creator");
		{
			static std::unordered_map<std::string, Track> tracks;
			static std::string selectedPart = "";
			static float keyframe = 0;
			static float pos[3] = { 0, 0, 0 }, rot[3] = { 0, 0, 0 }, sca[3] = { 1, 1, 1 };
			static bool isAddAnimation = false, isClearAllKF = false, isOnPreviewAnimation = false;

			// Create TreeNode list table for mesh.
			//ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			auto createModelPartsTable = [&](const Mesh& mesh, auto& createModelPartsTable) -> void {
				bool nodeOpen = ImGui::TreeNode(mesh.getName().c_str());

				// Whether by right-clicking
				if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
					selectedPart = mesh.getName();
				}

				// Create the children TreeNode list
				if (nodeOpen) {
					const auto& children = mesh.getChildren();
					for (auto& child : children) {
						createModelPartsTable(child, createModelPartsTable);
					}
					ImGui::TreePop();
				}
			};

			// Copy the parameters to textbox.
			auto copyParameters = [&](const std::string& part, KeyFrame& kf) {
				selectedPart = part;
				keyframe = kf.time;
				pos[0] = kf.position.x, pos[1] = kf.position.y, pos[2] = kf.position.z;
				rot[0] = kf.rotationAngle.x, rot[1] = kf.rotationAngle.y, rot[2] = kf.rotationAngle.z;
				sca[0] = kf.scale.x, sca[1] = kf.scale.y, sca[2] = kf.scale.z;
			};

			// Delete the keyframe on the list.
			auto deleteKeyFrame = [&](Track& track, int& idx) {
				std::vector<KeyFrame>& kfs = track.keyFrames;
				kfs.erase(kfs.begin() + idx);
			};

			// Clear the animation info.
			auto clearAnimationContent = [&] {
				for (int i = 0; i < 3; i++) {
					pos[i] = rot[i] = 0;
					sca[i] = 1;
				}
				tracks.clear();
				keyframe = 0;
				selectedPart = "";
			};

			if (isCopyAnimation) {
				tracks = copyAnimation.tracks;
				isCopyAnimation = false;
			}

			// Create the parts of model list table.
			ImGui::BeginChild("Model Parts", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
			{
				const auto& models = targetScene->getModels();
				for (const auto& [name, model] : models) {
					const auto& meshes = model.getMeshes();
					for (const auto& mesh : meshes) {
						createModelPartsTable(mesh, createModelPartsTable);
					}
				}
			}
			ImGui::EndChild();
			ImGui::NewLine();

			// The tracks and the keyframes of track list table.
			ImGui::BeginChild("Track and Keyframe", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
			{
				std::vector<std::string> remove;
				for (auto& [name, track] : tracks) {
					if (ImGui::TreeNode(name.c_str())) {
						for (int i = 0; i < track.keyFrames.size(); i++) {
							KeyFrame& kf = track.keyFrames[i];

							ImGui::Text(kf.toString().c_str());
							// Add Right-click menu
							std::string popID = "Keyframe Operation##" + std::to_string(i) + name;
							if (ImGui::BeginPopupContextItem(popID.c_str())) {
								if (ImGui::MenuItem("Copy Parameters")) {
									copyParameters(name, kf);
								}
								if (ImGui::MenuItem("Delete Keyframe")) {
									deleteKeyFrame(track, i);
									if (track.keyFrames.empty()) {
										remove.push_back(name);
									}
								}
								if (ImGui::MenuItem("Copy and Delete Keyframe")) {
									copyParameters(name, kf);
									deleteKeyFrame(track, i);
									if (track.keyFrames.empty()) {
										remove.push_back(name);
									}
								}
								ImGui::EndPopup();
							}
						}
						ImGui::TreePop();
					}
				}

				// Delete the empty parts
				for (auto& part : remove) {
					tracks.erase(part);
				}
			}
			ImGui::EndChild();
			ImGui::NewLine();

			// KeyFrame Creator
			ImGui::InputText("Selected Part Name", &selectedPart[0], selectedPart.size() + 1, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("KeyFrame", &keyframe);
			ImGui::InputFloat3("Position", pos);
			ImGui::InputFloat3("Rotation", rot);
			ImGui::InputFloat3("Scale", sca);
			ImGui::SameLine(0, 50);
			if (ImGui::Button("Add KeyFrame")) {
				Track& track = tracks[selectedPart];
				track.keyFrames.push_back(KeyFrame(keyframe, pos[0], pos[1], pos[2], rot[0], rot[1], rot[2], sca[0], sca[1], sca[2]));
				std::sort(track.keyFrames.begin(), track.keyFrames.end());
			}
			if (ImGui::Button("Add Animation")) {
				isAddAnimation = true;
			}

			float windowWidth = ImGui::GetContentRegionAvail().x;
			float itemWidth = ImGui::CalcTextSize("Preview Animation").x;
			ImGui::SameLine((windowWidth - itemWidth) / 2);
			if (ImGui::Button("Preview Animation")) {
				isOnPreviewAnimation = true;
			}

			itemWidth = ImGui::CalcTextSize("Clear All KeyFrame").x;
			ImGui::SameLine(windowWidth - itemWidth);
			if (ImGui::Button("Clear All KeyFrame")) {
				isClearAllKF = true;
			}
			
			// Animation Parameters
			if (isAddAnimation) {
				ImGui::Begin("Animation Parameters", &isAddAnimation);
				{
					static char name[1024];
					static float duration = 0, speed = 1;
					static bool isLoop = false;

					auto clearAddAnimationWindow = [&]() {
						isAddAnimation = false;
						memset(name, 0, sizeof(name));
						duration = 0;
						speed = 1;
						isLoop = false;
					};

					ImGui::Combo("Choose a Model", &modelsIdx, modelsNameDisplay.data(), modelsNameDisplay.size());
					ImGui::InputText("Animation Name", name, sizeof(name));
					ImGui::InputFloat("Duration Time", &duration);
					ImGui::InputFloat("Play Speed", &speed);
					ImGui::Checkbox("Is Loop", &isLoop);

					if (ImGui::Button("Add")) {
						AnimationClip clip;
						clip.tracks = tracks;
						clip.name = name;
						clip.duration = duration;
						clip.speed = speed;
						clip.isLoop = isLoop;
						targetScene->addAnimation(modelsNameDisplay[modelsIdx], clip);

						clearAddAnimationWindow();
						//clearAnimationContent();
					}
					ImGui::SameLine(0, 30);
					if (ImGui::Button("Cancel")) {
						clearAddAnimationWindow();
					}
					
					
				}
				ImGui::End();
			}

			if (isOnPreviewAnimation) {
				static bool isPlaying = false;
				const Animator& animator = targetScene->getModel(modelsNameDisplay[modelsIdx]).getAnimator();

				if (!isPlaying) {
					AnimationClip clip;
					clip.tracks = tracks;
					clip.name = "####Preview Animation####";
					clip.isLoop = false;
					clip.speed = 1;
					for (auto& [_, track] : tracks) {
						clip.duration = std::max(clip.duration, track.keyFrames.back().time);
					}

					targetScene->addAnimation(modelsNameDisplay[modelsIdx], clip);
					targetScene->playAnimation(modelsNameDisplay[modelsIdx], clip.name.c_str());
					isPlaying = true;
				}
				else if (isPlaying && !animator.isOnPlaying()) {
					targetScene->deleteAnimation(modelsNameDisplay[modelsIdx], "####Preview Animation####");
					isOnPreviewAnimation = false;
					isPlaying = false;
				}
			}

			if (isClearAllKF) {
				ImGui::Begin("Clear All KeyFrames", &isClearAllKF);
				{
					ImGui::Text("Do you want to clear all keyframes?");

					float windowWidth = ImGui::GetContentRegionAvail().x;
					float itemWidth = ImGui::CalcTextSize("Yes").x;
					ImGui::NewLine();
					ImGui::SameLine((windowWidth - itemWidth) / 3);
					if (ImGui::Button("Yes")) {
						isClearAllKF = false;
						clearAnimationContent();
					}

					itemWidth = ImGui::CalcTextSize("No").x;
					ImGui::SameLine((windowWidth - itemWidth) / 3 * 2);
					if (ImGui::Button("No")) {
						isClearAllKF = false;
					}
				}
				ImGui::End();
			}
		}
		ImGui::End();

		// Show the big demo window or not (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
