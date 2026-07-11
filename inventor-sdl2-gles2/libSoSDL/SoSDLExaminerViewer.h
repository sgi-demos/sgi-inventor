//
// SoSDLExaminerViewer: trackball-style viewer, the SDL equivalent of
// SoXtExaminerViewer. Camera manipulation math is ported directly from
// SoXtExamVwr.c++ (sphere-sheet spin, focal-plane pan, focal dolly).
//
// Interaction (viewing mode, the default):
//   left drag            spin (trackball)
//   middle drag or
//     shift + left drag  pan
//   ctrl + left drag or
//     mouse wheel        dolly in/out
//   ESC                  toggle viewing <-> picking mode
//   's' then click       seek to point
//   'h'                  view all (home)
// In picking mode all events go to the scene graph.
//
#ifndef _SO_SDL_EXAMINER_VIEWER_
#define _SO_SDL_EXAMINER_VIEWER_

#include "SoSDLRenderArea.h"
#include <Inventor/SbLinear.h>
#include <vector>


class SoCamera;
class SoDirectionalLight;
class SoGroup;
class SoSeparator;
class SbSphereSheetProjector;

class SoSDLExaminerViewer : public SoSDLRenderArea {
  public:
    SoSDLExaminerViewer(const char *title = "Examiner Viewer",
			int width = 800, int height = 600);
    virtual ~SoSDLExaminerViewer();

    // Sets the user scene graph. The viewer adds its own camera (if the
    // scene lacks one) and a headlight above it.
    void		setSceneGraph(SoNode *root);

    SoCamera		*getCamera() const	{ return camera; }
    void		viewAll();

    // Turn the built-in headlight on/off (apps that light their own
    // scenes turn it off).
    void		setHeadlight(SbBool on);

    bool		isViewing() const	{ return viewing; }
    void		setViewing(bool on)	{ viewing = on; }

    virtual void	processEvent(const SDL_Event *event);

    // Recompute camera near/far from the scene bounding box before
    // rendering, as SoXtViewer::adjustCameraClippingPlanes did.
    virtual void	render();

    // Direct camera operations (also used by tests).
    void		rotateCamera(const SbRotation &rot);
    void		panCamera(const SbVec2f &newLocatorNorm);
    void		dollyCamera(float amount);
    void		spinCamera(const SbVec2f &newLocatorNorm);

    // The headlight grafted above the user scene.
    SoDirectionalLight	*getHeadlight() const	{ return headlight; }

    // When off, near/far are not recomputed each render (SoXtViewer's
    // setAutoClipping semantics; scenes with fixed cameras use this).
    void		setAutoClipping(SbBool on)	{ autoClipping = on; }

    // Save/restore the camera's home position (SoXtViewer semantics).
    void		saveHomePosition();
    void		resetToHomePosition();

    // Finish callbacks, invoked when a viewer gesture completes
    // (SoXtViewer::addFinishCallback semantics; the TabBox manip uses
    // one to adjust its scale tabs after the camera moves).
    typedef void	FinishCB(void *userData, SoSDLExaminerViewer *viewer);
    void		addFinishCallback(FinishCB *cb, void *userData = NULL);
    void		removeFinishCallback(FinishCB *cb, void *userData = NULL);

  protected:
    // Drag machinery, available to viewer subclasses with their own
    // input bindings (e.g. drop's TsViewer).
    enum DragMode { IDLE, SPIN, PAN, DOLLY };

    void		startDrag(DragMode mode, const SbVec2s &pos);
    void		drag(const SbVec2s &pos);
    void		endDrag();
    SbVec2f		normalize(const SbVec2s &pos) const;

  private:
    SoSeparator		*viewerRoot;	// camera + headlight + user scene
    SoCamera		*camera;
    bool		cameraIsMine;
    SoDirectionalLight	*headlight;
    SoGroup		*userSceneHolder;

    bool		viewing;
    SbBool		autoClipping;
    // saved home position (saveHomePosition/resetToHomePosition)
    SbVec3f		homePos;
    SbRotation		homeOrient;
    float		homeFocal, homeHeight;
    SbBool		homeSaved;
    DragMode		mode;
    struct FinishCBEntry { FinishCB *cb; void *userData; };
    std::vector<FinishCBEntry> finishCBs;
    void		invokeFinishCallbacks();
    SbVec2s		locator;	// last mouse pos, y-up
    SbSphereSheetProjector *sphereSheet;
    SbPlane		focalplane;
    SbVec3f		locator3D;

    void		updateHeadlight();
};

#endif /* _SO_SDL_EXAMINER_VIEWER_ */
