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
	out vec4 color;

	void main()
	{
		color = vec4(pos,1);
	}
