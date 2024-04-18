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

void Animator::importAnimation(const char* filename) {
	std::string filenameStr = filename;
	std::string path = "./res/animations/" + filenameStr;

	std::ifstream fin(path);
	if (fin.fail()) {
		std::cout << "Open file failed from" << path << '\n';
		return;
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
			fin >> time 
				>> posX		>> posY		>> posZ 
				>> angleX	>> angleY	>> angleZ 
				>> scaX		>> scaY		>> scaZ;
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

void Animator::exportAnimation(const char* filename, const char* animation) {
	std::string filenameStr = filename;
	std::string path = "./res/animations/" + filenameStr + ".objani";

	std::ofstream fout(path);
	if (fout.fail()) {
		std::cout << "Fail to import the animation\n";
		return;
	}
	if (!animations.count(animation)) {
		std::cout << "Has no animation name of " << animation << '\n';
		return;
	}

	// Animation info
	const AnimationClip& clip = animations.at(animation);
	fout 
		<< clip.name << '\n'
		<< clip.duration << '\n'
		<< clip.isLoop << '\n'
		<< clip.speed << '\n';

	// The number of parts
	fout << clip.tracks.size() << '\n';

	// Animation keyframes
	for (auto& [name, track] : clip.tracks) {
		// The number of keyframes
		fout
			<< name << '\n'
			<< track.keyFrames.size() << '\n';
		
		for (auto& kf : track.keyFrames) {
			// Keyframes info
			fout 
				<< kf.time << ' '
				<< kf.position.x		<< ' ' << kf.position.y			<< ' '	<< kf.position.z << ' '
				<< kf.rotationAngle.x	<< ' ' << kf.rotationAngle.y	<< ' '	<< kf.rotationAngle.z << ' '
				<< kf.scale.x			<< ' ' << kf.scale.y			<< ' '	<< kf.scale.z << '\n';
		}
	}

	fout.close();
	std::cout 
		<< "Export Animation successfully.\n"
		<< "File path: " << path << '\n';
}

void Animator::deleteAnimation(const char* animation) {
	if (animations.count(animation)) {
		animations.erase(animation);
	}
	if (currAnimation == animation) {
		currAnimation = "";
		isPlaying = false;
	}
}

const bool Animator::isOnPlaying() const {
	return this->isPlaying;
}

const std::unordered_map<std::string, AnimationClip>& Animator::getAnimations() const {
	return this->animations;
}

void Animator::play(const char* animationName) {
	if (animations.count(animationName)) {
		std::cout << "Play animation: " << animationName << '\n';
		currAnimation = animationName;
		currTime = 0;
		isPlaying = true;
	}
}

void Animator::clear() {
	this->animations.clear();
	this->currAnimation = "";
	this->currTime = 0;
	this->isPlaying = false;
}

const glm::mat4& Animator::getAnimationMatrix(std::string part) {
	if (!animations.count(currAnimation)) {
		return glm::mat4(1.f);
	}

	AnimationClip& clip = animations[currAnimation];
	if (!clip.tracks.count(part)) {
		return glm::mat4(1.f);
	}

	Track& track = clip.tracks[part];
	if (isPlaying) {
		return track.interpolate(currTime, clip.speed);
	}
	return track.interpolate(clip.duration, clip.speed);
}
