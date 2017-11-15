//component.cpp
#include "component.h"
#include <stdlib.h>

using namespace std;
using namespace glm;

component::component(const vec3 &meshTrans,
		const vec3 &meshRotate,
		const vec3 &meshScale,
		const Material &meshMaterial) {

	this->translate = meshTrans;
	this->rotate = meshRotate;
	this->scale = meshScale;
	this->material = meshMaterial;
}

void component::draw(shared_ptr<MatrixStack> MV, shared_ptr<MatrixStack> P, shared_ptr<Program> prog, shared_ptr<Shape> shape, shared_ptr<Light> light) {
	//  Correct order is push T, push R, push S
	//  Alternatively:  T * (R * (S * M))
	
	MV->pushMatrix();

	//  Apply the translation
	MV->translate(this->translate);

	//  Apply an XYZ rotation in that order
	MV->rotate(this->rotate.x, vec3(1, 0, 0));
	MV->rotate(this->rotate.y, vec3(0, 1, 0));
	MV->rotate(this->rotate.z, vec3(0, 0, 1));

	//  Apply a scale
	MV->scale(this->scale);

	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);

	glUniform3f(prog->getUniform("lightPos"), light->lightPos.x, light->lightPos.y, light->lightPos.z);
	glUniform1f(prog->getUniform("lightInt"), light->intensity);

	glUniform3f(prog->getUniform("ka"), material.ka.x, material.ka.y, material.ka.z);
	glUniform3f(prog->getUniform("kd"), material.kd.x, material.kd.y, material.kd.z);
	glUniform3f(prog->getUniform("ks"), material.ks.x, material.ks.y, material.ks.z);
	glUniform1f(prog->getUniform("s"), material.s);

	shape->draw(prog);

	MV->popMatrix();
}
