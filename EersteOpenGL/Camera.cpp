#include "Camera.hpp"
#include <iostream>

Camera::Camera() {
	mEye = glm::vec3(0.0f, 0.0f, 0.0f);
	mViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	mUpVector = glm::vec3(0.0f, 1.0f, 0.0f); // naar boven 
}

glm::mat4 Camera::GetViewMatrix() const {
	return glm::lookAt(mEye, mViewDirection, mUpVector); // a gettr function
}

void Camera::MouseLook(int mouseX, int mouseY) {
	std::cout <<"mouse: " << mouseX << ", " << mouseY << std::endl;
}

void Camera::moveForward(float speed) {
	mEye.z -= speed;
}
void Camera::moveBackward(float speed) {
	mEye.z += speed;
}
void Camera::moveLeft(float speed) {

}
void Camera::moveRight(float speed) {

}