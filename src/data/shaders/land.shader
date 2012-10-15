#version 330
header:
	This is the land shader,
	it gets the height from a texture
	and colours it simply

vertex:
	in vec2 vertex;
	out vec3 pos;
	out vec3 normal;

	uniform mat4 VP;
	uniform sampler2D heightsSampler;
	uniform float heightScale;

	float rand(vec2 co) {
 		return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
	}

	vec3 getPos(vec2 location) {
		vec4 tex = texture2D( heightsSampler, location );
		return vec3(location.x + ((1-0.5*rand(location.xy))*tex.b/128),
		            heightScale * 
		             (tex.r                                         // r = ROCK
			     +(1 - 0.5 * rand(location.xy)) * tex.b / 32 ), // b = BUMPINESS
		            location.y + ((1-0.5*rand(location.xy))*tex.b/128) );
	}

	void main() {
		pos = getPos(vertex);

		gl_Position = VP * vec4(pos,1); // object->world
	}

fragment:
	in vec3 pos;
	in vec3 normal;
	out vec4 colour;
	uniform sampler2D groundSampler;
	uniform sampler2D groundDetail;

	void main()
	{
		// read colour from ground texture
		colour = texture2D( groundSampler, pos.xz );

		// multiply by some detail textures at various stretches
		colour.rgb = colour.rgb * 0.22 * 
		( texture2D( groundDetail, pos.xz*543).r
		+ texture2D( groundDetail, pos.xz*234).r
		+ texture2D( groundDetail, pos.xz*123).r
		+ texture2D( groundDetail, pos.xz* 37).r
		+ texture2D( groundDetail, pos.xz* 17).r);
	}
