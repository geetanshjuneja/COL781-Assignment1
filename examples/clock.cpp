#include "../src/a1.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <ctime>


namespace R = COL781::Software;
// namespace R = COL781::Hardware;
using namespace glm;

int main() {
	R::Rasterizer r;
    if (!r.initialize("Clock", 640, 480,16))
        return EXIT_FAILURE;
    R::ShaderProgram program = r.createShaderProgram(
        r.vsTransform(),
        r.fsConstant()
    );
    vec4 vertices[] = {
		vec4(0.0,  0.0, 0.0, 1.0),
        vec4(1.0, 0.0, 0.0, 1.0),
        vec4( 1.0,  1.0, 0.0, 1.0),
        vec4(0.0, 1.0, 0.0, 1.0)
    };
	ivec3 triangles[] = {
		ivec3(0, 1, 2),
		ivec3( 2, 3, 0)
	};
	R::Object tickmark = r.createObject();
	r.setVertexAttribs(tickmark, 0, 4, vertices);
	r.setTriangleIndices(tickmark, 2, triangles);
    r.useShaderProgram(program);

    while (!r.shouldQuit()) {
        r.clear(vec4(1.0, 1.0, 1.0, 1.0));
        r.setUniform<vec4>(program, "color", vec4(0.0, 0.0, 0.0, 1.0));
        r.drawCircle(320,240,200,glm::vec4(1.0,0.84,0.0,1.0));
        r.drawCircle(320,240,190,glm::vec4(1.0,1.0,1.0,1.0));

        glm::mat4 trans2(1.0f);
        trans2 = glm::translate(trans2,glm::vec3(0.0,0.69,0.0));
        trans2 = glm::rotate(trans2, glm::radians(90.0f),glm::vec3(0.0,0.0,1.0));
        trans2 = glm::scale(trans2,glm::vec3(0.1,0.02,0));
        r.setUniform(program, "transform", trans2);
		r.drawObject(tickmark);

        glm::mat4 trans1(1.0f);
        trans1 = glm::translate(trans1,glm::vec3(0.5,0.0,0.0));
        trans1 = glm::scale(trans1,glm::vec3(0.1,0.02,0));
        r.setUniform(program, "transform", trans1);
		r.drawObject(tickmark);

        glm::mat4 trans4(1.0f);
        trans4 = glm::translate(trans4,glm::vec3(0.0,-0.69,0.0));
        trans4 = glm::rotate(trans4, glm::radians(270.0f),glm::vec3(0.0,0.0,1.0));
        trans4 = glm::scale(trans4,glm::vec3(0.1,0.02,0));
        r.setUniform(program, "transform", trans4);
		r.drawObject(tickmark);

        glm::mat4 trans3(1.0f);
        trans3 = glm::translate(trans3,glm::vec3(-0.5,0.0,0.0));
        trans3 = glm::rotate(trans3, glm::radians(180.0f),glm::vec3(0.0,0.0,1.0));
        trans3 = glm::scale(trans3,glm::vec3(0.1,0.02,0));
        r.setUniform(program, "transform", trans3);
		r.drawObject(tickmark);


        //hour
        time_t now = time(0);
        tm* current_t = localtime(&now);
        int hr = current_t->tm_hour;
        int min = current_t->tm_min;
        int sec = current_t->tm_sec;

        glm::mat4 trans7(1.0);
        trans7 = rotate(trans7,radians(-6.0f * sec + 90.0f), vec3(0.0f, 0.0f, 1.0f));
        // trans7 = translate(trans7,vec3(0.0,-0.003,0.0));
        trans7 = scale(trans7, vec3(0.49,0.006,1));
        trans7 = translate(trans7, vec3(0,-0.5f,0));
        r.setUniform(program, "transform", trans7);
        r.drawObject(tickmark);

        glm::mat4 trans5(1.0f);
        trans5 = glm::rotate(trans5,radians(-30.0f * hr + 90.0f), vec3(0.0f, 0.0f, 1.0f));    
        // trans5 = translate(trans5, vec3(0,-0.01,0));
        trans5 = scale(trans5, vec3(0.3,0.009,1));                                           
        trans5 = translate(trans5, vec3(0,-0.5f,0));                                        
        r.setUniform(program, "transform", trans5);
        r.drawObject(tickmark);

        glm::mat4 trans6(1.0);
        trans6 = rotate(trans6,radians(-6.0f * min + 90.0f), vec3(0.0f, 0.0f, 1.0f));
        // trans6 = translate(trans6, vec3(0.0,-0.05,0.0));
        trans6 = scale(trans6, vec3(0.4,0.01,1));
        trans6 = translate(trans6, vec3(0,-0.5f,0));
        r.setUniform(program, "transform", trans6);
        r.drawObject(tickmark);

        r.show();
    }
    r.deleteShaderProgram(program);
    return EXIT_SUCCESS;
}
