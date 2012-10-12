#version 330

vertex:

	in vec4 in_Position;
	in vec4 in_Color;

	out vec4 ex_Color;

	void main(void)
	{
		gl_Position = in_Position;
		ex_Color = in_Color;
	}

fragment:
	in vec4 ex_Color;
	out vec4 out_Color;

	void main(void)
	{
		out_Color = ex_Color;
	}
