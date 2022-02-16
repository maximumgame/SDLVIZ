//glad loader is required outside of emscripten
//em will use its own static pointers to gl functions

#ifdef __EMSCRIPTEN__
#include <GL/gl.h>
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif