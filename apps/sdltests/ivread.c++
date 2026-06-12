// M1 smoke test: prove core libInventor is alive without any GUI.
// Reads a .iv file, reports scene statistics, writes the graph to stdout.
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoSeparator.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s file.iv [out.iv]\n", argv[0]);
        return 1;
    }

    SoInteraction::init();   // inits SoDB + SoNodeKit + interaction

    SoInput in;
    if (!in.openFile(argv[1])) {
        fprintf(stderr, "cannot open %s\n", argv[1]);
        return 1;
    }
    SoSeparator *root = SoDB::readAll(&in);
    if (!root) {
        fprintf(stderr, "read failed for %s\n", argv[1]);
        return 1;
    }
    root->ref();

    SoSearchAction search;
    search.setType(SoNode::getClassTypeId());
    search.setInterest(SoSearchAction::ALL);
    search.apply(root);
    printf("read OK: %s — %d nodes in scene graph\n",
           argv[1], search.getPaths().getLength());

    if (argc > 2) {
        SoOutput out;
        if (!out.openFile(argv[2])) {
            fprintf(stderr, "cannot open %s for write\n", argv[2]);
            return 1;
        }
        SoWriteAction wa(&out);
        wa.apply(root);
        out.closeFile();
        printf("wrote %s\n", argv[2]);
    }

    root->unref();
    return 0;
}
