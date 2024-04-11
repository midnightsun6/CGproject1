#include "Animator.h"

Animator::Animator() {
	currTime = 0;
}

Animator::~Animator() {}

void Animator::update(float dt) {
	if (animations.count(currAnimation)) {
		AnimationClip& clip = animations[currAnimation];

		currTime += dt;
		if (currTime > clip.duration) {
			currTime = fmod(currTime, clip.duration);
		}
	}
}

void Animator::addAnimation(AnimationClip animation) {
	animations[animation.name] = animation;
}

void Animator::transitionTo(std::string animationName) {
	this->currAnimation = animationName;
}

const glm::mat4& Animator::getAnimationMatrix(std::string part) {
	return animations[currAnimation].tracks[part].interpolate(currTime);
}
