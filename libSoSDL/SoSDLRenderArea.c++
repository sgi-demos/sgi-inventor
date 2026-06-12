#include "SoSDLRenderArea.h"
#include "SoSDL.h"

#include <GL/gl.h>
#include <Inventor/SoSceneManager.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoWindowElement.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/nodes/SoNode.h>

#include <stdio.h>
#include <stdlib.h>

// Translation scratch events, reused across calls (as SoXt did).
static SoMouseButtonEvent	*buttonEvent = NULL;
static SoLocation2Event		*locationEvent = NULL;
static SoKeyboardEvent		*keyboardEvent = NULL;

SoSDLRenderArea::SoSDLRenderArea(const char *title, int width, int height)
{
    size.setValue(width, height);
    redrawNeeded = false;
    autoRedraw = TRUE;
    overlayMgr = NULL;
    closeCB = NULL;
    closeCBData = NULL;

    window = SDL_CreateWindow(title,
	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
	fprintf(stderr, "SoSDLRenderArea — window: %s\n", SDL_GetError());
	context = NULL;
	sceneMgr = NULL;
	return;
    }
    context = SDL_GL_CreateContext(window);
    if (!context) {
	fprintf(stderr, "SoSDLRenderArea — context: %s\n", SDL_GetError());
	sceneMgr = NULL;
	return;
    }

    // SoXtRenderArea enabled depth testing when realizing its GL widget;
    // as the GL surface owner, that responsibility is ours now.
    glEnable(GL_DEPTH_TEST);

    sceneMgr = new SoSceneManager;
    sceneMgr->setWindowSize(size);
    sceneMgr->setRenderCallback(renderCB, this);
    sceneMgr->activate();

    SoSDL::addRenderArea(this);
}

SoSDLRenderArea::~SoSDLRenderArea()
{
    SoSDL::removeRenderArea(this);
    delete sceneMgr;
    if (context) SDL_GL_DeleteContext(context);
    if (window) SDL_DestroyWindow(window);
}

Uint32
SoSDLRenderArea::getWindowId() const
{
    return window ? SDL_GetWindowID(window) : 0;
}

void
SoSDLRenderArea::setOverlaySceneGraph(SoNode *root)
{
    if (!overlayMgr) {
	overlayMgr = new SoSceneManager;
	overlayMgr->setWindowSize(size);
	overlayMgr->setRenderCallback(renderCB, this);
	overlayMgr->activate();
    }
    overlayMgr->setSceneGraph(root);
    scheduleRedraw();
}

SoNode *
SoSDLRenderArea::getOverlaySceneGraph() const
{
    return overlayMgr ? overlayMgr->getSceneGraph() : NULL;
}

void
SoSDLRenderArea::setAutoRedraw(SbBool flag)
{
    // With autoRedraw off, scene changes do not schedule redraws; the
    // application drives rendering (slotcar uses this for its one-shot
    // polygon-timing benchmark scene).
    autoRedraw = flag;
    if (sceneMgr) {
	if (flag) sceneMgr->activate(); else sceneMgr->deactivate();
    }
    if (flag)
	scheduleRedraw();
}

void
SoSDLRenderArea::setWindowCloseCallback(
    void (*cb)(void *, SoSDLRenderArea *), void *userData)
{
    closeCB = cb;
    closeCBData = userData;
}

void
SoSDLRenderArea::renderCB(void *userData, SoSceneManager *)
{
    // The scene changed; render lazily on the next loop iteration.
    ((SoSDLRenderArea *)userData)->scheduleRedraw();
}

void
SoSDLRenderArea::setSceneGraph(SoNode *root)
{
    sceneMgr->setSceneGraph(root);
    scheduleRedraw();
}

SoNode *
SoSDLRenderArea::getSceneGraph() const
{
    return sceneMgr->getSceneGraph();
}

void
SoSDLRenderArea::setBackgroundColor(const SbColor &c)
{
    sceneMgr->setBackgroundColor(c);
    scheduleRedraw();
}

void
SoSDLRenderArea::render()
{
    if (!window || !context)
	return;
    SDL_GL_MakeCurrent(window, context);

    // Make the window/context available to nodes that need it (e.g.
    // SoLocateHighlight's front-buffer redraw).
    // Scene callbacks may leave the GL viewport altered (e.g. slotcar's
    // Inventor-logo callback sets an 80x80 viewport and relies on the
    // windowing layer to restore it). Reset it for every pass.
    glViewport(0, 0, size[0], size[1]);
    sceneMgr->getGLRenderAction()->setUpdateArea(SbVec2f(0, 0), SbVec2f(1, 1));
    sceneMgr->render();

    // Overlay-plane emulation: render the overlay graph over the main
    // scene with a fresh depth buffer but without clearing color.
    if (overlayMgr) {
	glViewport(0, 0, size[0], size[1]);
	overlayMgr->render(FALSE, TRUE);	// clearWindow=no, clearZ=yes
    }

    // Debug aid for headless testing: IV_DUMP_DIR=<dir> writes each
    // rendered frame (back buffer) as a PPM before it is swapped.
    static const char *dumpDir = getenv("IV_DUMP_DIR");
    if (dumpDir) {
	static int frameNo = 0;
	char path[512];
	snprintf(path, sizeof path, "%s/frame-%04d.ppm", dumpDir, frameNo++);
	int w = size[0], h = size[1];
	unsigned char *pix = new unsigned char[w * h * 3];
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pix);
	FILE *f = fopen(path, "wb");
	if (f) {
	    fprintf(f, "P6\n%d %d\n255\n", w, h);
	    for (int y = h - 1; y >= 0; y--)
		fwrite(pix + y * w * 3, 1, w * 3, f);
	    fclose(f);
	}
	delete [] pix;
    }

    SDL_GL_SwapWindow(window);
    redrawNeeded = false;
}

//
// SDL -> SoEvent translation.
//
// Inventor events use y-up window coordinates; SDL reports y-down.
//

static void
setModifiers(SoEvent *ev)
{
    SDL_Keymod m = SDL_GetModState();
    ev->setShiftDown((m & KMOD_SHIFT) != 0);
    ev->setCtrlDown((m & KMOD_CTRL) != 0);
    ev->setAltDown((m & KMOD_ALT) != 0);
    ev->setTime(SbTime::getTimeOfDay());
}

static SoKeyboardEvent::Key
translateKey(SDL_Keycode k)
{
    // Letters and digits map contiguously in both enums.
    if (k >= SDLK_a && k <= SDLK_z)
	return (SoKeyboardEvent::Key)(SoKeyboardEvent::A + (k - SDLK_a));
    if (k >= SDLK_0 && k <= SDLK_9)
	return (SoKeyboardEvent::Key)(SoKeyboardEvent::NUMBER_0 + (k - SDLK_0));
    switch (k) {
      case SDLK_ESCAPE:	return SoKeyboardEvent::ESCAPE;
      case SDLK_RETURN:	return SoKeyboardEvent::RETURN;
      case SDLK_SPACE:	return SoKeyboardEvent::SPACE;
      case SDLK_BACKSPACE: return SoKeyboardEvent::BACKSPACE;
      case SDLK_TAB:	return SoKeyboardEvent::TAB;
      case SDLK_LEFT:	return SoKeyboardEvent::LEFT_ARROW;
      case SDLK_RIGHT:	return SoKeyboardEvent::RIGHT_ARROW;
      case SDLK_UP:	return SoKeyboardEvent::UP_ARROW;
      case SDLK_DOWN:	return SoKeyboardEvent::DOWN_ARROW;
      case SDLK_HOME:	return SoKeyboardEvent::HOME;
      case SDLK_END:	return SoKeyboardEvent::END;
      case SDLK_PAGEUP:	return SoKeyboardEvent::PAGE_UP;
      case SDLK_PAGEDOWN: return SoKeyboardEvent::PAGE_DOWN;
      case SDLK_LSHIFT:	return SoKeyboardEvent::LEFT_SHIFT;
      case SDLK_RSHIFT:	return SoKeyboardEvent::RIGHT_SHIFT;
      case SDLK_LCTRL:	return SoKeyboardEvent::LEFT_CONTROL;
      case SDLK_RCTRL:	return SoKeyboardEvent::RIGHT_CONTROL;
      case SDLK_LALT:	return SoKeyboardEvent::LEFT_ALT;
      case SDLK_RALT:	return SoKeyboardEvent::RIGHT_ALT;
      case SDLK_DELETE:	return SoKeyboardEvent::DELETE;
      default:		return SoKeyboardEvent::ANY;
    }
}

const SoEvent *
SoSDLRenderArea::translateEvent(const SDL_Event *e)
{
    switch (e->type) {
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP: {
	if (!buttonEvent) buttonEvent = new SoMouseButtonEvent;
	SoMouseButtonEvent::Button b;
	switch (e->button.button) {
	  case SDL_BUTTON_LEFT:		b = SoMouseButtonEvent::BUTTON1; break;
	  case SDL_BUTTON_MIDDLE:	b = SoMouseButtonEvent::BUTTON2; break;
	  case SDL_BUTTON_RIGHT:	b = SoMouseButtonEvent::BUTTON3; break;
	  default:			return NULL;
	}
	buttonEvent->setButton(b);
	buttonEvent->setState(e->type == SDL_MOUSEBUTTONDOWN ?
	    SoButtonEvent::DOWN : SoButtonEvent::UP);
	buttonEvent->setPosition(
	    SbVec2s(e->button.x, size[1] - 1 - e->button.y));
	setModifiers(buttonEvent);
	return buttonEvent;
      }
      case SDL_MOUSEMOTION: {
	if (!locationEvent) locationEvent = new SoLocation2Event;
	locationEvent->setPosition(
	    SbVec2s(e->motion.x, size[1] - 1 - e->motion.y));
	setModifiers(locationEvent);
	return locationEvent;
      }
      case SDL_KEYDOWN:
      case SDL_KEYUP: {
	if (!keyboardEvent) keyboardEvent = new SoKeyboardEvent;
	keyboardEvent->setKey(translateKey(e->key.keysym.sym));
	keyboardEvent->setState(e->type == SDL_KEYDOWN ?
	    SoButtonEvent::DOWN : SoButtonEvent::UP);
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	keyboardEvent->setPosition(SbVec2s(mx, size[1] - 1 - my));
	setModifiers(keyboardEvent);
	return keyboardEvent;
      }
    }
    return NULL;
}

bool
SoSDLRenderArea::sendEventToScene(const SoEvent *ev)
{
    if (!ev)
	return false;
    return sceneMgr->processEvent(ev) ? true : false;
}

void
SoSDLRenderArea::processEvent(const SDL_Event *e)
{
    if (e->type == SDL_WINDOWEVENT) {
	switch (e->window.event) {
	  case SDL_WINDOWEVENT_SIZE_CHANGED:
	    size.setValue(e->window.data1, e->window.data2);
	    sceneMgr->setWindowSize(size);
	    if (overlayMgr) overlayMgr->setWindowSize(size);
	    scheduleRedraw();
	    break;
	  case SDL_WINDOWEVENT_EXPOSED:
	    scheduleRedraw();
	    break;
	  case SDL_WINDOWEVENT_CLOSE:
	    if (closeCB)
		closeCB(closeCBData, this);
	    else
		SoSDL::exitMainLoop();
	    break;
	}
	return;
    }
    sendEventToScene(translateEvent(e));
}
