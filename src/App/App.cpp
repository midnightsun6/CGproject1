#include "App.h"

namespace CG
{
	App::App()
	{
		mainWindow = nullptr;

		controlWindow = nullptr;
		showControlWindow = true;

		mainScene = nullptr;
	}

	App::~App() {}

	auto App::Initialize() -> bool
	{
		// Set error callback
		glfwSetErrorCallback([](int error, const char* description)
			{ fprintf(stderr, "GLFW Error %d: %s\n", error, description); });

		// Initialize GLFW
		if (!glfwInit())
			return false;

		// GL 4.6 + GLSL 460
		const char* glsl_version = "#version 460";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

		// Create window with graphics context
		mainWindow = glfwCreateWindow(width, height, "cg-gui", nullptr, nullptr);
		if (mainWindow == nullptr)
			return false;
		glfwMakeContextCurrent(mainWindow);
		glfwSwapInterval(1); // Enable vsync

		// Initialize GLEW
		glewExperimental = GL_TRUE;
		GLenum glew_err = glewInit();
		if (glew_err != GLEW_OK)
		{
			throw std::runtime_error(std::string("Error initializing GLEW, error: ") + (const char*)glewGetErrorString(glew_err));
			return false;
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
		io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
		io.DisplaySize = ImVec2(width, height);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		glfwSetWindowUserPointer(mainWindow, this);
		glfwSetFramebufferSizeCallback(
			mainWindow,
			[](GLFWwindow* window, int w, int h)
			{
				glViewport(0, 0, w, h);
				auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
				auto mainScene = app->GetMainScene();
				mainScene->OnResize(w, h);
			}
		);

		glfwSetKeyCallback(
			mainWindow,
			[](GLFWwindow* window, int key, int scancode, int action, int mode)
			{
				ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mode);

				ImGuiIO& io = ImGui::GetIO();
				if (!io.WantCaptureKeyboard) {
					auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
					auto mainScene = app->GetMainScene();
					mainScene->input->OnKeyboard(key, action);
				}
			}
		);

		glfwSetCursorPosCallback(
			mainWindow,
			[](GLFWwindow* window, double xpos, double ypos) {
				ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

				ImGuiIO& io = ImGui::GetIO();
				if (!io.WantCaptureMouse) {
					auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
					auto mainScene = app->GetMainScene();
					mainScene->input->OnCursorMove(xpos, ypos);
				}
			}
		);

		glfwSetMouseButtonCallback(
			mainWindow,
			[](GLFWwindow* window, int button, int action, int mods) {
				ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

				ImGuiIO& io = ImGui::GetIO();
				if (!io.WantCaptureMouse) {
					auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
					auto mainScene = app->GetMainScene();
					mainScene->input->OnMouseButton(button, action, mods);
				}
			}
		);

		glfwSetScrollCallback(
			mainWindow,
			[](GLFWwindow* window, double xoffset, double yoffset) {
				ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

				ImGuiIO& io = ImGui::GetIO();
				if (!io.WantCaptureMouse) {
					auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
					auto mainScene = app->GetMainScene();
					mainScene->input->OnMouseScroll(xoffset, yoffset);
				}
			}
		);

		controlWindow = new ControlWindow();
		controlWindow->Initialize();

		mainScene = new MainScene();
		mainScene->Initialize();

		controlWindow->SetTargetScene(mainScene);

		// Initialization done
		return true;
	}

	void App::Loop()
	{
		while (!glfwWindowShouldClose(mainWindow))
		{
			// Poll and handle events (inputs, window resize, etc.)
			// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
			// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
			// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
			// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			glfwPollEvents();

			timeNow = glfwGetTime();
			timeDelta = timeNow - timeLast;
			timeLast = timeNow;

			// Limit 60 FPS
			timeDeltaFPS += timeDelta / limitFPS;
			while (timeDeltaFPS >= 1.0f) {
				Update(timeDelta);
				updates++;
				timeDeltaFPS--;
			}
			frames++;

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (showControlWindow)
			{
				controlWindow->Display();
			}

			// GUI Rendering
			ImGui::Render();

			// Render 3D scene
			Render();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Update and Render additional Platform Windows
			// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
			//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
			ImGuiIO& io = ImGui::GetIO();
			(void)io;
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			glfwSwapBuffers(mainWindow);

			if (glfwGetTime() - timer > 1.0) {
				timer++;
				//std::cout << frames << ' ' << updates << '\n';
				frames = 0, updates = 0;
			}
		}
	}

	void App::Terminate()
	{
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(mainWindow);
		glfwTerminate();
	}

	void App::Update(double dt)
	{
		mainScene->Update(dt);
	}

	void App::Render()
	{
		int display_w, display_h;
		glfwGetFramebufferSize(mainWindow, &display_w, &display_h);

		mainScene->Render();
	}
}