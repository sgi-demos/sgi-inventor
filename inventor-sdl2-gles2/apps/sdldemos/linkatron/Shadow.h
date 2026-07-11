//  -*- C++ -*-

/*
 * Copyright (C) 1992   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   Class:
 |	Shadow, ShadowKit
 |
 |   Description:
 |	A shadow object thingy.
 |
 |   Author(s): Howard Look, Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SHADOW_CLASS_
#define _SHADOW_CLASS_

#include <Inventor/SbLinear.h>

#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFPlane.h>

class Shadow : public SoMatrixTransform {

    SO_NODE_HEADER(Shadow);

  public:

    // Constructor
    Shadow();

    // Dir. of the light. Default is (-1,-1,-1)
    SoSFVec3f		direction;   

    // Plane to project the shadow onto. Default is ((0 1 0), 0).
    SoSFPlane		projectionPlane;

    static void initClass();

  protected:
    
    ~Shadow();

    void		calculateXform();
    
    // Sensors
    static void		fieldSensorCB(void *, SoSensor *);
    SoFieldSensor *directionFieldSensor;
    SoFieldSensor *planeFieldSensor;
};

class ShadowKit : public SoBaseKit {

    SO_KIT_HEADER(ShadowKit);

    SO_KIT_CATALOG_ENTRY_HEADER(topSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(shadow);
    SO_KIT_CATALOG_ENTRY_HEADER(separator);
    SO_KIT_CATALOG_ENTRY_HEADER(lightModel);
    SO_KIT_CATALOG_ENTRY_HEADER(material);
    SO_KIT_CATALOG_ENTRY_HEADER(stuff);
    
  public:

    // Constructor
    ShadowKit();

    // Flips the topSwitch on and off.
    SbBool isShown();
    void show();
    void hide();

    static void initClass();

  protected:
    
    ~ShadowKit();
};

#endif /* _SHADOW_CLASS_ */

