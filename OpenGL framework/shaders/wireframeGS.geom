#version 330 core
uniform mat4 projection;
uniform mat4 model;
uniform mat4 iModelViewNormal;

layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.2;

out vec3 vertexNormal;

void doEmitVertex(vec3 p, vec3 n) {
	vertexNormal = (iModelViewNormal * vec4(n, 0)).xyz;;
	gl_Position = projection * model * vec4(p, 1);
	EmitVertex();
}
 

//void GenerateLine(int index)
//{
//    gl_Position = projection * gl_in[index].gl_Position;
//    EmitVertex();
//    gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
//    EmitVertex();
//    EndPrimitive();
//}

//void main()
//{
//    GenerateLine(0); // first vertex normal
//    GenerateLine(1); // second vertex normal
//    GenerateLine(2); // third vertex normal
//}
void Edge(int a , int b)
{
	gl_Position = projection *  vec4(gl_in[a].gl_Position.xyz, 0);
	EmitVertex();
	gl_Position = projection *  vec4(gl_in[b].gl_Position.xyz , 0);
	EmitVertex();
	EndPrimitive();

}

void main()
{
	//for(int i = 0; i < gl_in.length(); i++) 
	//{
	//	doEmitVertex( gl_in[i].gl_Position.xyz, gs_in[i].normal);
	//}
	//EndPrimitive();

	for(int i = 0; i < gl_in.length(); i++)
    {
      gl_Position = projection * gl_in[i].gl_Position ;
      EmitVertex();
    }

    EndPrimitive(); 

 
  
}