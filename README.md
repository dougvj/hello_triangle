# Hello Triangle
Simple hello triangle OpenGL example using SDL2 and GLEW

## Compiling

`meson <build dir>`

`meson compile -C <build dir>`

Because this is so simple, you can also simply compile manually and not use
a build system. This command works on most Linuxes after installing the dev 
packages for libraries:

`gcc main.c -o hello_triangle -lGL -lGLEW -lSDL2 -lGLU`


## Running

`./<build dir>/hello_triangle`

## Notes

You must run the executable from the same working directory as the shader files
