varying vec3 pos;
varying vec3 normal;

uniform vec3 origin;
uniform vec3 look;
uniform float time;

float glob(vec3 p, vec3 ctr, float falloff){
	return exp(-falloff * length(p-ctr));
}

float fmod(float f, float g){
	return f/g - floor(f)/floor(g);
}

float distfunc2(vec3 p){
	float globField = 2.0*glob(p, vec3(0.5,-1,6), 3.0) + glob(p, vec3(0,2.9,10), 2.0);
	p.x += 0.05*sin(p.x*16.0) + 0.01*(0.5*sin(p.z*23.0)+1.0)*(sin(p.z*80.0));
	p.y += sin(p.z/6.0 + time*10.0); 
	p.y += sin(p.x/4.0)*1.3;
	p.x += sin(p.y/2.0)*0.7;
	float v = sin(3.0*p.x) + sin(3.0*p.y) + 0.5*sin(3.0*p.z) + sin(time*29.0)*2.0 - 0.0*globField; 
	return v;
}

float distfunc(vec3 p){
	p.y += 2.0*sin(p.z/7.0 + time*10.0);
	p.x += sin(0.2*p.z);
	p = p - 5.0*floor(p/5.0);
	//float extradist = sin(p.y*20.0)*0.02*max(sin(p.x*p.y + time),0.0);
	return min(distance(p, vec3(2,2.5,2.5)) - 1.5, distance(p, vec3(2.5,2.5,2.5)) - 1.0);
}

float distfunc1(vec3 p){
	p = p - sin(p.z)*0.1 + sin(p.z*0.71)*vec3(-1,1,1)*0.08;
	float v = dot(p.xy,p.xy) - 1.0 + sin(p.z*3.0)*0.1 + sin(p.z * 20.0)*0.01;
	return v;
}

float estimate_dV(vec3 e, vec3 pt, float val){
	float epsilon = 0.00001;
	return (distfunc(pt + epsilon*e) - val) / epsilon;
}

vec3 estimate_normal(vec3 p, float val){
	float epsilon = 0.00001;
	float dx = distfunc(p+vec3(epsilon,0,0));
	float dy = distfunc(p+vec3(0,epsilon,0));
	float dz = distfunc(p+vec3(0,0,epsilon));
	return normalize(vec3(dx-val,dy-val,dz-val));
}

vec3 estimate_occlusion(vec3 p, vec3 n){
	p += 0.001 * n;
	return vec3(0);
}

vec3 light(vec3 n, vec3 l, vec3 color){
	l = -normalize(l);
	return max(0.0, dot(n,l)) * color;
}

void main(){
	vec3 fog_color = vec3(1);
	float fog_multiplier = 0.05;
	vec3 e = normalize(vec3(pos.xy,1));
	
	//find the isosurface
	vec3 cur = origin;
	float val = 0.0;
	float last_val = distfunc(cur);
	float last_step = 0.5;
	for (int i = 0; i < 50; i++){
		val = distfunc(cur);
		if (abs(val) < 0.001) break;
		if (sign(last_val) != sign(val)){
		    //we have crossed zero; binary search
		    for (int j = 0; j < 4; j++){
		        if (abs(val) < 0.001) break;
		    	float nextStep = (last_step*val)/(val-last_val);
		    	vec3 new_cur   = cur - e*nextStep;
		    	float new_val  = distfunc(new_cur);
		    	
		    	//continue searching between the points that differ in sign
		    	if (sign(new_val) == sign(val)){
		    		cur = new_cur;
		    		val = new_val;
		    		last_step = last_step - nextStep;
		    	} else {
		    		last_val = new_val;
		    		last_step = nextStep;
		    	}
		    }
		    break;
		} else {
			//float dv = estimate_dV(e,cur,val);
			//vec3 grad = estimate_normal(cur,val);
			//float nextStep = min(last_step*2.0, max(abs(val)*0.2, dot(grad,e)*last_step));
			//float nextStep = min(last_step*2.0, abs(val)*0.2);
			float nextStep = abs(val);
			cur += e * nextStep;
			last_val = val;
			last_step = nextStep;
		}
	}
	
	vec3 final_color;
	float dist_traveled = length(cur - origin);
	vec3 n = estimate_normal(cur, val);
	vec3 l1 = -normalize(vec3(1, 1, 1));
	vec3 l2 = -normalize(vec3(-0.5,1,0.5));
	vec3 lcolor = light(n, vec3(1,1,1), vec3(1));
	lcolor     += light(n, vec3(-0.5,-1,0.5), 0.5*vec3(0.5,0.33,0.2));
	lcolor     *= vec3(0.6,0.5,0.4);
	float fogmix = 1.0 - exp(-dist_traveled*fog_multiplier);
	final_color = mix(lcolor, fog_color, fogmix);
	
	gl_FragColor = vec4(final_color, 1);
}