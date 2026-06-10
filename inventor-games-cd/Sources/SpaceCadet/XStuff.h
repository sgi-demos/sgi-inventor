////////////////////////////////////////////////////////////////////////////
//
// Class: XStuff
//
// Handles X stuff, such as cursors.
//
////////////////////////////////////////////////////////////////////////////

#include <Inventor/SbLinear.h>

extern class Display;
extern class SoXtComponent;

class XStuff {
  public:
    enum CursorCode {
	DIRECTION,	// Flying direction indicator
	TARGET,		// Blaster target indicator
	WAIT,		// Waiting
	PAUSE,		// Paused
    };

    // Sets component changes will affect
    static void		setComponent(SoXtComponent *component);

    // Sets current cursor
    static void		setCursor(CursorCode code);
    
    // Moves pointer to given spot
    static void		movePointer(const SbVec2s &loc);

  private:
    static Display		*display;
    static SoXtComponent	*component;
    static Cursor		*cursors;

    // Creates a cursor
    static Cursor		createCursor(unsigned char *data,
					     u_int width, u_int height,
					     u_int hotSpotX, u_int hotSpotY);
};
