#version 120

// The texture contains the scalar pressure in Red,
// but we want to visualize the Vector Field (Velocity) passed as uniforms or a texture.
// For now, let's assume we pass a texture where RG = Velocity X/Y
uniform sampler2D u_flowMap;

void main() {
    vec2 coord = gl_TexCoord[0].xy;
    vec4 data = texture2D(u_flowMap, coord);

    // Remap velocity from [-1, 1] range to [0, 1] for color display
    // 0.5 is "Zero Velocity" (Gray)
    float velX = (data.r * 0.5) + 0.5;
    float velY = (data.g * 0.5) + 0.5;

    // Blue channel represents pressure magnitude
    float pressure = data.b;

    gl_FragColor = vec4(velX, velY, pressure, 1.0);
}