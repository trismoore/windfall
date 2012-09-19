vertex:
  in vec2 vertexPos;
  in vec2 vertexUV;
  out vec2 UV;

  void main() {
    gl_Position = vec4(vertexPos,0,1);
    UV = vertexUV;
  }

fragment:
  in vec2 UV;
  out vec4 color;
  uniform sampler2D myTextureSampler;

  void main()
  {
    color = texture2D( myTextureSampler, UV );
  }
