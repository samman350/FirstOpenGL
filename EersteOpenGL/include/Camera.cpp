#include "Camera.hpp"
#include <iostream>

#include "glm.hpp"
#include "gtc\matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx\rotate_vector.hpp"

Camera::Camera() { //constructor voor initialisatie
	mEye = glm::vec3(0.0f, 0.0f, 0.0f);
	mViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	mUpVector = glm::vec3(0.0f, 1.0f, 0.0f); // naar boven 
}

glm::mat4 Camera::GetViewMatrix() const {
	return glm::lookAt(mEye, mEye + mViewDirection, mUpVector); // a getter function
}

void Camera::SetProjectionMatrix(float fovy, float aspect, float near, float far) { // a setter function
	mProjectionMatrix = glm::perspective(fovy, aspect, near, far);
}

glm::mat4 Camera::GetProjectionMatrix() const{
	return mProjectionMatrix;
}

void Camera::MouseLook(int mouseX, int mouseY) {
	glm::vec2 currentMouse = glm::vec2(mouseX, mouseY);

	static bool firstLook = true; // static betekent : initialize once
	if (firstLook) {
		mOldMousePosition = currentMouse;
		firstLook = false; // functie om alleen eerste keer oldMousePos een waarde te geven
	}

	glm::vec2 mouseDelta = mOldMousePosition - currentMouse;
	mViewDirection = glm::rotate(mViewDirection, glm::radians(0.1f*mouseDelta.x), mUpVector);
	mViewDirection = glm::rotate(mViewDirection, glm::radians(0.1f * mouseDelta.y), glm::cross(mViewDirection,mUpVector)); //up and down door kruisproduct
	
	mOldMousePosition = currentMouse;
}

void Camera::moveForward(float speed) {
	mEye += (mViewDirection*speed);  // loop in de richting van view
	//std::cout << mViewDirection << std::endl;
}

void Camera::moveBackward(float speed) {
	mEye -= (mViewDirection*speed);
}
void Camera::moveLeft(float speed) {
	//glm::vec3 DeltaPerp1 = (glm::cross( mViewDirection, mUpVector) * speed);
	mEye -= (glm::cross(mViewDirection, mUpVector) * speed);
	//float message1 = DeltaPerp1.x;
	//float message2 = DeltaPerp1.y;
	//float message3 = DeltaPerp1.z;
	//std::cout << "viewDirection: " << mViewDirection.x << ", " << mViewDirection.y << ", " << mViewDirection.z << std::endl;
	//std::cout << "direction: " << message1 << ", " << message2 << ", " << message3 << std::endl;
}
void Camera::moveRight(float speed) {
	//glm::vec3 DeltaPerp1 = (glm::cross( mViewDirection, mUpVector) * speed);
	mEye += (glm::cross(mViewDirection, mUpVector) * speed);
	//float message1 = DeltaPerp1.x;
	//float message2 = DeltaPerp1.y;
	//float message3 = DeltaPerp1.z;
	//std::cout << "viewDirection: " << mViewDirection.x << ", " << mViewDirection.y << ", " << mViewDirection.z << std::endl;
	//std::cout << "direction: " << message1 << ", " << message2 << ", " << message3 << std::endl;
}