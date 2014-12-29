#include <stdio.h>
#include <assert.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// debugging
#define ERROR(...) fprintf(stderr, __VA_ARGS__);
#define INFO(...) fprintf(stdout, __VA_ARGS__);

#ifdef LOG_DEBUG
#define DEBUG(...) fprintf(stderr, __VA_ARGS__);
#else
#define DEBUG(...) while(0);
#endif

#define EXCEPTION(...) {\
	char msg[8192], xpand[4096]; \
	snprintf(xpand, 4096, __VA_ARGS__); \
	snprintf(msg, 8192, "EXCEPTION in '%s' at line %d: %s", __PRETTY_FUNCTION__, __LINE__, xpand); \
	ERROR("%s\n", msg); exit(0); \
}

typedef struct {
	SDL_Renderer* renderer;
	int running;
} info_t;

int eventThreadFn(void *data) {
	info_t* info = data;
	SDL_Event event;

	while(info->running) {
		// events
		if(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				info->running = 0;
				break;
			}
		}
	}

	return 0;
}

int main(int argc, char** argv)
{
	info_t info = {NULL,0};

	SDL_Thread* eventThread;
	int eventThreadReturn;

	int width = 720;
	int height = 1280;
	float scale = argc>1 ? atof(argv[1]) : 1;

	// sdl init
	if (!SDL_Init(SDL_INIT_VIDEO) < 0)
		EXCEPTION("SDL_Init: %s", SDL_GetError());

	// window
	SDL_Window* window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED,
			     SDL_WINDOWPOS_CENTERED, width*scale, height*scale, SDL_WINDOW_HIDDEN);
	if (!window)
		EXCEPTION("SDL_CreateWindow: %s", SDL_GetError());

	// renderer
	info.renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!info.renderer) {
		EXCEPTION("SDL_CreateRenderer Error: %s", SDL_GetError());
	}
	// scale if needed
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(info.renderer, width, height);

	// IMG
	int imgFlags = IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags) & IMG_INIT_PNG)) {
		EXCEPTION("IMG_Init Error: %s", IMG_GetError());
	}

	// show window
	SDL_ShowWindow(window);
	info.running = 1;

	// start key thread
	eventThread = SDL_CreateThread(eventThreadFn, "EventThread", &info);
	if (!eventThread) EXCEPTION("SDL_CreateThread failed: %s\n", SDL_GetError());

	while(info.running) {
		// make screenshot
		system("timeout 1 /media/Data/repositories/git/bigG/build/tools/take_screenshot /tmp/screen.png");

		// load image
		SDL_Surface* bmp = IMG_Load("/tmp/screen.png");
		if (!bmp) EXCEPTION("SDL_LoadBMP: %s", SDL_GetError());

		// convert to texture
		SDL_Texture* texture = SDL_CreateTextureFromSurface(info.renderer, bmp);
		if (!texture) EXCEPTION("SDL_CreateTextureFromSurface: %s", SDL_GetError());
		SDL_FreeSurface(bmp);

		// clear screen
		SDL_SetRenderDrawColor(info.renderer, 0, 0, 0, 255);
		SDL_RenderClear(info.renderer);

		// draw texture
		SDL_RenderCopy(info.renderer, texture, NULL, NULL);

		// update
		SDL_RenderPresent(info.renderer);

		// free texture
		SDL_DestroyTexture(texture);
	}

	SDL_WaitThread(eventThread, &eventThreadReturn);

	// hide window
	SDL_HideWindow(window);

	// IMG
	IMG_Quit();

	// renderer
	info.renderer = NULL;

	// window
	SDL_DestroyWindow(window);
	window = NULL;

	// sdl
	SDL_Quit();

	return 0;
}
