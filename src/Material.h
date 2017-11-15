#pragma once
#ifndef __Material__
#define __Material__

#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

class Material {
	public:
		Material();
		glm::vec3 ka;
		glm::vec3 kd;
		glm::vec3 ks;
		float s;

		void setMat(Material &m);
};

#endif
