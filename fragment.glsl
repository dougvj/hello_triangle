#version 330
in vec2 position;
in vec4 color;
out vec4 frag_color;

void main()
{
  //We have more fragments than verticies so our in values are interpolated,  
  //including the position 
  //Force alpha to 1 here, but we could just output directly
  frag_color = vec4(color.x, color.y, color.z, 1.0);
  //This would just output white and ignore our input colors
  //frag_color = vec4(1.0, 1.0, 1.0, 1.0);
}
