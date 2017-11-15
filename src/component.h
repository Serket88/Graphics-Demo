//component.h
#ifndef COMPONENT_H
#define COMPONENT_H
#include <vector>
#include <glm/glm.hpp>
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Light.h"
#include "Material.h"
#include <iostream>

using namespace std;
using namespace glm;

class component {
	public:
		//  Transformation vectors:
		
		//  vector rotation
		vec3 rotate;
		//  vector translation
		vec3 translate;
		//  vector scale
		vec3 scale;

		Material material;

		string type;

		//  Methods:

		//  Initializer
		component(const vec3 &meshRotate,
			const vec3 &meshTrans,
			const vec3 &meshScale,
			const Material &meshMaterial);

		//  Draw function
		void draw(shared_ptr<MatrixStack> MV,
			shared_ptr<MatrixStack> P,
			shared_ptr<Program> prog,
			shared_ptr<Shape> shape,
			shared_ptr<Light> light);

};

#endif
