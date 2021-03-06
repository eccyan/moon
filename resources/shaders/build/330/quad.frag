#version 330

in vec2 f_texcoord;
uniform sampler2D tex0;
uniform float opacity;
uniform vec4 color;
in vec4 f_color;
uniform vec4 tone;
out vec4 fragColor;
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
void main(void)
{
    const vec3 white = vec3(1.0, 1.0, 1.0);
    vec4 basecolor = texture(tex0, f_texcoord);
    if (basecolor.a == 0.0) discard;
    vec3 blendcolor = mix(basecolor.rgb, white, tone.rgb);
    if (tone.a != 1.0) {
        fragColor = f_color * vec4(hsv2rgb(rgb2hsv(blendcolor) * vec3(1.0, tone.a, 1.0)),
                                   basecolor.a * opacity) * color;
    } else {
        fragColor = f_color * vec4(blendcolor, basecolor.a * opacity) * color;
    }
}
