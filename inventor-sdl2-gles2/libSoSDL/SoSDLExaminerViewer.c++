#include "SoSDLExaminerViewer.h"

#include <Inventor/SoDB.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/projectors/SbSphereSheetProjector.h>
#include <Inventor/SbLinear.h>


#include <math.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

// Ported from SoXtViewer::adjustCameraClippingPlanes: fit the camera
// near/far planes to the scene each frame so nothing gets clipped.
static void
adjustCameraClippingPlanes(SoCamera *camera, SoNode *sceneRoot,
			   const SbVec2s &size)
{
    static const float minimumNearPlane = 0.001f;	// SoXt default

    if (camera == NULL || sceneRoot == NULL)
	return;

    static SoGetBoundingBoxAction *bboxAction = NULL;
    if (!bboxAction)
	bboxAction = new SoGetBoundingBoxAction(SbViewportRegion(size));
    bboxAction->setViewportRegion(SbViewportRegion(size));
    bboxAction->apply(sceneRoot);
    SbXfBox3f xfbbox = bboxAction->getXfBoundingBox();

    // Align the bbox to camera space; its z extent gives near/far.
    SbMatrix mx;
    mx.setTranslate(-camera->position.getValue());
    xfbbox.transform(mx);
    mx = camera->orientation.getValue().inverse();
    xfbbox.transform(mx);

    SbBox3f bbox = xfbbox.project();
    // Negated: the camera looks down -Z.
    float farD  = -bbox.getMin()[2];
    float nearD = -bbox.getMax()[2];

    if (farD < 0)		// scene entirely behind the camera
	return;

    if (!camera->isOfType(SoOrthographicCamera::getClassTypeId())) {
	if (nearD < (minimumNearPlane * farD))
	    nearD = minimumNearPlane * farD;
    }

    // Slack so objects flush with their bbox don't pop.
    nearD *= 0.999f;
    farD  *= 1.001f;

    if (camera->nearDistance.getValue() != nearD)
	camera->nearDistance = nearD;
    if (camera->farDistance.getValue() != farD)
	camera->farDistance = farD;
}

SoSDLExaminerViewer::SoSDLExaminerViewer(const char *title, int w, int h)
	: SoSDLRenderArea(title, w, h)
{
    camera = NULL;
    cameraIsMine = false;
    autoClipping = TRUE;
    homeSaved = FALSE;
    viewing = true;
    mode = IDLE;

    // Same projector setup as SoXtExamVwr.
    sphereSheet = new SbSphereSheetProjector;
    SbViewVolume vv;
    vv.ortho(-1, 1, -1, 1, -10, 10);
    sphereSheet->setViewVolume(vv);
    sphereSheet->setSphere(SbSphere(SbVec3f(0, 0, 0), .7f));

    viewerRoot = new SoSeparator;
    viewerRoot->ref();
    headlight = new SoDirectionalLight;
    userSceneHolder = new SoGroup;
}

SoSDLExaminerViewer::~SoSDLExaminerViewer()
{
    viewerRoot->unref();
    delete sphereSheet;
}

void
SoSDLExaminerViewer::setSceneGraph(SoNode *root)
{
    viewerRoot->removeAllChildren();
    userSceneHolder->removeAllChildren();
    if (root)
	userSceneHolder->addChild(root);

    // Use the scene's own camera if it has one, else make ours.
    camera = NULL;
    cameraIsMine = false;
    if (root) {
	SoSearchAction sa;
	sa.setType(SoCamera::getClassTypeId());
	sa.setInterest(SoSearchAction::FIRST);
	sa.apply(root);
	if (sa.getPath())
	    camera = (SoCamera *)sa.getPath()->getTail();
    }
    if (!camera) {
	camera = new SoPerspectiveCamera;
	cameraIsMine = true;
	viewerRoot->addChild(camera);
	// Headlight must follow the camera in traversal order so it is
	// transformed by the viewing matrix (shines from the viewer).
	viewerRoot->addChild(headlight);
	viewerRoot->addChild(userSceneHolder);
    }
    else {
	// The scene owns the camera: graft the headlight in right after
	// it, as SoXt viewers did, so it still tracks the viewpoint.
	SoSearchAction sa;
	sa.setNode(camera);
	sa.apply(root);
	SoPath *p = sa.getPath();
	if (p && p->getLength() >= 2) {
	    SoGroup *parent = (SoGroup *)p->getNodeFromTail(1);
	    parent->insertChild(headlight, parent->findChild(camera) + 1);
	}
	viewerRoot->addChild(userSceneHolder);
    }

    SoSDLRenderArea::setSceneGraph(viewerRoot);
    if (cameraIsMine)
	viewAll();
    updateHeadlight();
}

void
SoSDLExaminerViewer::saveHomePosition()
{
    if (!camera) return;
    homePos    = camera->position.getValue();
    homeOrient = camera->orientation.getValue();
    homeFocal  = camera->focalDistance.getValue();
    homeHeight = camera->isOfType(SoPerspectiveCamera::getClassTypeId())
	? ((SoPerspectiveCamera *)camera)->heightAngle.getValue()
	: ((SoOrthographicCamera *)camera)->height.getValue();
    homeSaved = TRUE;
}

void
SoSDLExaminerViewer::resetToHomePosition()
{
    if (!camera || !homeSaved) return;
    camera->position = homePos;
    camera->orientation = homeOrient;
    camera->focalDistance = homeFocal;
    if (camera->isOfType(SoPerspectiveCamera::getClassTypeId()))
	((SoPerspectiveCamera *)camera)->heightAngle = homeHeight;
    else
	((SoOrthographicCamera *)camera)->height = homeHeight;
    scheduleRedraw();
}

void
SoSDLExaminerViewer::render()
{
    if (autoClipping)
	adjustCameraClippingPlanes(camera, viewerRoot, getSize());
    SoSDLRenderArea::render();
}

void
SoSDLExaminerViewer::setHeadlight(SbBool on)
{
    if (headlight)
	headlight->on = on;
    scheduleRedraw();
}

void
SoSDLExaminerViewer::viewAll()
{
    if (camera)
	camera->viewAll(userSceneHolder, SbViewportRegion(getSize()));
    scheduleRedraw();
}

void
SoSDLExaminerViewer::updateHeadlight()
{
    if (!camera)
	return;
    // Aim the headlight along the camera's view direction, slightly
    // from upper-left, like SoXtFullViewer's headlight default.
    SbMatrix mx;
    mx = camera->orientation.getValue();
    SbVec3f forward(-mx[2][0], -mx[2][1], -mx[2][2]);
    headlight->direction = forward;
}

SbVec2f
SoSDLExaminerViewer::normalize(const SbVec2s &pos) const
{
    SbVec2s sz = getSize();
    return SbVec2f(pos[0] / float(sz[0]), pos[1] / float(sz[1]));
}

//
// Camera operations — ported from SoXtExamVwr.c++.
//

void
SoSDLExaminerViewer::rotateCamera(const SbRotation &rot)
{
    if (camera == NULL)
	return;

    // Rotate about the focal point.
    SbRotation camRot = camera->orientation.getValue();
    float radius = camera->focalDistance.getValue();
    SbMatrix mx;
    mx = camRot;
    SbVec3f forward(-mx[2][0], -mx[2][1], -mx[2][2]);
    SbVec3f center = camera->position.getValue() + radius * forward;

    camRot = rot * camRot;
    camera->orientation = camRot;

    mx = camRot;
    forward.setValue(-mx[2][0], -mx[2][1], -mx[2][2]);
    camera->position = center - radius * forward;

    updateHeadlight();
}

void
SoSDLExaminerViewer::spinCamera(const SbVec2f &newLocator)
{
    SbRotation rot;
    sphereSheet->projectAndGetRotation(newLocator, rot);
    rot.invert();
    rotateCamera(rot);
}

void
SoSDLExaminerViewer::panCamera(const SbVec2f &newLocator)
{
    if (camera == NULL)
	return;

    // Map the new mouse location into the camera focal plane and move
    // the camera by the 3D delta.
    SbViewVolume cameraVolume;
    SbLine line;
    SbVec3f newLocator3D;
    SbVec2s sz = getSize();
    cameraVolume = camera->getViewVolume(sz[0] / float(sz[1]));
    cameraVolume.projectPointToLine(newLocator, line);
    focalplane.intersect(line, newLocator3D);

    camera->position = camera->position.getValue() +
	(locator3D - newLocator3D);
    // (locator3D is intentionally not updated; see SoXtExamVwr comment.)
}

void
SoSDLExaminerViewer::dollyCamera(float d)
{
    if (camera == NULL)
	return;

    if (camera->isOfType(SoOrthographicCamera::getClassTypeId())) {
	SoOrthographicCamera *cam = (SoOrthographicCamera *)camera;
	cam->height = cam->height.getValue() * powf(2.0f, d);
    }
    else {
	float focalDistance = camera->focalDistance.getValue();
	float newFocalDist = focalDistance * powf(2.0f, d);

	SbMatrix mx;
	mx = camera->orientation.getValue();
	SbVec3f forward(-mx[2][0], -mx[2][1], -mx[2][2]);
	camera->position = camera->position.getValue() +
	    (focalDistance - newFocalDist) * forward;
	camera->focalDistance = newFocalDist;
    }
}

//
// Drag-state machine.
//

void
SoSDLExaminerViewer::startDrag(DragMode m, const SbVec2s &pos)
{
    mode = m;
    locator = pos;

    if (mode == SPIN) {
	sphereSheet->project(normalize(pos));
    }
    else if (mode == PAN && camera) {
	// Establish the focal plane and the drag's 3D anchor point.
	SbMatrix mx;
	mx = camera->orientation.getValue();
	SbVec3f forward(-mx[2][0], -mx[2][1], -mx[2][2]);
	SbVec3f fp = camera->position.getValue() +
	    forward * camera->focalDistance.getValue();
	focalplane = SbPlane(forward, fp);

	SbViewVolume cameraVolume;
	SbLine line;
	SbVec2s sz = getSize();
	cameraVolume = camera->getViewVolume(sz[0] / float(sz[1]));
	cameraVolume.projectPointToLine(normalize(pos), line);
	focalplane.intersect(line, locator3D);
    }
}

void
SoSDLExaminerViewer::drag(const SbVec2s &pos)
{
    switch (mode) {
      case SPIN:
	spinCamera(normalize(pos));
	break;
      case PAN:
	panCamera(normalize(pos));
	break;
      case DOLLY:
	// Vertical motion dollies, as in SoXt (divisor 40 in pixels).
	dollyCamera((pos[1] - locator[1]) / 40.0f);
	break;
      default:
	return;
    }
    locator = pos;
    scheduleRedraw();
}

void
SoSDLExaminerViewer::endDrag()
{
    if (mode != IDLE)
	invokeFinishCallbacks();
    mode = IDLE;
}

void
SoSDLExaminerViewer::addFinishCallback(FinishCB *cb, void *userData)
{
    FinishCBEntry e;
    e.cb = cb;
    e.userData = userData;
    finishCBs.push_back(e);
}

void
SoSDLExaminerViewer::removeFinishCallback(FinishCB *cb, void *userData)
{
    for (size_t i = 0; i < finishCBs.size(); i++)
	if (finishCBs[i].cb == cb && finishCBs[i].userData == userData) {
	    finishCBs.erase(finishCBs.begin() + i);
	    return;
	}
}

void
SoSDLExaminerViewer::invokeFinishCallbacks()
{
    for (size_t i = 0; i < finishCBs.size(); i++)
	finishCBs[i].cb(finishCBs[i].userData, this);
}

void
SoSDLExaminerViewer::processEvent(const SDL_Event *e)
{
    // Window management always handled by the base class.
    if (e->type == SDL_WINDOWEVENT) {
	SoSDLRenderArea::processEvent(e);
	return;
    }

    // ESC toggles viewing <-> picking, 'h' homes, like SGI viewers.
    if (e->type == SDL_KEYDOWN) {
	if (e->key.keysym.sym == SDLK_ESCAPE) {
	    viewing = !viewing;
	    return;
	}
	if (viewing && e->key.keysym.sym == SDLK_h) {
	    viewAll();
	    return;
	}
    }

    if (!viewing) {
	// Picking mode: scene graph sees everything.
	SoSDLRenderArea::processEvent(e);
	return;
    }

    SDL_Keymod kmod = SDL_GetModState();

    switch (e->type) {
      case SDL_MOUSEBUTTONDOWN: {
	SbVec2s pos(e->button.x, getSize()[1] - 1 - e->button.y);
	if (e->button.button == SDL_BUTTON_LEFT) {
	    if (kmod & KMOD_CTRL)	startDrag(DOLLY, pos);
	    else if (kmod & KMOD_SHIFT)	startDrag(PAN, pos);
	    else			startDrag(SPIN, pos);
	}
	else if (e->button.button == SDL_BUTTON_MIDDLE)
	    startDrag(PAN, pos);
	else if (e->button.button == SDL_BUTTON_RIGHT)
	    startDrag(DOLLY, pos);
	return;
      }
      case SDL_MOUSEBUTTONUP:
	endDrag();
	return;
      case SDL_MOUSEMOTION:
	if (mode != IDLE)
	    drag(SbVec2s(e->motion.x, getSize()[1] - 1 - e->motion.y));
	return;
      case SDL_MOUSEWHEEL:
	dollyCamera(e->wheel.y * -0.1f);
	invokeFinishCallbacks();
	scheduleRedraw();
	return;
    }

    // Anything else (keys in viewing mode, etc.) goes to the scene.
    SoSDLRenderArea::processEvent(e);
}
