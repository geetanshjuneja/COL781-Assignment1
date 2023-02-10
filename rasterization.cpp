// Rasterization starter code for COL781: Computer Graphics

/* Instructions for execution:
    1. Install SDL2 and SDL2_image libraries
    2. Compile using: g++ starter_code.cpp -I/path/to/SDL2 -lSDL2 -lSDL2_image
       (on Linux or MacOS, it should be sufficient to copy-paste the following:
       g++ starter_code.cpp `pkg-config --cflags --libs SDL2 SDL2_image`
    3. Run using: ./a.out
    4. The rendered frame will be shown in a window, and saved to a file on exit
*/
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


// Framebuffer
const int frameWidth = 10;
const int frameHeight = 10;

const int displayScale = 40; // display scaled version of framebuffer so you can see the pixels

SDL_Surface* framebuffer = NULL;

// SDL parameters

SDL_Window* window = NULL;
SDL_Surface *windowSurface = NULL;
bool quit = false;

// Output file
const char* outputFile = "out.png";

// Function prototypes
bool initialize();
void handleEvents();
void saveFramebuffer();
void terminate();

class SoftwareRasterizer {
    private:
        SDL_Surface *surface;
        const Uint32 ColorScale = 255;

        bool isLeft(float x1, float y1, float x2, float y2, float x, float y){
            glm::vec2 perp(y1-y2,x2-x1); 
            glm::vec2 v(x-x1,y-y1);
            float dt = glm::dot(v,perp);
            if(dt>=0) return true;
            return false;
        }

        bool insideTriangle(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 P){
            bool flag = isLeft(A.x,A.y,B.x,B.y,P.x,P.y);
            flag &= isLeft(B.x,B.y,C.x,C.y,P.x,P.y);
            flag &= isLeft(C.x,C.y,A.x,A.y,P.x,P.y);
            return flag;
        }

    public:

        SoftwareRasterizer(SDL_Surface *surf){
            surface = surf;
        }
        
        void clearBuffer(glm::vec4 color){
            color = color*(float)ColorScale;
            // std::cout << glm::to_string(color) << std::endl;
            SDL_LockSurface(surface);
            Uint8 r = (Uint8)color.w, g = (Uint8)color.x, b = (Uint8)color.y, a = (Uint8)color.z;
            SDL_memset(surface->pixels, (int) SDL_MapRGBA(surface->format,r,g,b,a), surface->h * surface->pitch);
            SDL_UnlockSurface(surface); 
        }

        void createRasterTriangle(glm::vec4 A, glm::vec4 B, glm::vec4 C, glm::vec4 color){
            std::cout << (A.x+1.0f)*frameWidth/2 << " " <<(A.y+1.0f)*frameWidth/2 << std::endl;
            glm::vec2 a((A.x+1.0f)*frameWidth/2, (A.y+1.0f)*frameHeight/2);
            glm::vec2 b((B.x+1.0f)*frameWidth/2, (B.y+1.0f)*frameHeight/2);
            glm::vec2 c((C.x+1.0f)*frameWidth/2, (C.y+1.0f)*frameHeight/2); 
            color = color*(float)ColorScale;
            Uint8 red = (Uint8)color.w, green = (Uint8)color.x, blue = (Uint8)color.y, alpha = (Uint8)color.z;
            Uint32 col = SDL_MapRGBA(surface->format,red,green,blue,alpha);
            Uint32 *pixels = (Uint32*)framebuffer->pixels;
            for (int i = 0; i < frameWidth; i++) {
                for (int j = 0; j < frameHeight; j++) {
					float x = i + 0.5;
					float y = j + 0.5;
                    glm::vec2 p(x,y);
                    if(insideTriangle(a,b,c,p)) pixels[i + frameWidth*j] = col;
				}
            }
        }
};

int main(int argc, char* args[]) {
    if (!initialize()) {
        printf("Failed to initialize!");
    } else {
        // Display and interaction
        while (!quit) {
            // Event handling
            handleEvents();

            // Set pixel data: CHANGE THIS TO YOUR OWN CODE!
			// Uint32 *pixels = (Uint32*)framebuffer->pixels;
			// SDL_PixelFormat *format = framebuffer->format;
            // for (int i = 0; i < frameWidth; i++) {
            //     for (int j = 0; j < frameHeight; j++) {
			// 		float x = i + 0.5;
			// 		float y = j + 0.5;
			// 		float r = 4;
			// 		Uint32 color;
			// 		if ((x-5)*(x-5) + (y-5)*(y-5) <= r*r) { // circle
			// 			color = SDL_MapRGBA(format, 0, 153, 0, 255); // green
			// 		} else {
			// 			color = SDL_MapRGBA(format, 255, 255, 255, 255); // white
			// 		}
			// 		pixels[i + frameWidth*j] = color;
			// 	}
            // }

            SoftwareRasterizer obj1(framebuffer);
            glm::vec4 c(1.0f);
            obj1.clearBuffer(c);
            obj1.createRasterTriangle(glm::vec4(-0.4, -0.8, 0.0, 1.0),glm::vec4(-0.4, -0.4, 0.0, 1.0),glm::vec4(-0.8,  0.0, 0.0, 1.0),glm::vec4(1.0,0.0,0.0,1.0f));

            // Update screen to apply the changes
            SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
            SDL_UpdateWindowSurface(window);
        }
    }
	// Save image
    saveFramebuffer();
    terminate();
    return 0;
}

// Initialising SDL2
bool initialize() {
    bool success = true;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        success = false;
    } else {
		int screenWidth = frameWidth * displayScale;
		int screenHeight = frameHeight * displayScale;
        window = SDL_CreateWindow("COL781", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s", SDL_GetError());
            success = false;
        } else {
			windowSurface = SDL_GetWindowSurface(window);
            framebuffer = SDL_CreateRGBSurface(0, frameWidth, frameHeight, 32, 0, 0, 0, 0);
        }
    }
    return success;
}

// Handle window exit 
void handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }
    }
}
    
void saveFramebuffer() {
    // Save the image
    IMG_SavePNG(framebuffer, outputFile);
}

void terminate() {
    // Free resources and close SDL
    SDL_FreeSurface(framebuffer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
