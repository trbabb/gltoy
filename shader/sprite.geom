#version 120 
#extension GL_EXT_geometry_shader4 : enable

uniform mat4 camOrient;

varying in  float ptSize_v[];
varying out float ptSize;
varying out vec3  ptLoc;
varying out vec4  position;

void main(void){
	for(int i = 0; i < gl_VerticesIn; i++){
		vec4 ctr = gl_PositionIn[i];
		vec3 ptLocXformed = (gl_ModelViewMatrix * ctr).xyz;
		//for each point, emit a quad.
		for (int j = 0; j < 4; j++){
			int p = j/2;
			vec2 add = vec2(p, j - p*2) - vec2(0.5);
			vec4 corner = vec4(add*ptSize_v[i], 0, 1);
			corner = camOrient * corner + ctr;
			corner.w = 1;
			
			position = gl_ModelViewMatrix * corner;
			gl_Position = gl_ProjectionMatrix * position;
			ptLoc = ptLocXformed;
			ptSize = ptSize_v[i];
			
			EmitVertex();
		}
		EndPrimitive();
	}
}