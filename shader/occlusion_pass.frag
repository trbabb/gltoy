#extension GL_EXT_gpu_shader4 : enable

varying vec3 pos;
varying vec3 normal;
varying vec4 color;

//number of unique sample locations 
//power of two please, kthx
#define NUM_SAMPLES  64
#define NUM_SAMPLESF 64.0
#define SAMPLES_MASK 63

//TODO: expose thickness metric. in real units would be nice
//TODO: speaking of real units, depth is not exact because it doesn't account for near and far clip
//TODO: performance seems heavily dependent on spread of samples in texture space. what can I do about this?
//TODO: how the F does inigo do a blur? sampling textures seems to be awfully slow. I don't have any time left over for another pass.
//TODO: why is the occlusion so smooth when our perlin noise plane is camera-facing?
//TODO:  - sample a constant pixel and plot a histogram of the occlusion samples.

uniform sampler2D color_input;
uniform sampler2D depth_input;

uniform vec3 samples[NUM_SAMPLES]; //library of points in a sphere
uniform int  permute[NUM_SAMPLES]; //permutation map, for cheap randomness

uniform int nsamples;      //number of per-pixel occlusion tests
uniform int rnd_seed;      //randomness seed
uniform float z_near;      //camera near
uniform float z_far;       //camera far
uniform vec4 tint;         //occlusion tint color 
uniform float world_scale; //occlusion radius, as fraction of screen space at depth = 1.0
						   //could be expressed more elegantly, but I gots bigger problems

/*
float samplePixel(vec2 xy, int smpidx, float pxd, vec3 flipv, float scale){
    vec3 rand_dx = reflect(samples[smpidx], flipv);
    vec2 smp_pt  = xy + rand_dx.xy * width;
    float d_sampled = 1.0 / (1.0 - texture2D(depth_input, smp_pt).r);
    float exposed_column = d_sampled - (pxd - h * 0.5);
    return exposed_column;
}
*/

void main(){
	vec2 tcoord  = gl_TexCoord[0].st;
	vec4 color   = texture2D(color_input, tcoord);
	float d_read = texture2D(depth_input, tcoord).r;
	float pixel_depth = clamp(1.0 / (1.0 - d_read), 0.0, z_far);
	float occ_sum = 0.0;
	
	int seed_x = int(gl_FragCoord.x);
	int seed_y = int(gl_FragCoord.y);
	
	int i_start = permute[(permute[seed_x & SAMPLES_MASK] + seed_y + rnd_seed) & SAMPLES_MASK]; //0;
	vec3 flip   = samples[permute[(permute[seed_y & SAMPLES_MASK] + seed_x - rnd_seed) & SAMPLES_MASK]];
	float width = clamp(world_scale/pixel_depth, 0.0, 0.25);
	float h = world_scale * 2.0; //todo: this should sample over a sphere, not a cone. and put this back inside the loop
	
	float vol = float(nsamples) * h; //0.0;
	
	for (int i = 0; i < nsamples; i++){
		int idx = (i_start + i) & SAMPLES_MASK;
		
		//by reflecting about a "random" normal, we effectively square the number unique sample locations
		vec3 rand_dx = reflect(samples[idx], flip); //samples[idx]; //
		vec2 smp_pt  = tcoord + rand_dx.xy * width;
		float d_sampled = 1.0 / (1.0 - texture2D(depth_input, smp_pt).r);
		//vol += h;
		float exposed_column = d_sampled - (pixel_depth - h*0.5);
		occ_sum += clamp(max(exposed_column, -exposed_column - h*10.0), 0.0, h);
	}
	
	occ_sum = occ_sum/vol;
	vec4 occ_color = mix(tint, vec4(1), occ_sum);
	
	gl_FragColor = color * occ_color;
}