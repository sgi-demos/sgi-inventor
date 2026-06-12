// M3 verification: drive the examiner viewer with synthetic SDL events
// and check that the camera responds (spin / pan / dolly), saving
// before/after screenshots for visual confirmation.
//   usage: ivinteract file.iv outprefix
#include "SoSDL.h"
#include "SoSDLExaminerViewer.h"
#include <GL/gl.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCamera.h>
#include <stdio.h>
#include <string.h>

static const int W = 640, H = 480;

static void shoot(SoSDLExaminerViewer &v, const char *prefix, const char *tag)
{
    v.render();
    v.render();                 // ensure back buffer holds latest frame
    glFinish();
    char path[512];
    snprintf(path, sizeof path, "%s-%s.ppm", prefix, tag);
    unsigned char *pix = new unsigned char[W * H * 3];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, pix);
    FILE *f = fopen(path, "wb");
    fprintf(f, "P6\n%d %d\n255\n", W, H);
    for (int y = H - 1; y >= 0; y--)
        fwrite(pix + y * W * 3, 1, W * 3, f);
    fclose(f);
    delete [] pix;
    printf("wrote %s\n", path);
}

static void pushButton(Uint32 winId, Uint32 type, Uint8 button, int x, int y)
{
    SDL_Event e;
    memset(&e, 0, sizeof e);
    e.type = type;
    e.button.windowID = winId;
    e.button.button = button;
    e.button.x = x; e.button.y = y;
    e.button.state = (type == SDL_MOUSEBUTTONDOWN) ? SDL_PRESSED : SDL_RELEASED;
    SDL_PushEvent(&e);
}

static void pushMotion(Uint32 winId, int x, int y)
{
    SDL_Event e;
    memset(&e, 0, sizeof e);
    e.type = SDL_MOUSEMOTION;
    e.motion.windowID = winId;
    e.motion.x = x; e.motion.y = y;
    SDL_PushEvent(&e);
}

static void pump()    // run a few loop iterations to consume events
{
    for (int i = 0; i < 5; i++)
        SoSDL::doOneIteration();
}

static void printCam(SoCamera *c, const char *tag)
{
    SbVec3f p = c->position.getValue();
    SbVec3f ax; float ang;
    c->orientation.getValue().getValue(ax, ang);
    printf("%-10s pos(%.3f %.3f %.3f) orient(%.2f %.2f %.2f @ %.3f) focal %.3f\n",
        tag, p[0], p[1], p[2], ax[0], ax[1], ax[2], ang,
        c->focalDistance.getValue());
}

int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "usage: %s file.iv outprefix\n", argv[0]); return 1; }
    SoSDL::init();

    SoInput in;
    if (!in.openFile(argv[1])) { fprintf(stderr, "cannot open %s\n", argv[1]); return 1; }
    SoSeparator *scene = SoDB::readAll(&in);
    if (!scene) { fprintf(stderr, "read failed\n"); return 1; }
    scene->ref();

    SoSDLExaminerViewer viewer("interact test", W, H);
    viewer.setBackgroundColor(SbColor(0.2f, 0.2f, 0.3f));
    viewer.setSceneGraph(scene);
    Uint32 id = viewer.getWindowId();

    SoCamera *cam = viewer.getCamera();
    printCam(cam, "initial");
    shoot(viewer, argv[2], "0-initial");

    // --- spin: left-drag from center to upper-right
    pushButton(id, SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT, 320, 240); pump();
    for (int x = 320, y = 240; x < 440; x += 20, y -= 12) { pushMotion(id, x, y); pump(); }
    pushButton(id, SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT, 440, 168); pump();
    printCam(cam, "spun");
    shoot(viewer, argv[2], "1-spin");

    // --- pan: middle-drag left
    pushButton(id, SDL_MOUSEBUTTONDOWN, SDL_BUTTON_MIDDLE, 320, 240); pump();
    for (int x = 320; x > 200; x -= 20) { pushMotion(id, x, 240); pump(); }
    pushButton(id, SDL_MOUSEBUTTONUP, SDL_BUTTON_MIDDLE, 200, 240); pump();
    printCam(cam, "panned");
    shoot(viewer, argv[2], "2-pan");

    // --- dolly: right-drag up (closer)
    pushButton(id, SDL_MOUSEBUTTONDOWN, SDL_BUTTON_RIGHT, 320, 240); pump();
    for (int y = 240; y > 140; y -= 20) { pushMotion(id, 320, y); pump(); }
    pushButton(id, SDL_MOUSEBUTTONUP, SDL_BUTTON_RIGHT, 320, 140); pump();
    printCam(cam, "dollied");
    shoot(viewer, argv[2], "3-dolly");

    scene->unref();
    return 0;
}
