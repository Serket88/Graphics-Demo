#version 120

uniform vec3 lightPos;
uniform float lightInt;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

varying vec3 position;
varying vec3 normal;

void main()
{
	//  ============  Ambient Component  ============

	vec3 ca = ka;

	//  ============  Diffuse Component  ============

	vec3 n = normalize(normal);
	vec3 l = normalize(lightPos - position);
	vec3 cd = kd * max(0, dot(l, n));

	//  ============  Specular Component  ============

	vec3 view = normalize(-position);
	vec3 h = normalize(l + view);
	vec3 cs = ks * pow(max(0, dot(h, n)), s);

	//  ============  Color  ============

	vec3 color = lightInt * (ca + cd + cs);
	gl_FragColor = vec4(color.r, color.g, color.b, 1.0);
}
