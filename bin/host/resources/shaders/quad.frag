varying vec2 f_texcoord;
uniform sampler2D texture;
uniform float opacity;

void main(void) {
  vec4 basecolor = texture2D(texture, f_texcoord);
  if(basecolor.a == 0.0) discard; // alpha testing
  gl_FragColor = basecolor;
  gl_FragColor.a = gl_FragColor.a * opacity;
}