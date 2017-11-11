#version 150

out vec4  fColor;
in vec3 Color;

void
main()
{
    fColor = vec4(Color, 1.0);//vec4( 1.0, 0.0, 0.0, 1.0 );
}
