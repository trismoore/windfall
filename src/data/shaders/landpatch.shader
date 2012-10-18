#version 330
header:
	This is the land patch shader,
	it gets it's position and texture offset and
	it gets the height from a texture
	and colours it simply

vertex:
	in vec2 vertex;
	out vec2 uv;

	uniform mat4 VP;
	uniform sampler2D heightsSampler;
	uniform float heightScale;

	uniform vec2 textureScale; // 1 / (how many patches X and Z), to stretch the texture over all patches
	uniform vec2 position;     // XZ for this

	float rand(vec2 co) {
 		return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
	}

	vec3 getPos(vec2 location) {
		vec4 tex = texture2D( heightsSampler, location);
		return vec3(location.x /* + ((1-0.5*rand(location.xy))*tex.b/128) */ ,
		            heightScale * 
		             (tex.r                                         // r = ROCK
			     +(1 - 0.5 * rand(location.xy)) * tex.b / 32 ), // b = BUMPINESS
		            location.y /* + ((1-0.5*rand(location.xy))*tex.b/128) */ );
	}

	float getHeight(vec2 location) {
		vec4 tex = texture2D( heightsSampler, location);
		return heightScale * (tex.r + (1-0.5*rand(location)) * tex.b/32);
	}

	void main() {
		uv = (vertex + position) * textureScale;  // ([0..1] + [0..tiles-1]) * 1/tiles
		vec3 pos;
		pos.x = vertex.x + position.x;
		pos.y = getHeight(uv);
		pos.z = vertex.y + position.y;

		gl_Position = VP * vec4(pos,1); // object->world
	}

fragment:
	in vec2 uv;
	out vec4 colour;

	uniform sampler2D groundSampler;
	uniform sampler2D groundDetail;

	void main()
	{
		// read colour from ground texture
		colour = texture2D( groundSampler, uv );

		// multiply by some detail textures at various stretches
		colour.rgb = colour.rgb * 0.22 * 
		( texture2D( groundDetail, uv*543).r
		+ texture2D( groundDetail, uv*234).r
		+ texture2D( groundDetail, uv*123).r
		+ texture2D( groundDetail, uv* 37).r
		+ texture2D( groundDetail, uv* 17).r);
	}
