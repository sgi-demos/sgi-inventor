//
// SoSDLRenderArea: an SDL2 window with a GL context driving an Inventor
// SoSceneManager. The SDL equivalent of SoXtRenderArea.
//
#ifndef _SO_SDL_RENDER_AREA_
#define _SO_SDL_RENDER_AREA_

#include <SDL2/SDL.h>
#include <Inventor/SbColor.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbViewportRegion.h>

class SoNode;
class SoSceneManager;
class SoEvent;
class SoGLRenderAction;

class SoSDLRenderArea {
  public:
    SoSDLRenderArea(const char *title = "Open Inventor",
		    int width = 800, int height = 600);
    virtual ~SoSDLRenderArea();

    void		setSceneGraph(SoNode *root);
    SoNode		*getSceneGraph() const;

    void		setBackgroundColor(const SbColor &c);
    // Forwarded to the scene manager's GL render action.
    void		setTransparencyType(int type);
    SoGLRenderAction	*getGLRenderAction() const;
    // Window title (SoXtComponent::setTitle semantics).
    void		setTitle(const char *title);
    SbVec2s		getSize() const { return size; }
    // Resize the window (SoXtComponent::setSize semantics).
    void		setSize(const SbVec2s &newSize);
    SbViewportRegion	getViewportRegion() const
					{ return SbViewportRegion(size); }

    // Overlay-plane emulation: the overlay scene graph is rendered as a
    // second pass over the main scene (depth buffer cleared, color kept).
    // SGI hardware drew this in separate overlay planes; the visual
    // result here is equivalent for HUD-style content.
    void		setOverlaySceneGraph(SoNode *root);
    SoNode		*getOverlaySceneGraph() const;
    // Overlay color maps applied to SoColorIndex content have no meaning
    // in RGBA rendering; accepted and ignored for source compatibility.
    void		setOverlayColorMap(int, int, const SbColor *) {}
    // The overlay is drawn as part of every render pass; an overlay
    // redraw request is just a redraw request.
    void		renderOverlay()	{ scheduleRedraw(); }

    // When autoRedraw is on (default), scene-graph changes schedule a
    // redraw automatically (SoXtRenderArea semantics).
    void		setAutoRedraw(SbBool flag);
    SbBool		isAutoRedraw() const	{ return autoRedraw; }

    // When off, the color buffer is not cleared before rendering (the
    // scene is expected to cover the window). SoXtRenderArea semantics.
    void		setClearBeforeRender(SbBool flag)
					{ clearBeforeRender = flag; }
    SbBool		isClearBeforeRender() const
					{ return clearBeforeRender; }

    // Called when the user closes the window (SDL_QUIT / window close).
    // If unset, the main loop simply exits.
    void		setWindowCloseCallback(
				void (*cb)(void *userData, SoSDLRenderArea *),
				void *userData = NULL);
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
    SbBool		autoRedraw;
    SbBool		clearBeforeRender;
    SoSceneManager	*overlayMgr;	// NULL until an overlay graph is set
    void		(*closeCB)(void *, SoSDLRenderArea *);
    void		*closeCBData;

    static void		renderCB(void *userData, SoSceneManager *mgr);
};

#endif /* _SO_SDL_RENDER_AREA_ */
