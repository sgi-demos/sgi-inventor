//  -*- C++ -*-

//////////////////////////////////////////////////////////////////
//
// Dealing with Xt/Xm arguments
//

#define ARG_VARS(num)							      \
    Cardinal    argN = 0;						      \
    Arg         args[num]

#define ADD_ARG(resource, value)					      \
		XtSetArg(args[argN], resource, value); argN++

#define RESET_ARGS()    argN = 0
#define ARGS            args, argN

//////////////////////////////////////////////////////////////////
//
// Dealing with form arguments
//

#define ADD_LEFT_FORM(offset)						      \
    ADD_ARG(XmNleftAttachment,		XmATTACH_FORM);			      \
    ADD_ARG(XmNleftOffset,		offset)

#define ADD_LEFT_WIDGET(widget, offset)					      \
    ADD_ARG(XmNleftAttachment,		XmATTACH_WIDGET);		      \
    ADD_ARG(XmNleftWidget,		widget);			      \
    ADD_ARG(XmNleftOffset,		offset)

#define ADD_RIGHT_FORM(offset)						      \
    ADD_ARG(XmNrightAttachment,		XmATTACH_FORM);			      \
    ADD_ARG(XmNrightOffset,		offset)

#define ADD_RIGHT_WIDGET(widget, offset)				      \
    ADD_ARG(XmNrightAttachment,		XmATTACH_WIDGET);		      \
    ADD_ARG(XmNrightWidget,		widget);			      \
    ADD_ARG(XmNrightOffset,		offset)

#define ADD_TOP_FORM(offset)						      \
    ADD_ARG(XmNtopAttachment,		XmATTACH_FORM);			      \
    ADD_ARG(XmNtopOffset,		offset)

#define ADD_TOP_WIDGET(widget, offset)					      \
    ADD_ARG(XmNtopAttachment,		XmATTACH_WIDGET);		      \
    ADD_ARG(XmNtopWidget,		widget);			      \
    ADD_ARG(XmNtopOffset,		offset)

#define ADD_BOTTOM_FORM(offset)						      \
    ADD_ARG(XmNbottomAttachment,	XmATTACH_FORM);			      \
    ADD_ARG(XmNbottomOffset,		offset)

#define ADD_BOTTOM_WIDGET(widget, offset)				      \
    ADD_ARG(XmNbottomAttachment,	XmATTACH_WIDGET);		      \
    ADD_ARG(XmNbottomWidget,		widget);			      \
    ADD_ARG(XmNbottomOffset,		offset)

//////////////////////////////////////////////////////////////////
//
// Dealing with Xm strings
//

#define STRING(a) XmStringCreateSimple(a)


#if JUST_FOR_CUT_AND_PASTE
    ADD_LEFT_FORM(0);
    ADD_LEFT_WIDGET(w, 0);

    ADD_RIGHT_FORM(0);
    ADD_RIGHT_WIDGET(w, 0);

    ADD_TOP_FORM(0);
    ADD_TOP_WIDGET(w, 0);

    ADD_BOTTOM_FORM(0);
    ADD_BOTTOM_WIDGET(w, 0);
#endif
