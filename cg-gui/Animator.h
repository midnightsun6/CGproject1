#pragma once
#include "Definition.h"

struct KeyFrame {
	float time;
	glm::vec3 position;
	glm::vec3 rotationAngle;
	glm::quat rotation;
	glm::vec3 scale;

	KeyFrame(float time) {
		this->time = time;
	}

	KeyFrame(float time, glm::vec3 pos, glm::vec3 rot, glm::vec3 sca) {
		this->time = time;
		this->position = pos;
		this->rotationAngle = rot;
		this->rotation = glm::quat(rotationAngle);
		this->scale = sca;
	}

	KeyFrame(float time, float posX, float posY, float posZ, float angleX, float angleY, float angleZ, float scaleX, float scaleY, float scaleZ) {
		this->time = time;
		position = glm::vec3(posX, posY, posZ);
		rotationAngle = glm::vec3(angleX, angleY, angleZ);
		rotation = glm::quat(glm::vec3(glm::radians(angleX), glm::radians(angleY), glm::radians(angleZ)));
		scale = glm::vec3(scaleX, scaleY, scaleZ);
	}

	bool operator < (const KeyFrame& rhs) const {
		return this->time < rhs.time;
	}

	std::string toString() {
		std::stringstream ss;
		ss << "Time: " << this->time << '\n'
			<< "\tPosition (" << this->position.x << ", " << this->position.y << ", " << this->position.z << ")\n"
			<< "\tRotation (" << rotationAngle.x << ", " << rotationAngle.y << ", " << rotationAngle.z << ")\n"
			<< "\tScale (" << this->scale.x << ", " << this->scale.y << ", " << this->scale.z << ")\n";
		return ss.str();
	};
};

// For each mesh how to move.
struct Track {
	std::vector<KeyFrame> keyFrames;

	glm::mat4 interpolate(float currentTime, float speed) {
		// If no animation, skip it.
		if (keyFrames.empty()) {
			return glm::mat4(1.0f);
		}

		// If current time is greater than last frame of animation, keep it.
		if (currentTime >= keyFrames.back().time / speed) {
			KeyFrame& lastFrame = keyFrames.back();
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), lastFrame.position);
			glm::mat4 rotationMatrix = glm::mat4_cast(lastFrame.rotation);
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), lastFrame.scale);
			return translationMatrix * rotationMatrix * scaleMatrix;
		}

		// Find a exact key frame to play the animation.
		for (int i = 0; i < keyFrames.size() - 1; ++i) {
			KeyFrame& startFrame = keyFrames[i], &endFrame = keyFrames[i + 1];

			float startTime = startFrame.time / speed, endTime = endFrame.time / speed;
			if (startTime <= currentTime && currentTime <= endTime) {
				float progression = (currentTime - startTime) / (endTime - startTime);

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
	float duration = 1, speed = 1;
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
	void play(const char* animationName);
	void clear();

	void addAnimation(AnimationClip animation);
	void deleteAnimation(const char* animation);
	void importAnimation(const char* filename);
	void exportAnimation(const char* filename, const char* animation);

	const std::unordered_map<std::string, AnimationClip>& getAnimations() const;
	const glm::mat4& getAnimationMatrix(std::string part);
};

