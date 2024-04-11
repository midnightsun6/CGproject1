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
	//std::unordered_map<float, glm::mat4> interpolation;

	glm::mat4 interpolate(float currentTime) {
		if (keyFrames.empty()) {
			return glm::mat4(1.0f);
		}

		// 计算循环内的当前时间
		float duration = keyFrames.back().time;
		float loopedTime = std::fmod(currentTime, duration);

		// 找到适当的关键帧进行插值
		for (int i = 0; i < keyFrames.size() - 1; ++i) {
			KeyFrame startFrame = keyFrames[i];
			KeyFrame endFrame = (i + 1 < keyFrames.size()) ? keyFrames[i + 1] : keyFrames.front();

			// 如果当前时间小于下一个关键帧的时间或者已经是最后一个关键帧，则进行插值
			if (loopedTime <= endFrame.time || i == keyFrames.size() - 1) {
				float progression;
				// 如果是最后一个关键帧，则需要特殊处理，因为下一个关键帧是第一个关键帧，时间被重置
				if (i == keyFrames.size() - 1) {
					float endTime = duration + keyFrames.front().time; // 假设循环结束时的虚拟时间点
					progression = (loopedTime + (duration - startFrame.time)) / (endTime - startFrame.time);
				}
				else {
					progression = (loopedTime - startFrame.time) / (endFrame.time - startFrame.time);
				}
				progression = std::abs(progression);

				glm::vec3 position = glm::mix(startFrame.position, endFrame.position, progression);
				glm::quat rotation = glm::slerp(startFrame.rotation, endFrame.rotation, progression);
				std::cout << "time: " << progression << ' ' << "Rotation: " << startFrame.rotation.z << ' ' << endFrame.rotation.z << '\n';
				glm::vec3 scale = glm::mix(startFrame.scale, endFrame.scale, progression);

				glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
				glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
				glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
				return translationMatrix * rotationMatrix * scaleMatrix;
			}
		}
		// 作为后备，返回单位矩阵
		return glm::mat4(1.0f);
	}
};

struct AnimationClip {
	std::string name;
	std::unordered_map<std::string, Track> tracks;
	float duration;
};

class Animator {
private:
	float currTime;

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

