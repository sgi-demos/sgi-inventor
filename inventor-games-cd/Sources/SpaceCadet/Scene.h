////////////////////////////////////////////////////////////////////////////
//
// Class: Scene
//
// Reads scene from file or string, returning root. Finds nodes by name.
//
////////////////////////////////////////////////////////////////////////////

extern class SoEngine;
extern class SoNode;

class Scene {
  public:
    // Read from file or string
    static SoNode *	readFile(const char *filename);
    static SoNode *	readString(const char *string);

    // Find node by name in global DB or under root
    static SoNode *	find(const char *nodeName);
    static SoNode *	findUnder(const char *nodeName, SoNode *root);

    // Finds engine by name in DB
    static SoEngine *	findEngine(const char *engineName);
};
