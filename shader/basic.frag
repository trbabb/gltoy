varying vec3 pos;
varying vec3 normal;
varying vec4 color;

void main(){
	vec3 normal1 = normalize(normal);
	vec3 Lcolor  = vec3(1, 1, 1);
	vec3 Lx      = normalize(vec3(5, 10, -5));
	vec3 c = max(0.0, dot(normal1, Lx)) * color.xyz * Lcolor;
	gl_FragColor = vec4(c, 1.0);
}