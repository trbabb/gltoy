varying float ptSize;
varying vec3 ptLoc;
varying vec4 position;

void main(void){
	vec3  e = position.xyz;
	vec3  o = -ptLoc;
	float r = ptSize / 5.0;
	float a = dot(e, e);
	float b = dot(o, e) * 2.0;
	float c = -(r*r - dot(o,o));
	float descr = b*b - 4.0*a*c;
	if (descr < 0.0){
		gl_FragColor = vec4(0);
		gl_FragDepth = 1.0;
	} else {
		descr = sqrt(descr);
		float s1 = (-b + descr) / (2.0 * a);
		float s2 = (-b - descr) / (2.0 * a);
		vec3 surfPt = min(s1, s2) * e;
		vec3 n = surfPt - ptLoc;
		vec3 color = normalize(n) * 0.5 + vec3(0.5);
		color = mix(vec3(1), color, exp(-length(surfPt)*1.0));
		vec4 projectedPos = (gl_ProjectionMatrix * vec4(surfPt, 1));
		gl_FragColor = vec4(color, 1);
		gl_FragDepth = projectedPos.z / 4.0; // projectedPos.w;
	}
}