#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 aPos;

void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
