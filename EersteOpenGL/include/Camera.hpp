#pragma once

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx\rotate_vector.hpp>

class Camera {
public:
	Camera(); // constructor!

	// getter en setter functies voor matrices:
	glm::mat4 GetViewMatrix() const;
	void SetProjectionMatrix(float fovy, float aspect, float near, float far);
	glm::mat4 GetProjectionMatrix() const;

	// andere functies
	void MouseLook(int mouseX, int mouseY);
	void moveForward(float speed);
	void moveBackward(float speed);
	void moveLeft(float speed);
	void moveRight(float speed);

private:
	glm::mat4 mProjectionMatrix;
	glm::vec3 mEye;
	glm::vec3 mViewDirection;
	glm::vec3 mUpVector;
	glm::vec2 mOldMousePosition;
};
