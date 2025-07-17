#version 410 core

#ifdef GL_ES
precision mediump float;
#endif

//uniform vec2 u_resolution;
//uniform vec3 u_mouse;
//uniform float u_time;
uniform vec4 u_color;

void main() {
    //vec2 st = gl_FragCoord.st/u_resolution;
    //gl_FragColor = vec4(st.x,st.y,0.0,1.0);
    gl_FragColor = u_color;
}
