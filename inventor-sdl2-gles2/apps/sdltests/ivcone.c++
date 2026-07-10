// M2: first pixels. SDL2 window + GL context driving SoSceneManager.
// Renders the classic red cone; writes a PPM screenshot if asked.
//   usage: ivcone [out.ppm] [file.iv]
#include <SDL2/SDL.h>
#include <GL/gl.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/SoSceneManager.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <stdio.h>

static const int W = 640, H = 480;

static SoSeparator *buildScene(const char *ivfile)
{
    SoSeparator *root = new SoSeparator;
    root->ref();

    SoPerspectiveCamera *camera = new SoPerspectiveCamera;
    root->addChild(camera);
    root->addChild(new SoDirectionalLight);

    if (ivfile) {
        SoInput in;
        if (in.openFile(ivfile)) {
            SoSeparator *model = SoDB::readAll(&in);
            if (model) root->addChild(model);
        }
    } else {
        SoMaterial *mat = new SoMaterial;
        mat->diffuseColor.setValue(0.8f, 0.1f, 0.1f);   // the red cone
        root->addChild(mat);
        root->addChild(new SoCone);
    }

    camera->viewAll(root, SbViewportRegion(W, H));
    return root;
}

static void writePPM(const char *path)
{
    unsigned char *pix = new unsigned char[W * H * 3];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, pix);
    FILE *f = fopen(path, "wb");
    fprintf(f, "P6\n%d %d\n255\n", W, H);
    for (int y = H - 1; y >= 0; y--)            // GL rows are bottom-up
        fwrite(pix + y * W * 3, 1, W * 3, f);
    fclose(f);
    delete [] pix;
    printf("wrote %s\n", path);
}

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_Window *win = SDL_CreateWindow("Inventor SDL — cone",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H,
        SDL_WINDOW_OPENGL);
    if (!win) { fprintf(stderr, "window: %s\n", SDL_GetError()); return 1; }
    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    if (!ctx) { fprintf(stderr, "context: %s\n", SDL_GetError()); return 1; }
    // SoXtRenderArea enabled depth testing when realizing its GL widget;
    // that responsibility now falls to the SDL surface owner.
    glEnable(GL_DEPTH_TEST);
    printf("GL_VENDOR:   %s\nGL_RENDERER: %s\nGL_VERSION:  %s\n",
        glGetString(GL_VENDOR), glGetString(GL_RENDERER),
        glGetString(GL_VERSION));

    SoInteraction::init();
    SoSeparator *root = buildScene(argc > 2 ? argv[2] : NULL);

    SoSceneManager *mgr = new SoSceneManager;
    mgr->setSceneGraph(root);
    mgr->setWindowSize(SbVec2s(W, H));
    mgr->setBackgroundColor(SbColor(0.2f, 0.2f, 0.3f));
    mgr->activate();

    mgr->render();
    SDL_GL_SwapWindow(win);

    if (argc > 1) {
        mgr->render();        // render again into back buffer, read it
        glFinish();
        writePPM(argv[1]);
    }

    // brief interactive loop when a real display is present
    Uint32 until = SDL_GetTicks() + 2000;
    SDL_Event e;
    while (SDL_GetTicks() < until) {
        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT) until = 0;
        SDL_Delay(16);
    }

    root->unref();
    delete mgr;
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
