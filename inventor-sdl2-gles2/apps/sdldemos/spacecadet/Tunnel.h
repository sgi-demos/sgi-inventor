////////////////////////////////////////////////////////////////////////////
//
// Class: Tunnel
//
// Made up of straight sections joied at corners. Based on given
// level (>= 1). Origin at center of start of first section, looking
// down negative z axis. Radius always 1, length always 1000.
//
////////////////////////////////////////////////////////////////////////////

#include <Inventor/SbLinear.h>

extern struct Motion;
extern struct TunnelSection;

extern class SoAction;
extern class SoNode;

class Tunnel {
  public:
    // Constructor and destructor
    Tunnel(int level);
    ~Tunnel();

    // Returns scene graph representing tunnel
    SoNode *		getGraph();

    // Updates location and status of ship within tunnel
    void		updateShip(Motion *shipMotion);

    // Moves ship safely through tunnel for given distance
    void		moveSafely(Motion *shipMotion, float distance);

    // Updates location and status of blast within tunnel
    void		updateBlast(Motion *blastMotion);

  private:
    // These are constant for all tunnels:
    float	radius;			// Radius of tunnel bore
    int		numSides;		// Number of sides of cross section
    float	totalLength;		// Total length of tunnel
    float	maxSectionLength;	// Maximum length of straight section
    float	maxSubSectionLength;	// Maximum length of subsection

    // These vary based on current level:
    float	minSectionLength;	// Minimum length of straight section
    float	maxJointAngle;		// Maximum joint angle (0 == straight)

    // These represent the straight sections of the tunnel
    int		numSections;		// Number of straight sections
    TunnelSection *sections;		// Sections

    // These store the coordinates, normals, and texture coordinates
    // of the vertices of each section (and subsection). There are
    // (numSides + 1) entries for each cross-section, and there are (N
    // + 1) cross-sections, in order from start to finish, where N is
    // the total number of subsections in all sections.
    SbVec3f	*vertexCoords;		// Coordinates at vertices
    SbVec3f	*vertexNormals;		// Normals at vertices (pointing in)
    SbVec2f	*vertexTexCoords;	// Texture coordinates at vertices
    int		numVertices;		// Number of vertices set

    // These are nodes in the scene graph representing the tunnel
    SoNode	*root;			// Root of scene graph

    // These maintain information about the current position of the
    // ship within the tunnel
    int		curSection;		// Section containing position
    float	prevDistance;		// From start to beginning of section

    // Computes per-section info
    void	computeSections();

    // Computes vertex coordinates and normals
    void	computeVertices();

    // Stores vertices of nth cross-section
    void	storeCrossSection(int sectionNum,
				  const SbRotation &rotation,
				  const SbVec3f &translation,
				  float distFromStart);

    // Adds mines/rings to tunnel, based on current level
    void	addMines(int level);
    void	addRings(int level);

    // Computes a position that is the given distance from the start
    // of the section and the given radius from the center axis.
    static SbVec3f computePosition(TunnelSection *sec,
				   float distance, float radius);

    // Builds scene graph
    void	buildGraph();

    // Updates mine/ring status
    void	updateMines(Motion *shipMotion);
    void	updateRings(Motion *shipMotion);

    // Implements updateBlast() for a section
    void	updateBlastSection(int secNum, Motion *blastMotion);

    // Sets switch for each section on or off, based on current position
    void	setSectionSwitches();

    // Callback to enable or disable GL depth test. The userData
    // pointer is actually the Boolean flag.
    static void	depthCB(void *userData, SoAction *action);
};
