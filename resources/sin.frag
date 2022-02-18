#version 300 es

precision mediump float;
out vec4 fragColor;
uniform float iTime;
uniform vec3 iResolution;

float q(vec2 pos,float angle){
    return pos.x * cos(angle) + pos.y * sin(angle);
}

float pi=atan(1.0,0.0)*2.0;

float Hexagon(vec2 pos, float time){
    float c0 = 0.0;
    float c2 = 2.0;
    float c3 = 3.0;
    
    float c = cos(q(pos, pi / c3));
    c += cos(q(pos, c0));
    c += cos(q(pos, time + pi / c3));
    c += cos(q(pos, time + c0));
    c += cos(q(pos, pi / c3 * c2));
    c += cos(q(pos, time + pi / c3 * c2));
    
    
    float distanceToCenter = sqrt((pos.x * pos.x) + (pos.y * pos.y));
    return c;
}

vec3 HexagonColor(float hex, vec2 uv, float time, float duv){
    float f = dot(uv, uv);
    float distanceToCenter = sqrt((uv.x * uv.x) + (uv.y * uv.y));
    vec3 col = 0.5 + 0.5 * cos(time + hex + uv.xyx + vec3(0, 2, 4));
    
    return col;
}

void main()
{
    vec2 UV = gl_FragCoord.xy/iResolution.xy-.5;
    float duv= dot(UV, UV);
    
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 pos = (uv - vec2(0.5)) * iResolution.xy / iResolution.y * 200.0;
    
    float time = iTime / 15.0;
    
    float angle = atan(pos.y, pos.x) + time;
    pos = length(pos) * vec2(cos(angle), sin(angle));
    
    //time = 0.0;
    float hex = Hexagon(pos, time);
    
    // Time varying pixel color
    vec3 col = HexagonColor(hex, uv, time, duv);

    // Output to screen
    fragColor = vec4(col * (-hex), 1.0);
}