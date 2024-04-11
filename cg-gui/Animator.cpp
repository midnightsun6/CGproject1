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

void Animator::transitionTo(std::string animationName) {
	if (animations.count(animationName)) {
		currAnimation = animationName;
		currTime = 0;
		isPlaying = true;
	}
}

const glm::mat4& Animator::getAnimationMatrix(std::string part) {
	AnimationClip& clip = animations[currAnimation];
	Track& track = clip.tracks[part];
	if (isPlaying) {
		return track.interpolate(currTime);
	}
	return track.interpolate(clip.duration);
}
