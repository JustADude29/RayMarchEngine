#version 440 core

in vec3 vs_positiom;
in vec3 vs_color;
in vec2 vs_texcoord;

uniform vec2 u_resolution;

out vec4 fs_color;

const float FOV = 1.0;
const int MAX_STEPS = 256;
const float MAX_DIST = 500;
const float HIT_DIST = 0.001;

float fractal(vec3 p) {
    vec3 z = p;
    float d = length(p);
    float n = 0.0;
    float r;
    for (int i = 0; i < 64; i++) {
        r = length(z);
        if (r > 2.0) break;
        z = vec3(pow(r, 8.0) * cos(8.0 * atan(z.y, z.x)) * cos(8.0 * acos(z.z / r)),
                 pow(r, 8.0) * sin(8.0 * atan(z.y, z.x)) * cos(8.0 * acos(z.z / r)),
                 pow(r, 8.0) * sin(8.0 * acos(z.z / r)));
        n++;
    }
    return 0.5 * log(length(z)) / log(2.0) + 4.0 - n;
}

float sdOctahedron( vec3 p, float s)
{
  p = abs(p);
  float m = p.x+p.y+p.z-s;
  vec3 q;
       if( 3.0*p.x < m ) q = p.xyz;
  else if( 3.0*p.y < m ) q = p.yzx;
  else if( 3.0*p.z < m ) q = p.zxy;
  else return m*0.57735027;
    
  float k = clamp(0.5*(q.z-q.y+s),0.0,s); 
  return length(vec3(q.x,q.y-s+k,q.z-k)); 
}


float map(vec3 p){
	//p = mod(p, 4.f) - 4.0*0.5;
	//sphere
	float sphereDist = length(p) - 1.0;
	float sphereID = 1.0;
	float sphere = sphereDist;
	//mandelbrot
	float frac = fractal(p);
	//octahedron
	float octa = sdOctahedron(p, 1);
	//result
	float res =  fOpUnionChamfer(sphere, octa, 0.5);
	return res;
}

float rayMarch(vec3 ro, vec3 rd){
	float hit, object;
	for(int i=0;i<MAX_STEPS;i++){
		vec3 p = ro + object.x * rd;
		hit = map(p);
		object += hit;
		if (abs(hit) < HIT_DIST || object > MAX_DIST) break;
	}
	return object;
}


void render(inout vec3 col, in vec2 uv){
	vec3 ro = vec3(0.f,0.f,-3.f);
	vec3 rd = normalize(vec3(uv, FOV));

	float object = rayMarch(ro, rd);

	if(object.x < MAX_DIST){
		col += vec3(1.f, 2.f, 2.f) / object;
	}
}

void main(){
	vec2 uv = (2.0 * gl_FragCoord.xy - u_resolution.xy) / u_resolution.y;

	vec3 col;
	render(col, uv);

	fs_color = vec4(col, 1.f);
}