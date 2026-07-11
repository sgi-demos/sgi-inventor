#include <Inventor/SbBasic.h>
#ifdef SDL_PORT
typedef void *Widget;
typedef void *XtPointer;
#endif

//////////////////////////////////////////////////////////////////////////////
//
//  Displays a message in a pop-up window.
//
//////////////////////////////////////////////////////////////////////////////

class Message {

  public:
    // Constructor - takes parent widget and message
    Message(Widget parent, const char *message);

    // Destructor
    ~Message();

  private:
    Widget		widget;
    SbBool		deleteMe;	// If should be deleted in destroyCB
    static int		numShown;	// Number of messages shown

    // Callbacks for when this is destroyed or button is pressed
    static void		destroyCB(Widget, XtPointer, XtPointer);
    static void		okCB(Widget, XtPointer, XtPointer);
};
