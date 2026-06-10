////////////////////////////////////////////////////////////////////////////
//
// Class: Meter
//
// Displays scalar as a meter bar. Always constructs meter graphics to
// fit in -1 to 1 in x. Meter scale goes from 0 to 1.
//
////////////////////////////////////////////////////////////////////////////

#include <Inventor/SbBasic.h>

extern class SoNode;
extern class SoSwitch;
extern class SoTransform;

class Meter {
  public:
    // Constructor and destructor
    Meter(float start, float minAlarm, float maxAlarm);
    ~Meter();

    // Returns scene graph
    SoNode *		getGraph();

    // Changes current meter value
    void		setValue(float newValue);

    // Returns current meter value
    float		getValue() const		{ return curValue; } 

    // Resets to initial value
    void		reset();

  private:
    float		initValue;	// Initial meter value
    float		curValue;	// Current meter value
    float		minAlarm;	// Min value to activate alarm
    float		maxAlarm;	// Max value to activate alarm
    SbBool		inAlarm;
    SoNode		*root;		// Root of scene graph
    SoSwitch		*alarmSwitch;	// Activates alarm color
    SoTransform		*barXform;	// Scales meter bar

    // Builds scene graph
    void		buildGraph();
};
