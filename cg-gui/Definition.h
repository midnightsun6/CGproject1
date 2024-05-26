#pragma once
#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <stack>
#include <sstream>
#include <unordered_map>
#include <set>
#include <cstring>
#include <queue>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <stb\stb_image.h>

#define MAX_BONE_INFLUENCE 4

/*
    Texture ID

    Skybox: 3
    Terrian: 4
    ReflectionSphere: 5
    Particle: 6
    Velocity: 7
    Color: 8
    Model: 9
    Model shadow map: 10
    Mosaic: 11
    Grass: 15, 16, 17
*/





