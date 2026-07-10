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

  private:
    enum DragMode { IDLE, SPIN, PAN, DOLLY };

    void		startDrag(DragMode mode, const SbVec2s &pos);
    void		drag(const SbVec2s &pos);
    void		endDrag();
    SbVec2f		normalize(const SbVec2s &pos) const;

    SoSeparator		*viewerRoot;	// camera + headlight + user scene
    SoCamera		*camera;
    bool		cameraIsMine;
    SoDirectionalLight	*headlight;
    SoGroup		*userSceneHolder;

    bool		viewing;
    DragMode		mode;
    SbVec2s		locator;	// last mouse pos, y-up
    SbSphereSheetProjector *sphereSheet;
    SbPlane		focalplane;
    SbVec3f		locator3D;

    void		updateHeadlight();
};

#endif /* _SO_SDL_EXAMINER_VIEWER_ */
