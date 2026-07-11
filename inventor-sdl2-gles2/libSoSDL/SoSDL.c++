#include "SoSDL.h"
#include "SoSDLRenderArea.h"

#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/sensors/SoSensorManager.h>
#include <Inventor/SbTime.h>

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

bool SoSDL::mainLoopDone = false;
static std::vector<SoSDLRenderArea *> renderAreas;

void
SoSDL::init()
{
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
	    fprintf(stderr, "SoSDL::init — SDL_Init failed: %s\n",
		    SDL_GetError());
	    return;
	}
    }
    // GL surface attributes must be set before window creation.
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef __EMSCRIPTEN__
    // Inventor's render caching (GL display lists) works under gl4es
    // natively, but on WebGL the cached geometry replays black (maze
    // floor/walls lose their material; gl4es display-list issue, not
    // yet root-caused). Disable caching before SoDB::init on the web.
    setenv("IV_SEPARATOR_MAX_CACHES", "0", 1);
#endif
    SoDB::init();
    SoNodeKit::init();
    SoInteraction::init();
}

void
SoSDL::addRenderArea(SoSDLRenderArea *ra)
{
    renderAreas.push_back(ra);
}

void
SoSDL::removeRenderArea(SoSDLRenderArea *ra)
{
    for (size_t i = 0; i < renderAreas.size(); i++)
	if (renderAreas[i] == ra) {
	    renderAreas.erase(renderAreas.begin() + i);
	    break;
	}
}

static SoSDLRenderArea *
findRenderArea(Uint32 windowId)
{
    for (size_t i = 0; i < renderAreas.size(); i++)
	if (renderAreas[i]->getWindowId() == windowId)
	    return renderAreas[i];
    return NULL;
}

static Uint32
eventWindowId(const SDL_Event *e)
{
    switch (e->type) {
      case SDL_MOUSEMOTION:	return e->motion.windowID;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:	return e->button.windowID;
      case SDL_MOUSEWHEEL:	return e->wheel.windowID;
      case SDL_KEYDOWN:
      case SDL_KEYUP:		return e->key.windowID;
      case SDL_WINDOWEVENT:	return e->window.windowID;
      default:			return 0;
    }
}

bool
SoSDL::doOneIteration()
{
    if (mainLoopDone || renderAreas.empty())
	return false;

    SoSensorManager *sm = SoDB::getSensorManager();

    // Compute how long we may sleep in SDL_WaitEventTimeout: until the
    // next Inventor timer fires, or a short tick if a delay-queue
    // sensor is pending (idle work), mirroring SoXt's select() timeout.
    int timeoutMs = 10;
    SbTime nextTimer;
    if (sm->isTimerSensorPending(nextTimer)) {
	SbTime now = SbTime::getTimeOfDay();
	double ms = (nextTimer - now).getValue() * 1000.0;
	if (ms < 0.0) ms = 0.0;
	if (ms < timeoutMs) timeoutMs = (int)ms;
    }

    SDL_Event event;
#ifdef __EMSCRIPTEN__
    // The browser paces the loop; never block in the event wait.
    (void)timeoutMs;
    if (SDL_PollEvent(&event)) {
	do {
#else
    if (SDL_WaitEventTimeout(&event, timeoutMs)) {
	do {
#endif
	    if (event.type == SDL_QUIT) {
		mainLoopDone = true;
		return false;
	    }
	    SoSDLRenderArea *ra = findRenderArea(eventWindowId(&event));
	    if (ra)
		ra->processEvent(&event);
	} while (SDL_PollEvent(&event));
    }

    // Service Inventor sensor queues: timers first, then idle (delay)
    // sensors. This is what makes engines, SoRotor, scheduled redraws,
    // and animation work.
    sm->processTimerQueue();
    sm->processDelayQueue(TRUE);	// TRUE: we are idle

    // Perform any scheduled redraws.
    for (size_t i = 0; i < renderAreas.size(); i++)
	if (renderAreas[i]->isRedrawNeeded())
	    renderAreas[i]->render();

    return !mainLoopDone;
}

#ifdef __EMSCRIPTEN__
static void
emIteration()
{
    if (!SoSDL::doOneIteration())
	emscripten_cancel_main_loop();
}
#endif

void
SoSDL::mainLoop()
{
    mainLoopDone = false;
#ifdef __EMSCRIPTEN__
    // Hand the loop to the browser; fps=0 uses requestAnimationFrame.
    emscripten_set_main_loop(emIteration, 0, 1);
#else
    while (doOneIteration())
	;
#endif
}

void
SoSDL::exitMainLoop()
{
    mainLoopDone = true;
}
