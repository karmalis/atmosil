#version 120

uniform sampler2D u_flowMap;

void main() {
    // 1. Read the texture coordinate
    vec2 coord = gl_TexCoord[0].xy;

    // 2. Sample the texture
    // The CPU packed Wind X into Red, Wind Y into Green.
    // Range [0.0, 1.0] where 0.5 is Zero Velocity.
    vec4 data = texture2D(u_flowMap, coord);

    // 3. Unpack Data
    // Convert 0.5 -> 0.0, 1.0 -> 1.0, 0.0 -> -1.0
    float velX = (data.r - 0.5) * 2.0;
    float velY = (data.g - 0.5) * 2.0;

    // 4. Calculate Flow Magnitude (Speed)
    float speed = sqrt(velX * velX + velY * velY);

    // 5. Visualization Logic
    // If speed is very low, draw dark grey (Atmosphere background)
    vec3 color = vec3(0.1, 0.1, 0.1);

    if (speed > 0.01) {
        // Normalize direction for color wheel
        vec2 dir = normalize(vec2(velX, velY));

        // Map Direction to Color
        // X+ (Right) = Red
        // Y+ (Down)  = Green
        // Diagonal   = Yellow
        color.r = (dir.x * 0.5) + 0.5; // Map [-1,1] to [0,1]
        color.g = (dir.y * 0.5) + 0.5;
        color.b = 0.2; // Constant blue tint

        // Apply Speed to Brightness
        // The faster the wind, the brighter the color
        color *= (0.5 + (speed * 5.0));
    }

    gl_FragColor = vec4(color, 1.0);
}