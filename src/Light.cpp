#include "Light.h"
#include <cassert>

using namespace std;

Light::Light() {
	//  nothing set here
}

void Light::setLight(Light &l) {
	this->lightPos = l.lightPos;
	this->intensity = l.intensity;
}
