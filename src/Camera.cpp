#include "Camera.h"
#include "MatrixStack.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#define PI 3.14159265358979323846

Camera::Camera() :
	aspect(1.0f),
	fovy(45.0f),
	znear(0.1f),
	zfar(1000.0f),
	rotations(0.0, 0.0),
	translations(0.0f, 0.0f, -5.0f),
	rfactor(0.01f),
	tfactor(0.001f),
	sfactor(0.005f)
{
}

Camera::~Camera()
{
}

void Camera::mouseClicked(float x, float y, bool shift, bool ctrl, bool alt)
{
	mousePrev.x = x;
	mousePrev.y = y;
}

void Camera::mouseMoved(float x, float y)
{
	glm::vec2 mouseCurr(x, y);
	float xchange = x - mousePrev.x;
	float ychange = mousePrev.y - y;
	glm::vec2 dv(xchange, ychange);

	rotations += rfactor * dv;

	mousePrev = mouseCurr;
}

void Camera::applyProjectionMatrix(std::shared_ptr<MatrixStack> P) const
{
	// Modify provided MatrixStack
	P->multMatrix(glm::perspective(fovy, aspect, znear, zfar));
}

void Camera::applyViewMatrix(std::shared_ptr<MatrixStack> MV) 
{
	float yaw = rotations.x;
	float pitch = rotations.y;

	if (pitch >= PI/2) {
		pitch = (PI/2)-0.01;
	} else if (pitch < -(PI/2)) {
		pitch = -(PI/2) + 0.01;
	}

	glm::vec3 target;

	target.x = cos(yaw) * cos(pitch);
	target.y = sin(pitch);
	target.z = sin(yaw) * cos(pitch);

	front = glm::normalize(target);

	pos.y = 0.0f;
	MV->multMatrix(glm::lookAt(pos, pos+front, up));
	
}

