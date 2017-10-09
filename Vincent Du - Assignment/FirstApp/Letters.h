//////////////////////////////////////////////////////////////////////////////
//
//  --- Letters.h ---
//
//   The main header file for letter generation source file Letters.cpp
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __LETTERS_H__
#define __LETTERS_H__

#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
//#  include "GLee.h"
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__

