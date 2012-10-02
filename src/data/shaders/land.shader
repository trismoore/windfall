vertex:
  in vec2 vertex;
  out vec2 UV;

  uniform mat4 VP;
  uniform sampler2D heightsSampler;

  void main() {
    UV = vertex;
    float h = texture2D( heightsSampler, vertex ).r * 0.4;
    gl_Position = VP * vec4(vertex,h,1);
  }

fragment:
  in vec2 UV;
  out vec4 color;
  uniform sampler2D heightsSampler;

  void main()
  {
    vec4 t = texture2D( heightsSampler, UV );
    color = vec4(clamp(t.r-0.3,0,1)+0.2,t.r,1-t.r,1);
  }
