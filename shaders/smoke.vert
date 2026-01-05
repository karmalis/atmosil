#version 120

void main() {
    // Transform position to screen coordinates
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    // Pass the particle color (including alpha fade) to the fragment shader
    gl_FrontColor = gl_Color;

    // Optional: Scale point size based on a uniform or attribute if needed
    gl_PointSize = 10.0;
}