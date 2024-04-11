#pragma once
#include "Definition.h"

struct KeyFrame {
	float time;
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	KeyFrame(float time) {
		this->time = time;
	}

	KeyFrame(float time, glm::vec3 pos, glm::vec3 rot, glm::vec3 sca) {
		this->time = time;
		position = pos;
		rot.x = glm::radians(rot.x), rot.y = glm::radians(rot.y), rot.z = glm::radians(rot.z);
		rotation = glm::quat(rot);
		scale = sca;
	}

	KeyFrame(float time, float posX, float posY, float posZ, float angleX, float angleY, float angleZ, float scaleX, float scaleY, float scaleZ) {
		this->time = time;
		position = glm::vec3(posX, posY, posZ);
		rotation = glm::quat(glm::vec3(glm::radians(angleX), glm::radians(angleY), glm::radians(angleZ)));
		scale = glm::vec3(scaleX, scaleY, scaleZ);
	}
};

// For each mesh how to move.
struct Track {
	std::vector<KeyFrame> keyFrames;

	glm::mat4 interpolate(float currentTime) {
		// If no animation, skip it.
		if (keyFrames.empty()) {
			return glm::mat4(1.0f);
		}

		// If current time is greater than last frame of animation, keep it.
		if (currentTime >= keyFrames.back().time) {
			KeyFrame& lastFrame = keyFrames.back();
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), lastFrame.position);
			glm::mat4 rotationMatrix = glm::mat4_cast(lastFrame.rotation);
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), lastFrame.scale);
			return translationMatrix * rotationMatrix * scaleMatrix;
		}

		// Find a exact key frame to play the animation.
		for (int i = 0; i < keyFrames.size() - 1; ++i) {
			KeyFrame& startFrame = keyFrames[i], &endFrame = keyFrames[i + 1];

			if (startFrame.time <= currentTime && currentTime <= endFrame.time) {
				float progression = (currentTime - startFrame.time) / (endFrame.time - startFrame.time);

				glm::vec3 position = glm::mix(startFrame.position, endFrame.position, progression);
				glm::quat rotation = glm::slerp(startFrame.rotation, endFrame.rotation, progression);
				glm::vec3 scale = glm::mix(startFrame.scale, endFrame.scale, progression);

				glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
				glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
				glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
				return translationMatrix * rotationMatrix * scaleMatrix;
			}
		}
		return glm::mat4(1.0f);
	}
};

struct AnimationClip {
	std::string name;
	std::unordered_map<std::string, Track> tracks;
	float duration;
	bool isLoop = false;
};

class Animator {
private:
	float currTime;
	bool isPlaying;

	std::string currAnimation;
	std::unordered_map<std::string, AnimationClip> animations;
	
public:
	Animator();
	~Animator();

	void update(float dt);
	void transitionTo(std::string animationName);
	void addAnimation(AnimationClip animation);

	const glm::mat4& getAnimationMatrix(std::string part);
};

