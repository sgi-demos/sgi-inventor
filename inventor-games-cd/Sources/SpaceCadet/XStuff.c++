/*
 * Copyright 1994, Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Silicon Graphics, Inc.
 *
 * RESTRICTED RIGHTS LEGEND:
 * Use, duplication or disclosure by the Government is subject to restrictions
 * as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
 * and Computer Software clause at DFARS 252.227-7013, and/or in similar or
 * successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
 * rights reserved under the Copyright Laws of the United States.
 */
#include <Inventor/Xt/SoXtComponent.h>
#include "XStuff.h"

SoXtComponent	*XStuff::component = NULL;
Display		*XStuff::display;
Cursor		*XStuff::cursors = NULL;

////////////////////////////////////////////////////////////////////////////
//
// These cursor definitions were created and edited using "bitmap":

#define direction_cursor_width 24
#define direction_cursor_height 24
#define direction_cursor_x_hot 11
#define direction_cursor_y_hot 11
static unsigned char direction_cursor_bits[] = {
   0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00,
   0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00,
   0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0xff, 0xf7, 0xff,
   0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00,
   0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00,
   0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00};

#define target_cursor_width 24
#define target_cursor_height 24
#define target_cursor_x_hot 11
#define target_cursor_y_hot 11
static unsigned char target_cursor_bits[] = {
   0x00, 0x08, 0x00, 0x00, 0x3e, 0x00, 0x80, 0xc9, 0x00, 0x60, 0x08, 0x03,
   0x10, 0x08, 0x04, 0x08, 0x08, 0x08, 0x08, 0x7f, 0x08, 0x84, 0x88, 0x10,
   0x44, 0x08, 0x11, 0x42, 0x1c, 0x21, 0x42, 0x22, 0x21, 0xff, 0xe3, 0xff,
   0x42, 0x22, 0x21, 0x42, 0x1c, 0x21, 0x44, 0x08, 0x11, 0x84, 0x88, 0x10,
   0x08, 0x7f, 0x08, 0x08, 0x08, 0x08, 0x10, 0x08, 0x04, 0x60, 0x08, 0x03,
   0x80, 0xc9, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00};

#define wait_cursor_width 24
#define wait_cursor_height 24
#define wait_cursor_x_hot 11
#define wait_cursor_y_hot 11
static unsigned char wait_cursor_bits[] = {
   0xf8, 0xff, 0x0f, 0x3c, 0x00, 0x1c, 0x10, 0x00, 0x08, 0x10, 0x00, 0x08,
   0x20, 0x00, 0x04, 0xa0, 0x40, 0x07, 0x40, 0xd3, 0x02, 0x80, 0x7d, 0x01,
   0x00, 0x9b, 0x00, 0x00, 0x7a, 0x00, 0x00, 0x34, 0x00, 0x00, 0x18, 0x00,
   0x00, 0x18, 0x00, 0x00, 0x24, 0x00, 0x00, 0x46, 0x00, 0x00, 0x89, 0x00,
   0x80, 0x00, 0x01, 0x40, 0x08, 0x02, 0x60, 0x94, 0x04, 0xa0, 0x75, 0x06,
   0xb0, 0xff, 0x0b, 0xf0, 0x7d, 0x0a, 0xbc, 0xd6, 0x1d, 0xf8, 0xff, 0x0f};

#define pause_cursor_width 24
#define pause_cursor_height 24
#define pause_cursor_x_hot 12
#define pause_cursor_y_hot 11
static unsigned char pause_cursor_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x00, 0xe0, 0xff, 0x01,
   0xe0, 0xff, 0x03, 0xe0, 0x80, 0x07, 0xe0, 0x00, 0x07, 0xe0, 0x00, 0x07,
   0xe0, 0x00, 0x07, 0xe0, 0x00, 0x07, 0xe0, 0x80, 0x07, 0xe0, 0xff, 0x03,
   0xe0, 0xff, 0x01, 0xe0, 0xff, 0x00, 0xe0, 0x00, 0x00, 0xe0, 0x00, 0x00,
   0xe0, 0x00, 0x00, 0xe0, 0x00, 0x00, 0xe0, 0x00, 0x00, 0xe0, 0x00, 0x00,
   0xe0, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
// Sets component changes will affect
//
////////////////////////////////////////////////////////////////////////////

void
XStuff::setComponent(SoXtComponent *_component)
{
    component = _component;
    display = component->getDisplay();
}

////////////////////////////////////////////////////////////////////////////
//
// Sets current cursor.
//
////////////////////////////////////////////////////////////////////////////

void
XStuff::setCursor(CursorCode code)
{
    if (cursors == NULL) {
	cursors = new Cursor[4];

	cursors[DIRECTION] =
	    createCursor(direction_cursor_bits,
			 direction_cursor_width, direction_cursor_height,
			 direction_cursor_x_hot, direction_cursor_y_hot);

	cursors[TARGET] =
	    createCursor(target_cursor_bits,
			 target_cursor_width, target_cursor_height,
			 target_cursor_x_hot, target_cursor_y_hot);

	cursors[WAIT] =
	    createCursor(wait_cursor_bits,
			 wait_cursor_width, wait_cursor_height,
			 wait_cursor_x_hot, wait_cursor_y_hot);

	cursors[PAUSE] =
	    createCursor(pause_cursor_bits,
			 pause_cursor_width, pause_cursor_height,
			 pause_cursor_x_hot, pause_cursor_y_hot);
    }

    XDefineCursor(display, XtWindow(component->getWidget()), cursors[code]);
}

////////////////////////////////////////////////////////////////////////////
//
// Moves pointer to given spot.
//
////////////////////////////////////////////////////////////////////////////

void
XStuff::movePointer(const SbVec2s &loc)
{
    XWarpPointer(component->getDisplay(),
		 NULL,
		 XtWindow(component->getWidget()),
		 0, 0, 0, 0,
		 loc[0], loc[1]);
}

////////////////////////////////////////////////////////////////////////////
//
// Creates a cursor.
//
////////////////////////////////////////////////////////////////////////////

Cursor
XStuff::createCursor(unsigned char *data, u_int width, u_int height,
		     u_int hotSpotX, u_int hotSpotY)
{
    XColor foreground;
    Pixmap source;
    Drawable d = DefaultRootWindow(display);
    Cursor cursor;

    foreground.red = 65535;
    foreground.green = foreground.blue = 10000;

    source = XCreateBitmapFromData(display, d, (char *) data, width, height);
    cursor = XCreatePixmapCursor(display, source, source, 
				 &foreground, &foreground,
				 hotSpotX, hotSpotY);

    XFreePixmap(display, source);

    return cursor;
}
