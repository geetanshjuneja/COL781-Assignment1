#include <stdio.h>
#include <iostream>
#include<vector>
#include<bits/stdc++.h>

// SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Open GL
#include <glm/glm.hpp>
#include <glm/vec4.hpp>



class SoftwareRasterizer{
    private:
        // Framebuffer
        int frameWidth;
        int frameHeight;
        int displayScale; // display scaled version of framebuffer so you can see the pixels
        SDL_Surface* framebuffer = NULL;

        // SDL parameters
        SDL_Window* window = NULL;
        SDL_Surface *windowSurface = NULL;
        bool quit = false;

        // Output file
        const char* outputFile = "out.png";

        // For supersampling
        int samples = 1;


        // For point in triangle test : Barycentric Method Used
        bool insideTriangle(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 P){

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
    
    public:

        SoftwareRasterizer(int _frameWidth = 10, int _frameHeight = 10, int _displayScale = 40){
            frameWidth = _frameWidth;
            frameHeight = _frameHeight;
            displayScale = _displayScale;

            if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
                throw SDL_GetError();
            } 
            else {
                int screenWidth = frameWidth * displayScale;
                int screenHeight = frameHeight * displayScale;
                window = SDL_CreateWindow("COL781", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
                if (window == NULL) {
                    throw SDL_GetError();
                } 
                else {
                    windowSurface = SDL_GetWindowSurface(window);
                    framebuffer = SDL_CreateRGBSurface(0, frameWidth, frameHeight, 32, 0, 0, 0, 0);
                }
            }

        }

        // Destroy class and SDL Surfaces
        ~SoftwareRasterizer(){
            SDL_FreeSurface(framebuffer);
            framebuffer = NULL;

            SDL_DestroyWindow(window);
            window = NULL;
            windowSurface = NULL;

            SDL_Quit();
        }

        void setSamples(int _samples){
            int x = sqrt(_samples) + 2;
            while (x * x > _samples) x--;
            samples = x;
        }
            
        // Save the image
        void saveFramebuffer() {
            IMG_SavePNG(framebuffer, outputFile);
        }

        // Handle window exit 
        void handleEvents(){
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
            }
        }


        // Clear Frame Buffer
        void clearBuffer(glm::vec4 color){
            color *= 255;
            Uint8 r = (Uint8)color.w, g = (Uint8)color.x, b = (Uint8)color.y;
            SDL_FillRect(framebuffer, NULL, SDL_MapRGB(framebuffer->format, r, g, b));
        }

        void createRasterTriangle(glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec4 color){
            color *= 255;
            Uint8 red = (Uint8)color.w, green = (Uint8)color.x, blue = (Uint8)color.y, alpha = (Uint8)color.z;

            glm::vec2 t1((A.x+1.0f)*frameWidth/2, (A.y+1.0f)*frameHeight/2);
            glm::vec2 t2((B.x+1.0f)*frameWidth/2, (B.y+1.0f)*frameHeight/2);
            glm::vec2 t3((C.x+1.0f)*frameWidth/2, (C.y+1.0f)*frameHeight/2);
            Uint32 *pixels = (Uint32*)framebuffer->pixels;

            float offset = 0.5/(samples);
            int totalSamples = samples*samples;

            for (int i = 0; i < frameWidth; i++) {
                for (int j = 0; j < frameHeight; j++){

                    // float x = i + 0.5;
					// float y = j + 0.5;
                    // glm::vec2 P(x,y);
                    // if(insideTriangle(t1, t2, t3, P)){
                    //     pixels[i + frameWidth*j] = SDL_MapRGBA(framebuffer->format,red,green,blue,alpha);
                    //     std::cout << "YES" << std::endl;
                    // }
                    // else{
                    //     std::cout << "NO" << std::endl;
                    // }
                    
                    // I have (i,j) sample
                    glm::ivec4 sampleColor(0, 0, 0, 0);
                    SDL_GetRGBA(pixels[i + frameWidth*(frameHeight-1-j)], framebuffer->format, &red, &green, &blue, &alpha);
                    glm::ivec4 previousColor(red, green, blue, alpha);

                    for (int k = i+offset; k < i+1; k += offset){
                        for (int l = j+offset; l < j+1; l += offset){
                            glm::vec2 P(k, l);
                            if (insideTriangle(A, B, C, P)){
                                sampleColor += color;
                            }
                            else{
                                sampleColor += previousColor;
                            }

                        }
                    }

                    sampleColor /= totalSamples;
                    pixels[i + frameWidth*(frameHeight - 1 - j)] = SDL_MapRGBA(framebuffer->format, sampleColor[0], sampleColor[1], sampleColor[2], sampleColor[3]);

				}
                

            }

        }



        // Game Loop + Rendering
        void display(){
            while(!quit){
                // Event handling
                handleEvents();

                // Clear Buffer
                clearBuffer(glm::vec4(1.0f));

                // Draw Triangle
                createRasterTriangle(glm::vec4(-0.4, -0.8, 0.0, 1.0),glm::vec4(-0.4, -0.4, 0.0, 1.0),glm::vec4(-0.8,  0.0, 0.0, 1.0),glm::vec4(1.0,0.0,0.0,1.0f));
                
                // Update screen to apply the changes
                SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
                SDL_UpdateWindowSurface(window);
            }

        }


};

int main(int argc, char* args[]) {
    SoftwareRasterizer srz(10,10,40);
    srz.display();
    return 0;
}



    