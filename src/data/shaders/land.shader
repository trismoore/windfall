#version 330
header:
	This is the land shader,
	it gets the height from a texture
	and colours it simply

vertex:
	in vec2 vertex;
	out vec3 pos;

	uniform mat4 VP;
	uniform sampler2D heightsSampler;
	uniform float heightScale;

	void main() {
		// fetch height from texture
		float h = texture2D( heightsSampler, vertex ).r;
		pos = vec3(vertex.x, h, vertex.y);
		h *= heightScale;
		gl_Position = VP * vec4(vertex.x,h,vertex.y,1);
	}

fragment:
	in vec3 pos;
	out vec4 colour;
	uniform sampler2D groundSampler;
	uniform sampler2D groundDetail;

	void main()
	{
		//colour = vec4(pos,1);
		colour = texture2D( groundSampler, pos.xz );

		colour.rgb = colour.rgb * 0.3 * 
		( texture2D( groundDetail, pos.xz*3 ).r
		+ texture2D( groundDetail, pos.xz*7 ).r
		+ texture2D( groundDetail, pos.xz*17 ).r
		+ texture2D( groundDetail, pos.xz*43 ).r);
	}
