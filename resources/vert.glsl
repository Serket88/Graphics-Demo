#version 120

uniform mat4 P;
uniform mat4 MV;
uniform vec3 lightPos;

attribute vec4 aPos; // in object space
attribute vec3 aNor; // in object space

varying vec3 position;
varying vec3 normal;

void main()
{
	vec4 posTemp = MV * aPos;
	vec4 norTemp = MV * vec4(aNor, 0.0);
	position = posTemp.xyz;
	normal = norTemp.xyz;

	gl_Position = P * MV * aPos;
}
