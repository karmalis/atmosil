#version 120

void main() {
    // gl_PointCoord is the texture coordinate (0.0 to 1.0) of the point sprite
    vec2 coord = gl_PointCoord - vec2(0.5);

    // Calculate distance from center
    float dist = length(coord);

    // 1. Shape: Discard pixels outside the circle radius (0.5)
    if (dist > 0.5)
    discard;

    // 2. Softness: Create a gradient for the "puff" effect
    // smoothstep returns 0.0 at center, 1.0 at edge. We invert it.
    float softness = 1.0 - smoothstep(0.0, 0.5, dist);

    // Apply the softness to the alpha channel
    vec4 color = gl_Color;
    color.a *= softness;

    gl_FragColor = color;
}