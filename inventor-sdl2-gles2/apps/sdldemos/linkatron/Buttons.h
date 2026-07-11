//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |      Button
 |      ToggleButton
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _MECH_BUTTONS_
#define  _MECH_BUTTONS_

#include <Inventor/fields/SoSFBool.h>
#include <Inventor/draggers/SoDragger.h>

// isOn is TRUE while mouse is pressed over button.
// FALSE when it lets up.
class Button : public SoDragger {

    SO_KIT_HEADER(Button);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(buttonSwitch);

    SO_KIT_CATALOG_ENTRY_HEADER(buttonSep);
    SO_KIT_CATALOG_ENTRY_HEADER(buttonGeom);
    SO_KIT_CATALOG_ENTRY_HEADER(buttonText);

    SO_KIT_CATALOG_ENTRY_HEADER(buttonActiveSep);
    SO_KIT_CATALOG_ENTRY_HEADER(buttonActiveGeom);
    SO_KIT_CATALOG_ENTRY_HEADER(buttonActiveText);

  public:

    // constructor
    Button();

    // Is the button on?
    SoSFBool isOn;

  SoINTERNAL public:
    static void initClass();

  protected:

    static void startCB( void *, SoDragger *);
    static void finishCB( void *, SoDragger *);

  private:

  protected:	// SDL port: derived classes need the dtor
    // destructor
    virtual ~Button();
  private:
};

// Just like button, but toggles value of 'isOn' with
// each press&release.
class ToggleButton : public Button {

    SO_KIT_HEADER(ToggleButton);

  public:

    // constructor
    ToggleButton();

  SoINTERNAL public:
    static void initClass();

  protected:

    static void startCB( void *, SoDragger *);

  private:

  protected:	// SDL port: derived classes need the dtor
    // destructor
    virtual ~ToggleButton();
  private:
};

#endif /* _MECH_BUTTONS_ */
