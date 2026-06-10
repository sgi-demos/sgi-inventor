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
#include <X11/Intrinsic.h>

#include <Xm/MessageB.h>

#include "Message.h"
#include "MotifHelp.h"

int	Message::numShown;	// Number of messages shown

////////////////////////////////////////////////////////////////////////
//
// Constructor - takes parent widget and message.
//
//
////////////////////////////////////////////////////////////////////////

Message::Message(Widget parent, const char *message)
{
    // If any message is already shown, don't show another
    if (numShown++ > 0) {
	delete this;
	return;
    }

    ARG_VARS(10);

    deleteMe = TRUE;

    ADD_ARG(XmNmessageString, XmStringCreateLtoR((char *) message,
						 XmSTRING_DEFAULT_CHARSET));
    widget = XmCreateMessageDialog(parent, "Message", ARGS);

    // Get rid of buttons we don't need
    XtUnmanageChild(XmMessageBoxGetChild(widget, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(widget, XmDIALOG_HELP_BUTTON));

    // Set up callbacks to use "this"
    XtAddCallback(widget, XmNdestroyCallback,
		  (XtCallbackProc) &Message::destroyCB, (XtPointer) this);
    XtAddCallback(widget, XmNokCallback,
		  (XtCallbackProc) &Message::okCB,  (XtPointer) this);

    // Display widget
    XtManageChild(widget);
}    

////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
////////////////////////////////////////////////////////////////////////

Message::~Message()
{
    // Do this for only the one message that is shown at a time
    if (--numShown == 0) {
	// Hide widget
	XtUnmanageChild(widget);
	XtDestroyWidget(widget);
    }
}    

////////////////////////////////////////////////////////////////////////
//
// Callback called when the widget is destroyed. The clientData
// pointer points to a Message instance.
//
////////////////////////////////////////////////////////////////////////

void
Message::destroyCB(Widget, XtPointer clientData, XtPointer)
{
    // Note that this will be called when the ok button is pressed, so
    // be careful!!!!

    Message *msg = (Message *) clientData;

    // Make sure we haven't already started the deletion of the message-box
    if (msg->deleteMe) {
	msg->deleteMe = FALSE;
	delete msg;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the "Ok" button in the message-box is
//    activated. The clientData pointer points to a Message instance.
//

void
Message::okCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    Message	*msg = (Message *) clientData;

    msg->deleteMe = FALSE;
    delete msg;
}
