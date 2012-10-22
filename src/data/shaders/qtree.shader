#version 330
vertex:
	in vec3 vertex;

	uniform mat4 VP;

	uniform vec3 bbMin, bbMax;

	void main() {
		vec3 size = bbMax - bbMin;
		vec3 pos = bbMin + vertex * size;

		gl_Position = VP * vec4(pos,1); // object->world
	}

fragment:
	uniform vec3 colour;
	out vec4 out_colour;

	void main()
	{
		out_colour = vec4(colour, 0.8);
	}
