// M3: interactive examiner viewer — the SDL "ivview".
//   usage: ivview file.iv
#include "SoSDL.h"
#include "SoSDLExaminerViewer.h"
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoSeparator.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    SoSDL::init();

    SoSeparator *scene = NULL;
    if (argc > 1) {
        SoInput in;
        if (!in.openFile(argv[1])) {
            fprintf(stderr, "cannot open %s\n", argv[1]);
            return 1;
        }
        scene = SoDB::readAll(&in);
    }
    if (!scene) {
        fprintf(stderr, "usage: %s file.iv\n", argv[0]);
        return 1;
    }
    scene->ref();

    SoSDLExaminerViewer viewer("Inventor SDL Viewer", 800, 600);
    viewer.setBackgroundColor(SbColor(0.2f, 0.2f, 0.3f));
    viewer.setSceneGraph(scene);

    SoSDL::mainLoop();
    scene->unref();
    return 0;
}
