#version 120

/********************************Interpolated values from the vertex shaders************************/
varying vec3 fragmentColor;

void main(){
    /********************************Rendering fragments one by one*************************************/
    gl_FragColor = vec4(fragmentColor, 1);
    
}
