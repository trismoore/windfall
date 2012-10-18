#version 330
vertex:
	in vec3 vertex;

	uniform mat4 VP;

	uniform float startX, startZ, sizeX, sizeZ;

	void main() {
		vec3 pos = vec3(startX+vertex.x*sizeX,
		                       vertex.y,
		                startZ+vertex.z*sizeZ);

		gl_Position = VP * vec4(pos,1); // object->world
	}

fragment:
	uniform vec3 colour;
	out vec4 out_colour;

	void main()
	{
		out_colour = vec4(colour, 0.8);
	}
