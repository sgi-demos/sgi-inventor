/*
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes	: TsViewer
 |
 |   Author(s)	: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SIMPLE_VIEWER_
#define  _SIMPLE_VIEWER_

#include <Inventor/Xt/viewers/SoXtViewer.h>
#include <Inventor/SbLinear.h>


//
// Class: TsViewer
//
// Drop Viewer - viewer which only operates in viewing mode.  Used with
// the drop game.
//
class TsViewer : public SoXtViewer {
 public:
    // constructor/destructor
    TsViewer(
        Widget parent = NULL,
        const char *name = NULL);
    ~TsViewer();
    
 protected:
    // redefine this to process the events
    virtual void	processEvent(XAnyEvent *anyevent);
    
    // redefine this to cache the renderArea size
    virtual void	sizeChanged(const SbVec2s &newSize);
    
 private:
    // viewer state variables
    int		    mode;
    SbBool	    createdCursors;
    Cursor	    vwrCursor, seekCursor;
    SbVec2s	    locator;    // mouse position
    SbVec2s	    movement;   // mouse movement
    SbVec2s	    windowSize; // cached size of the window
    
    // camera translation vars
    SbVec3f	    locator3D;
    SbPlane	    focalplane;
    float	    transXspeed, transYspeed;
    
    void	    switchMode(int newMode);
    void	    defineCursors();
    void	    translateCamera();
    void            rotateCamera();
    void	    computeTranslateValues();
    static void	    transWheelStartCB(void *v, float);
};

#endif  /* _TS_VIEWER_ */
