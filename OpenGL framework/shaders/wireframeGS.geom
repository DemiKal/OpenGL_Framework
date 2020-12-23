#version 330 core
uniform mat4 u_Projection; 

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;



out vec3 g_BarycentricCoords ;


void emitLine(vec4 a, vec4 b)
 {
     // gl_Position = a;
     // EmitVertex();
     // gl_Position = b;
     // EmitVertex();
     // EndPrimitive(); 
 }

void main()
{  
    vec4 u = u_Projection * gl_in[0].gl_Position;
    vec4 v = u_Projection * gl_in[1].gl_Position;
    vec4 w = u_Projection * gl_in[2].gl_Position;
    
    // g_BarycentricCoords[1] = vec3(0,1,0);
   // g_BarycentricCoords[2] = vec3(0,0,1);

  //  emitLine(u,v);
  //  emitLine(u,w);
  //  emitLine(v,w);
   
    gl_Position = u;
    g_BarycentricCoords = vec3(1,0,0);
    EmitVertex();
    gl_Position = v;
    g_BarycentricCoords = vec3(0,1,0);
    EmitVertex();

    gl_Position = w;
    g_BarycentricCoords = vec3(0,0,1); 
    EmitVertex();
    EndPrimitive(); 

}