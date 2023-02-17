
#include "sw.hpp"

#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <memory>

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

        /*
            Added vsColorTransform
        */

        VertexShader Rasterizer::vsColorTransform() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::vec4 color = in.get<glm::vec4>(1);
				out.set<glm::vec4>(0, color);
                glm::mat4 transform = uniforms.get<glm::mat4>("transform");
				return transform*vertex;
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
			//if (it != values.end()) {
				//delete it->second;
			//}
			values[name] = (void*)(new T(value));
		}


        // New functions added by me

        bool Rasterizer::initialize(const std::string &title, int width, int height, int spp){
            bool success = true;
            
            if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
                success = false;
            }
            else{
                window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

                if (window == NULL){
                    success = false;
                }
                else{
                    windowSurface = SDL_GetWindowSurface(window);
                    frameBuffer = SDL_CreateRGBSurface(0,width,height, 32, 0, 0, 0, 0);
                    totalSamples = spp;
                    frameWidth = width;
                    frameHeight = height;
					zbuffer = new float[width*height];
                }

            }

            return success;

        }


        bool Rasterizer::shouldQuit(){
			return quit;
		}

        ShaderProgram Rasterizer::createShaderProgram(const VertexShader &vs, const FragmentShader &fs){
			struct ShaderProgram program;
			program.vs = vs;
			program.fs = fs;
			program.uniforms = Uniforms();
			return program;
		}

        void Rasterizer::useShaderProgram(const ShaderProgram &program){
			myProgram = &program;
		}

        
        template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, float value) {
			program.uniforms.set<float>(name,value);
		}
		
		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, int value) {
			program.uniforms.set<int>(name,value);
		}

		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::vec2 value) {
			program.uniforms.set<glm::vec2>(name,value);
		}
		
		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::vec3 value) {
			program.uniforms.set<glm::vec3>(name,value);
		}
		
		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::vec4 value) {
			program.uniforms.set<glm::vec4>(name,value);
		}

		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::mat2 value) {
			program.uniforms.set<glm::mat2>(name,value);
		}

		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::mat3 value) {
			program.uniforms.set<glm::mat3>(name,value);
		}

		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::mat4 value) {
			program.uniforms.set<glm::mat4>(name,value);
		}


        void Rasterizer::deleteShaderProgram(ShaderProgram &program){
			// create destructor in uniforms instead
		}

        Object Rasterizer::createObject(){
			return Object();
		}

        // Float
        template <> void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, const float* data) {
            int x = object.attributes.size();
            for (int i = x; i < n; i++){
                object.attributes.push_back(Attribs());
            }

			for (int i = 0; i < n; i++){
				object.attributes[i].set<float>(attribIndex, data[i]);
			}		
		}

        //glm::vec2
        template <> void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, const glm::vec2* data) {
            int x = object.attributes.size();
            for (int i = x; i < n; i++){
                object.attributes.push_back(Attribs());
            }

			for (int i = 0; i < n; i++){
				object.attributes[i].set<glm::vec2>(attribIndex, data[i]);
			}		
		}

        //glm::vec3
        template <> void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, const glm::vec3* data) {
            int x = object.attributes.size();
            for (int i = x; i < n; i++){
                object.attributes.push_back(Attribs());
            }

			for (int i = 0; i < n; i++){
				object.attributes[i].set<glm::vec3>(attribIndex, data[i]);
			}		
		}

        //glm::vec4
        template <> void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, const glm::vec4* data) {
            int x = object.attributes.size();
            for (int i = x; i < n; i++){
                object.attributes.push_back(Attribs());
            }

			for (int i = 0; i < n; i++){
				object.attributes[i].set<glm::vec4>(attribIndex, data[i]);
			}
		}


        // set indices
        void Rasterizer::setTriangleIndices(Object &object, int n, glm::ivec3* indices) {
			object.indices = std::vector<glm::ivec3>(indices,indices+n);
		}



        void Rasterizer::enableDepthTest(){
			depthTest = true;
		}


        void Rasterizer::clear(glm::vec4 color){
			color *= 255;
			Uint8 r = (Uint8)color.w, g = (Uint8)color.x, b = (Uint8)color.y;
            SDL_FillRect(frameBuffer, NULL, SDL_MapRGB(frameBuffer->format, r, g, b));
			for (int i = 0; i < frameHeight*frameWidth; i++){
				zbuffer[i] = 1e8;
			}
		}

		void Rasterizer::drawCircle(int c_x, int c_y, int c_r, glm::vec4 color){
			int samples = sqrt(totalSamples) + 2;
            while(samples*samples > totalSamples) samples--;
            color *= 255;
            Uint32 *pixels = (Uint32*)frameBuffer->pixels;
            float offset = 0.5/(samples);
            int total_Samples = samples*samples;

            for (int i = 0; i < frameWidth; i++){
                for (int j = 0; j < frameHeight; j++){

                    Uint8 r, g, b, a;
                    glm::vec4 sampleColor(0, 0, 0, 0);
                    SDL_GetRGBA(pixels[i + frameWidth*(frameHeight-1-j)], frameBuffer->format, &r, &g, &b, &a);
                    glm::vec4 previousColor(r,g,b,a);

                    for (int k = 1; k <= samples; k++){
                        for (int l = 1; l <= samples; l++){
                            int x = i + (2*k - 1)*offset, y = j + (2*l - 1)*offset;

                            if ((x-c_x)*(x-c_x) + (y-c_y)*(y-c_y) <= c_r*c_r){
                                sampleColor += color;
                            } 
                            else{
                                sampleColor += previousColor;
                            }
                        }
                    }

                    sampleColor /= total_Samples;
                    pixels[i + frameWidth*(frameHeight-1-j)] = SDL_MapRGBA(frameBuffer->format, sampleColor[0], sampleColor[1], sampleColor[2], sampleColor[3]);

                }
            }
        }


        // for drawing
        bool Rasterizer::insideTriangle(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 P){

            // Doubles used for more precision
            double x1 = A[0], y1 = A[1];
            double x2 = B[0], y2 = B[1];
            double x3 = C[0], y3 = C[1];
            double x  = P[0], y  = P[1];
            
            double a = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3));
            double b = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3));
            double c = 1 - a - b;

            return (0 <= a & a <= 1) & (0 <= b & b <= 1) & (0 <= c & c <= 1);

        }


        void Rasterizer::createRasterTriangle(glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec4 C1, glm::vec4 C2, glm::vec4 C3){

            C1 *= 255;
			C2 *= 255;
			C3 *= 255;

			// Perspective Division
			if (depthTest){
				A /= A[3];
				B /= B[3];
				C /= C[3];
			}

			auto area = [&] (glm::vec2 t1, glm::vec2 t2, glm::vec2 t3){
				float x1 = t1.x, y1 = t1.y;
				float x2 = t2.x, y2 = t2.y;
				float x3 = t3.x, y3 = t3.y;
				return 0.5*abs(x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2));
			};

			auto interpolater = [&] (glm::vec2 t1, glm::vec2 t2, glm::vec2 t3, glm::vec4 C1, glm::vec4 C2, glm::vec4 C3, glm::vec2 P, float p1=1, float p2=1, float p3=1){
				float triangleArea = area(t1, t2, t3);
				float phi1 = area(P, t2, t3)/triangleArea, phi2 = area(P, t1, t3)/triangleArea;
				float phi3 = 1-phi1-phi2;

				glm::vec4 color(0);
				color = p1*phi1*C1 + p2*phi2*C2 + p3*phi3*C3;
				return color;
			};

			auto zinterpolater = [&] (glm::vec2 t1, glm::vec2 t2, glm::vec2 t3, float d1, float d2, float d3, glm::vec2 P){
				float triangleArea = area(t1, t2, t3);
				// std::cout << triangleArea << std::endl;
				// std::cout << t1[0] << " " << t1[1] << " " << t2[0] << " " << t2[1] << " " << t3[0] << " " << t3[1] << " " << d1 << "-" << d2 << "-" << d3 << " "  << triangleArea << std::endl;
				float phi1 = area(P, t2, t3)/triangleArea, phi2 = area(P, t1, t3)/triangleArea;
				float phi3 = 1-phi1-phi2;

				return  phi1*d1 + phi2*d2 + phi3*d3;
			};

            glm::vec2 t1((A.x+1.0f)*frameWidth/2, (A.y+1.0f)*frameHeight/2);
            glm::vec2 t2((B.x+1.0f)*frameWidth/2, (B.y+1.0f)*frameHeight/2);
            glm::vec2 t3((C.x+1.0f)*frameWidth/2, (C.y+1.0f)*frameHeight/2);
            Uint32 *pixels = (Uint32*)frameBuffer->pixels;

            int samples = sqrt(totalSamples) + 2;
            while(samples*samples > totalSamples) samples--;

            float offset = 0.5/(samples);
            totalSamples = samples*samples;

            int xmin = (int)std::min({t1.x, t2.x, t3.x}),xmax = (int)std::max({t1.x, t2.x, t3.x}) + 1;
            int ymin = (int)std::min({t1.y, t2.y, t3.y}),ymax = (int)std::max({t1.y, t2.y, t3.y}) + 1;

            for (int i = xmin; i <= std::min(frameWidth-1,xmax); i++) {
                for (int j = ymin; j <= std::min(frameHeight-1,ymax); j++){
					glm::vec2 z(i+0.5, j+0.5);
					if (depthTest && zinterpolater(t1, t2, t3, A[2], B[2], C[2], z) >= zbuffer[i+frameWidth*(frameHeight-1-j)]){
						continue;
					}
					
                    Uint8 r, g, b, a;
                    glm::vec4 sampleColor(0, 0, 0, 0);
                    SDL_GetRGBA(pixels[i + frameWidth*(frameHeight-1-j)], frameBuffer->format, &r, &g, &b, &a);
                    glm::vec4 previousColor(r,g,b,a);

					bool flag = false;
                    for (int k = 1; k <= samples; k += 1){
                        for (int l = 1; l <= samples; l += 1){
                            glm::vec2 P(i + (2*k-1)*offset, j + (2*l-1)*offset);
                            if (insideTriangle(t1, t2, t3, P)){
								flag = true;
                                sampleColor += interpolater(t1, t2, t3, C1, C2, C3, P);
                            }
                            else{
                                sampleColor += previousColor;
                            }
                        }
                    }

					if (!flag) continue;
                    sampleColor /= totalSamples;

					auto temp = zinterpolater(t1, t2, t3, A[2], B[2], C[2], z);
					if(!depthTest){
						pixels[i + frameWidth * (frameHeight - 1 - j)] = SDL_MapRGBA(frameBuffer->format, sampleColor[0], sampleColor[1], sampleColor[2], sampleColor[3]);
					}

					if(depthTest && temp <= zbuffer[i + frameWidth * (frameHeight - 1 - j)]){
						pixels[i+frameWidth*(frameHeight-1-j)] = SDL_MapRGBA(frameBuffer->format, sampleColor[0], sampleColor[1], sampleColor[2], sampleColor[3]);
						zbuffer[i+frameWidth*(frameHeight-1-j)] = temp;
					}

					
				}
                
            }

        }


        void Rasterizer::drawObject(const Object &object){
			auto indices = object.indices;
			for (auto x: indices){
				std::vector<glm::vec4> coordinates_2d(3), colors(3);
				std::vector<Attribs> attributes(3);
				
				for (int i = 0; i < 3; i++){
					coordinates_2d[i] = myProgram->vs(myProgram->uniforms, object.attributes[x[i]], attributes[i]);
				}

				for (int i = 0; i < 3; i++){
					colors[i] = myProgram->fs(myProgram->uniforms, attributes[i]);
				}

				// Draw Triangle Here...
				createRasterTriangle(coordinates_2d[0], coordinates_2d[1], coordinates_2d[2], colors[0], colors[1], colors[2]);

			}
		}


        void Rasterizer::show(){
			SDL_Surface* windowSurface = SDL_GetWindowSurface(window);
			SDL_BlitScaled(frameBuffer, NULL, windowSurface, NULL);
			SDL_UpdateWindowSurface(window);
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0) {
				if(e.type == SDL_QUIT) {
					quit = true;
				}
			}
		}




	}
}