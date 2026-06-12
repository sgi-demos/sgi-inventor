//
// SoSDL: SDL2 binding for SGI Open Inventor.
//
// Replaces the SoXt layer: window/context management, event translation,
// and Inventor sensor-queue servicing, with no X11/Motif dependency.
//
#ifndef _SO_SDL_
#define _SO_SDL_

#include <SDL2/SDL.h>

class SoSDLRenderArea;

class SoSDL {
  public:
    // Initializes SDL video and the Inventor database (SoDB, SoNodeKit,
    // SoInteraction). Call once before creating render areas.
    static void		init();

    // Runs until exitMainLoop() or all windows are closed. Dispatches
    // SDL events to render areas and services Inventor's timer and
    // delay sensor queues, mirroring the SoXt main loop behavior.
    static void		mainLoop();
    static void		exitMainLoop();

    // Process a single iteration of the loop (events + sensors +
    // redraws). Returns FALSE when the loop should exit. Useful for
    // callers that own the loop (e.g. emscripten_set_main_loop).
    static bool		doOneIteration();

  public:  // internal
    static void		addRenderArea(SoSDLRenderArea *ra);
    static void		removeRenderArea(SoSDLRenderArea *ra);

  private:
    static bool		mainLoopDone;
};

#endif /* _SO_SDL_ */
