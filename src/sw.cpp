#include "sw.hpp"

#include <iostream>
#include <vector>

const Uint32 COLORSCALE = 255;

namespace COL781 {
	namespace Software {

		// Forward declarations

		template <> float Attribs::get(int index) const;
		template <> glm::vec2 Attribs::get(int index) const;
		template <> glm::vec3 Attribs::get(int index) const;
		template <> glm::vec4 Attribs::get(int index) const;

		template <> void Attribs::set(int index, float value);
		template <> void Attribs::set(int index, glm::vec2 value);
		template <> void Attribs::set(int index, glm::vec3 value);
		template <> void Attribs::set(int index, glm::vec4 value);

		// Built-in shaders

		VertexShader Rasterizer::vsIdentity() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				return vertex;
			};
		}

		VertexShader Rasterizer::vsTransform() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::mat4 transform = uniforms.get<glm::mat4>("transform");
				return transform * vertex;
			};
		}

		VertexShader Rasterizer::vsColor() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::vec4 color = in.get<glm::vec4>(1);
				out.set<glm::vec4>(0, color);
				return vertex;
			};
		}

		FragmentShader Rasterizer::fsConstant() {
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec4 color = uniforms.get<glm::vec4>("color");
				return color;
			};
		}

		FragmentShader Rasterizer::fsIdentity() {
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec4 color = in.get<glm::vec4>(0);
				return color;
			};
		}

		// Implementation of Attribs and Uniforms classes

		void checkDimension(int index, int actual, int requested) {
			if (actual != requested) {
				std::cout << "Warning: attribute " << index << " has dimension " << actual << " but accessed as dimension " << requested << std::endl;
			}
		}

		template <> float Attribs::get(int index) const {
			checkDimension(index, dims[index], 1);
			return values[index].x;
		}

		template <> glm::vec2 Attribs::get(int index) const {
			checkDimension(index, dims[index], 2);
			return glm::vec2(values[index].x, values[index].y);
		}

		template <> glm::vec3 Attribs::get(int index) const {
			checkDimension(index, dims[index], 3);
			return glm::vec3(values[index].x, values[index].y, values[index].z);
		}

		template <> glm::vec4 Attribs::get(int index) const {
			checkDimension(index, dims[index], 4);
			return values[index];
		}

		void expand(std::vector<int> &dims, std::vector<glm::vec4> &values, int index) {
			if (dims.size() < index+1)
				dims.resize(index+1);
			if (values.size() < index+1)
				values.resize(index+1);
		}

		template <> void Attribs::set(int index, float value) {
			expand(dims, values, index);
			dims[index] = 1;
			values[index].x = value;
		}

		template <> void Attribs::set(int index, glm::vec2 value) {
			expand(dims, values, index);
			dims[index] = 2;
			values[index].x = value.x;
			values[index].y = value.y;
		}

		template <> void Attribs::set(int index, glm::vec3 value) {
			expand(dims, values, index);
			dims[index] = 3;
			values[index].x = value.x;
			values[index].y = value.y;
			values[index].z = value.z;
		}

		template <> void Attribs::set(int index, glm::vec4 value) {
			expand(dims, values, index);
			dims[index] = 4;
			values[index] = value;
		}

		template <typename T> T Uniforms::get(const std::string &name) const {
			return *(T*)values.at(name);
		}

		template <typename T> void Uniforms::set(const std::string &name, T value) {
			auto it = values.find(name);
			if (it != values.end()) {
				delete it->second;
			}
			values[name] = (void*)(new T(value));
		}

		bool Rasterizer::initialize(const std::string &title, int width, int height, int spp){
			bool success = true;
			if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
				printf("SDL could not initialize! SDL_Error: %s", SDL_GetError());
				success = false;
			} else {
				int displayScale = 1;
				int screenWidth = width * displayScale;
				int screenHeight = height * displayScale;
				window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
				if (window == NULL) {
					printf("Window could not be created! SDL_Error: %s", SDL_GetError());
					success = false;
				} else {
					SDL_Surface *windowSurface = SDL_GetWindowSurface(window);
					framebuffer = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
				}
			}
			return success;
		}

		bool Rasterizer::shouldQuit() {
			SDL_Event e;
			bool t = false;
			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_QUIT) {
					t = true;
				}
			}
			return t;
		}

		ShaderProgram Rasterizer::createShaderProgram(const VertexShader &vs, const FragmentShader &fs){
			struct ShaderProgram program;
			program.vs = vs;
			program.fs = fs;
			program.uniforms = Uniforms();
			return program;
		}

		void Rasterizer::useShaderProgram(const ShaderProgram &program) {
			myprogram = program;
		}

		template <typename T> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, T value){
			program.uniforms.set(name,value);
		}

		void Rasterizer::deleteShaderProgram(ShaderProgram &program){
			delete &program;
		}

		template <typename T> void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, const T* data){
			for(int i=0; i<n; i++){
				Attribs temp();
				
			}
		}

		void Rasterizer::setTriangleIndices(Object &object, int n, glm::ivec3* indices){
			for(int i=0; i<n; i++){
				object.indices.push_back(indices[i]);
			}
		}

		void Rasterizer::enableDepthTest(){

		}

		void Rasterizer::clear(glm::vec4 color){
			color = color*(float)COLORSCALE;
            // std::cout << glm::to_string(color) << std::endl;
            SDL_LockSurface(framebuffer);
            Uint8 r = (Uint8)color.w, g = (Uint8)color.x, b = (Uint8)color.y, a = (Uint8)color.z;
            SDL_memset(framebuffer->pixels, (int) SDL_MapRGBA(framebuffer->format,r,g,b,a), framebuffer->h * framebuffer->pitch);
            SDL_UnlockSurface(framebuffer); 
		}

		void Rasterizer::drawObject(const Object &object){

		}

		void Rasterizer::show(){
			SDL_Surface* windowSurface = SDL_GetWindowSurface(window);
			SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
            SDL_UpdateWindowSurface(window);
		}


	}
}
