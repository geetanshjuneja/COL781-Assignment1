#include "../src/a1.hpp"
#include <glm/gtc/matrix_transform.hpp>

// namespace R = COL781::Software;
namespace R = COL781::Hardware;
using namespace glm;


int main(){
    R::Rasterizer r;
    int width = 1366, height = 768;
    if (!r.initialize("Ruby", 1366, 768))
        return EXIT_FAILURE;

    R::ShaderProgram program = r.createShaderProgram(
        r.vsColorTransform(),
        r.fsIdentity()
    );

    float cos60 = 0.5000000, sin60 = 0.86602540378;
    float red11 = 0.80392156862, red12 = 0.16078431372, red13 = 0.1725490196, red14 = 1;
    float red21 = 0.47058823529, red22 = 0.18431372549, red23 = 0.19215686274, red24 = 1;
    float red31 = 0.30980392156, red32 = 0.18431372549, red33 = 0.19215686274, red34 = 1;


    vec4 vertices[] = {
        // Center of Top Hexagon
        vec4(  0,  0.4, 0, 1.0),

        // Top Hexagon Verticees
        vec4(  0.4,  0.4, 0.0, 1.0),
		vec4( 0.4*cos60,  0.4, 0.4*sin60, 1.0),
        vec4( -0.4*cos60,  0.4, 0.4*sin60, 1.0),
        vec4(  -0.4,  0.4, 0, 1.0),
		vec4( -0.4*cos60,  0.4, -0.4*sin60, 1.0),
        vec4( 0.4*cos60,  0.4, -0.4*sin60, 1.0),

        // Center of Bottom Hexagon
        vec4(  0,  0.2, 0, 1.0),

        // Bottom Hexagon Vertices
        vec4(  0.6,  0.2, 0.0, 1.0),
		vec4( 0.6*cos60,  0.2, 0.6*sin60, 1.0),
        vec4( -0.6*cos60,  0.2, 0.6*sin60, 1.0),
        vec4(  -0.6,  0.2, 0, 1.0),
		vec4( -0.6*cos60,  0.2, -0.6*sin60, 1.0),
        vec4( 0.6*cos60,  0.2, -0.6*sin60, 1.0),

        // Bottom Tip
        vec4( 0,  -0.6, 0, 1.0),

    };

    vec4 colors[] = {
		vec4(red21, red22, red23, 1.0),
        vec4(red11, red12, red13, 1.0),
		vec4(red11, red12, red13, 1.0),
        vec4(red31, red32, red33, 1.0),
        vec4(red31, red32, red33, 1.0),
		vec4(red31, red32, red33, 1.0),
        vec4(red11, red12, red13, 1.0),
        vec4(red21, red22, red23, 1.0),
		vec4(red11, red12, red13, 1.0),
		vec4(red11, red12, red13, 1.0),
        vec4(red31, red32, red33, 1.0),
        vec4(red31, red32, red33, 1.0),
		vec4(red31, red32, red33, 1.0),
        vec4(red11, red12, red13, 1.0),
        vec4(red21, red22, red23, 1.0),
    };

	ivec3 triangles[] = {
        // Connections of Top Hexagon

		ivec3(0, 1, 2),
		ivec3(0, 2, 3),
        ivec3(0, 3, 4),
        ivec3(0, 4, 5),
        ivec3(0, 5, 6),
        ivec3(0, 6, 1),

        // Connections of Bottom Hexagon

		ivec3(7, 8, 9),
		ivec3(7, 9, 10),
        ivec3(7, 10, 11),
        ivec3(7, 11, 12),
        ivec3(7, 12, 13),
        ivec3(7, 13, 8),

        // Connecting Top and Bottom Hexagon
        ivec3(1, 8, 9),
		ivec3(1, 2, 9),
        ivec3(2, 9, 10),
        ivec3(2, 3, 10),
        ivec3(3, 10, 11),
        ivec3(3, 4, 11),
        ivec3(4, 11, 12),
		ivec3(4, 5, 12),
        ivec3(5, 12, 13),
        ivec3(5, 6, 13),
        ivec3(6, 13, 8),
        ivec3(6, 1, 8),

        // Connecting Bottom Hexagon to Bottom Tip

        ivec3(14, 8, 9),
		ivec3(14, 9, 10),
        ivec3(14, 10, 11),
        ivec3(14, 11, 12),
        ivec3(14, 12, 13),
        ivec3(14, 13, 8),

	};



    R::Object pyramid = r.createObject();
    R::Object shape = r.createObject();
	r.setVertexAttribs(shape, 0, 15, vertices);
	r.setVertexAttribs(shape, 1, 15, colors);
	r.setTriangleIndices(shape, 30, triangles);
    r.enableDepthTest();

    mat4 model = mat4(1.0f);
	mat4 view = translate(mat4(1.0f), vec3(0.0f, 0.0f, -2.0f)); 
    mat4 projection = perspective(radians(60.0f), (float)width/(float)height, 0.1f, 100.0f);
    float speed = 0.25 * 90.0f; // degrees per second
    while (!r.shouldQuit()) {
        float time = SDL_GetTicks64()*1e-3;
        r.clear(vec4(1.0, 1.0, 1.0, 1.0));
        r.useShaderProgram(program);
        model = rotate(mat4(1.0f), radians(speed * time), vec3(1.0f,1.0f,1.0f));
        r.setUniform(program, "transform", projection * view * model);
		r.drawObject(shape);
        r.show();
    }
    r.deleteShaderProgram(program);
    return EXIT_SUCCESS;


}