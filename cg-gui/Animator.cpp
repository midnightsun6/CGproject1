#include "Animator.h"

Animator::Animator() {
	currTime = 0;
	isPlaying = false;
}

Animator::~Animator() {}

void Animator::update(float dt) {
	if (!isPlaying) return;

	if (animations.count(currAnimation)) {
		AnimationClip& clip = animations[currAnimation];

		currTime += dt;
		if (currTime > clip.duration) {
			isPlaying = clip.isLoop;
			currTime = fmod(currTime, clip.duration);
		}
	}
}

void Animator::addAnimation(AnimationClip animation) {
	animations[animation.name] = animation;
}

void Animator::addAnimation(const char* filename) {
	std::string filenameStr = filename;
	std::string path = "./res/animations/" + filenameStr;

	std::ifstream fin(path);
	if (fin.fail()) {
		std::cout << "Open file failed from" << path << '\n';
	}

	// Get the basic animation information
	AnimationClip clip;
	fin >> clip.name >> clip.duration >> clip.isLoop >> clip.speed;

	int parts;
	fin >> parts;
	for (int i = 0; i < parts; i++) {
		fin.get(); // To read a extra space

		std::string part;
		std::getline(fin, part);
		int keyAmount;
		fin >> keyAmount;

		std::vector<KeyFrame> keyFrames;
		for (int j = 0; j < keyAmount; j++) {
			// Read keyframe information
			float time, posX, posY, posZ, angleX, angleY, angleZ, scaX, scaY, scaZ;
			fin >> time >> posX >> posY >> posZ >> angleX >> angleY >> angleZ >> scaX >> scaY >> scaZ;
			keyFrames.push_back(KeyFrame(time, posX, posY, posZ, angleX, angleY, angleZ, scaX, scaY, scaZ));
		}

		// Add a track into animation
		Track track;
		track.keyFrames = keyFrames;
		clip.tracks[part] = track;
	}

	// Add the animation
	animations[clip.name] = clip;

	fin.close();
	std::cout << "Added animation successfully: " << filename << '\n';
}

void Animator::play(const char* animationName) {
	if (animations.count(animationName)) {
		std::cout << "Play animation: " << animationName << '\n';
		currAnimation = animationName;
		currTime = 0;
		isPlaying = true;
	}
}

const glm::mat4& Animator::getAnimationMatrix(std::string part) {
	AnimationClip& clip = animations[currAnimation];
	Track& track = clip.tracks[part];
	if (isPlaying) {
		return track.interpolate(currTime, clip.speed);
	}
	return track.interpolate(clip.duration, clip.speed);
}
