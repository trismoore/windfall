vertex:
//  in vec2 vertexPos;
  in vec2 vertexUV;
  out vec2 UV;
  uniform float offX;
  uniform float scale;

  void main() {
    gl_Position = vec4(-1+ offX + vertexUV.x * scale, -1+(1-vertexUV.y) * scale, 0, 1);
    UV = vertexUV;
  }

fragment:
  in vec2 UV;
  out vec4 colour;
  uniform sampler2D myTextureSampler;

  void main()
  {
    colour = texture2D( myTextureSampler, UV );
  }
