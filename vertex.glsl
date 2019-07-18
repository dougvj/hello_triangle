#version 330

in vec2 in_position;
in vec4 in_color;
out vec2 position;
out vec4 color;


void main() {
    //We don't do any transforms but here we could apply a transformation to the 
    //points such as to move or rotate an object in the scene and do the camera
    //viewpoint
    //This just passes the position to the fragment shader
    position = in_position;
    //This is actually used by opengl to determine the fragements to render
    //Note that it is a vec4 so the first 2 points are the output points, the
    //thirds is the point size and the fouth clipping control.
    gl_Position = vec4(in_position.x, in_position.y, 0.0, 1.0);
    //We could also change the colors here, but we don't bother
    color = in_color;
}
