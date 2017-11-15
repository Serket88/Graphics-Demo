#include "Material.h"
#include <cassert>

using namespace std;

Material::Material() {
	//  No actual values set here
}

void Material::setMat(Material &m) {
	this->ka = m.ka;
	this->kd = m.kd;
	this->ks = m.ks;
	this->s = m.s;
}
