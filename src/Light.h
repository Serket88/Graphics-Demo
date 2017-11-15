#pragma once
#ifndef __Light__
#define __Light__

#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

class Light {
	public:
		Light();
		glm::vec3 lightPos;
		float intensity;

		void setLight(Light &l);
};

#endif
