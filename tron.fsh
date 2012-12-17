uniform sampler2D qt_Texture0;
varying highp vec4 qt_TexCoord0;

const float step_w = 0.0015625;
const float step_h = 0.0027778;

void main(void)
{
    vec2 uv = qt_TexCoord0.xy;
    vec3 t1 = texture2D(qt_Texture0, vec2(uv.x - step_w, uv.y - step_h)).bgr;
    vec3 t2 = texture2D(qt_Texture0, vec2(uv.x, uv.y - step_h)).bgr;
    vec3 t3 = texture2D(qt_Texture0, vec2(uv.x + step_w, uv.y - step_h)).bgr;
    vec3 t4 = texture2D(qt_Texture0, vec2(uv.x - step_w, uv.y)).bgr;
    vec3 t5 = texture2D(qt_Texture0, uv).bgr;
    vec3 t6 = texture2D(qt_Texture0, vec2(uv.x + step_w, uv.y)).bgr;
    vec3 t7 = texture2D(qt_Texture0, vec2(uv.x - step_w, uv.y + step_h)).bgr;
    vec3 t8 = texture2D(qt_Texture0, vec2(uv.x, uv.y + step_h)).bgr;
    vec3 t9 = texture2D(qt_Texture0, vec2(uv.x + step_w, uv.y + step_h)).bgr;

    vec3 xx= t1 + 2.0*t2 + t3 - t7 - 2.0*t8 - t9;
    vec3 yy = t1 - t3 + 2.0*t4 - 2.0*t6 + t7 - t9;
    vec3 rr = sqrt(xx * xx + yy * yy);
    vec3 col = rr * 2.0 * t5;

    vec4 color = texture2D(qt_Texture0,uv);
    gl_FragColor = color.a * vec4(col, 1.0);
}
