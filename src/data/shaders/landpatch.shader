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
	uniform sampler2D groundDetail;

	uniform vec2 textureScale; // 1 / (how many patches X and Z), to stretch the texture over all patches
	uniform vec2 position;     // XZ for this patch
	uniform vec2 size;         // how large XZ this patch is

	float rough(vec2 location)
	{
		return 0.25 * (-2 +
		  ( texture2D(groundDetail,location*17).b
		  + texture2D(groundDetail,location*123).b
		  + texture2D(groundDetail,location*432).b
		  + texture2D(groundDetail,location*902).b )
		  );
	}

	float getHeight(vec2 location) {
		vec4 tex = texture2D( heightsSampler, location);
		return heightScale * (tex.r + rough(location) * tex.b * 0.05);
	}

	void main() {
		uv = (vertex * size + position) * textureScale;  // ([0..1] + [0..tiles-1]) * 1/tiles
		vec3 pos = vec3( vertex.x * size.x + position.x,
		                       getHeight(uv),
		                 vertex.y * size.y + position.y );

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
