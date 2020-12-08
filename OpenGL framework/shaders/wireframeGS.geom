#version 330 core
uniform mat4 u_Projection; 

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;
 
void emitLine(vec4 a, vec4 b)
 {
      gl_Position = a;
      EmitVertex();
      gl_Position = b;
      EmitVertex();
      EndPrimitive(); 
 }

void main()
{  
    vec4 u = u_Projection * gl_in[0].gl_Position;
    vec4 v = u_Projection * gl_in[1].gl_Position;
    vec4 w = u_Projection * gl_in[2].gl_Position;
    
    emitLine(u,v);
    emitLine(u,w);
    emitLine(v,w);
   
     
}