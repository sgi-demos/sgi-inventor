//
// SoSDLRenderArea: an SDL2 window with a GL context driving an Inventor
// SoSceneManager. The SDL equivalent of SoXtRenderArea.
//
#ifndef _SO_SDL_RENDER_AREA_
#define _SO_SDL_RENDER_AREA_

#include <SDL2/SDL.h>
#include <Inventor/SbColor.h>
#include <Inventor/SbLinear.h>

class SoNode;
class SoSceneManager;
class SoEvent;

class SoSDLRenderArea {
  public:
    SoSDLRenderArea(const char *title = "Open Inventor",
		    int width = 800, int height = 600);
    virtual ~SoSDLRenderArea();

    void		setSceneGraph(SoNode *root);
    SoNode		*getSceneGraph() const;

    void		setBackgroundColor(const SbColor &c);
    SbVec2s		getSize() const { return size; }
    SDL_Window		*getWindow() const { return window; }
    SDL_GLContext	getContext() const { return context; }
    Uint32		getWindowId() const;

    // Schedule (lazy) or force (immediate) a redraw.
    void		scheduleRedraw()	{ redrawNeeded = true; }
    virtual void	render();

    // Returns true when a scheduled redraw is pending.
    bool		isRedrawNeeded() const	{ return redrawNeeded; }

    // Handle one SDL event destined for this window. Translates it to
    // an SoEvent and applies the scene manager's handle-event action.
    // Virtual so viewers can intercept events before the scene sees them.
    virtual void	processEvent(const SDL_Event *event);

  protected:
    // Translate an SDL event to an Inventor event in this render area's
    // coordinate space (returns NULL if the event has no translation).
    // The returned event is owned by the render area; valid until the
    // next translateEvent call.
    const SoEvent	*translateEvent(const SDL_Event *event);

    // Send a translated event to the scene graph; returns whether some
    // node handled it.
    bool		sendEventToScene(const SoEvent *ev);

    SoSceneManager	*sceneMgr;
    SbVec2s		size;

  private:
    SDL_Window		*window;
    SDL_GLContext	context;
    bool		redrawNeeded;

    static void		renderCB(void *userData, SoSceneManager *mgr);
};

#endif /* _SO_SDL_RENDER_AREA_ */
