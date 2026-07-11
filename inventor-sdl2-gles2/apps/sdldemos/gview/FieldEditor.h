/*
 *
 *  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved. 
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 *  Mountain View, CA  94043, or:
 * 
 *  http://www.sgi.com 
 * 
 *  For further information regarding this notice, see: 
 * 
 *  http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 *
 */

//  -*- C++ -*-

#include <Inventor/SbString.h>
#ifdef SDL_PORT
#include <Inventor/SbBasic.h>
#ifndef GVIEW_SDL_XT_TYPES
#define GVIEW_SDL_XT_TYPES
typedef void *Widget;
typedef void *XtPointer;
#endif
#else
#include <Inventor/Xt/SoXtComponent.h>
#endif /* SDL_PORT */

class SoBase;
class SoError;
class SoNode;
class SoNodeSensor;
class SoSensor;

class FieldEditor;		// Forward reference

//////////////////////////////////////////////////////////////////////////////
//
//  Class: FieldEditor
//
//  Subclass of SoXtComponent that is used to edit values of a node's
//  fields.
//
//////////////////////////////////////////////////////////////////////////////

// Type of callback function passed to setFinishCallback()
typedef void	FieldEditorCB(void *userData, FieldEditor *editor);

#ifdef SDL_PORT

class SoField;

// Console/text-prompt port of the Motif field editor. show() prints
// the node's fields to the terminal and takes over keyboard input in
// the graph window; typed lines of the form "<fieldName> <value>" are
// applied to the node immediately (the Motif editor's Apply built the
// very same string for SoNode::set()). !-commands cover the dialog's
// buttons (defaults, Ignore and Override toggles).
class FieldEditor {
 public:
    // Constructor. Takes pointer to node whose fields we are to edit,
    // along with parent (ignored) and widget name (ignored)
    FieldEditor(SoNode *node, Widget parent = NULL, const char *name = NULL);

    // Destructor
    ~FieldEditor();

    // Returns number of fields in given node. This is static to allow
    // the check to be made before an editor is constructed.
    static int		getNumFields(SoNode *node);

    // Sets a callback to call when editing is finished.
    void		setFinishCallback(FieldEditorCB *cb,
					  const void *userData = NULL);

    // Prints the fields and starts the editing prompt
    void		show();

  private:
    SoNode		*nodeToEdit;	// Node whose fields we are editing
    SoNode		*defNode;	// Same type of node with default vals
    FieldEditorCB	*finishCB;	// Finish callback (NULL if none)
    void		*finishData;	// User data for callback
    int			numFields;	// Number of fields in node
    SbString		*errorString;	// Saves field read error

    // Prints all fields with their current values and flags
    void		printFields();

    // Applies one typed line; returns TRUE when editing is finished
    SbBool		applyLine(const char *line);

    // Invokes the finish callback and deletes the editor
    void		finish();

    // Returns the named field of the edited node (NULL if no such
    // field), optionally returning its index
    SoField *		findField(const char *name, int *indexOut = NULL);

    // TextPrompt callbacks
    static void		lineCB(void *userData, const char *line);
    static void		cancelCB(void *userData);

    // Callback for read errors that occur when a field's value is set
    static void		readErrorCB(const SoError *error, void *data);

    // Displays error message
    void		displayReadError();
};

#else /* SDL_PORT */

class FieldEditor : public SoXtComponent {
 public:
    // Constructor. Takes pointer to node whose fields we are to edit,
    // along with parent and widget name
    FieldEditor(SoNode *node, Widget parent = NULL, const char *name = NULL);

    // Destructor
    ~FieldEditor();

    // Returns number of fields in given node. This is static to allow
    // the check to be made before an editor is constructed.
    static int		getNumFields(SoNode *node);

    // Sets a callback to call when editing is finished.
    void		setFinishCallback(FieldEditorCB *cb,
					  const void *userData = NULL);

  private:
    SoNode		*nodeToEdit;	// Node whose fields we are editing
    SoNode		*defNode;	// Same type of node with default vals
    FieldEditorCB	*finishCB;	// Finish callback (NULL if none)
    void		*finishData;	// User data for callback
    SoNodeSensor	*nodeSensor;	// Sensor on edited node
    int			numFields;	// Number of fields in node
    struct FieldInfo	*fieldInfos;	// Widgets/data for each field
    static char		*fieldBuf;	// Buffer for writing field values
    static int		fieldBufSize;	// Size of buffer in bytes
    Widget		applyButton;	// Default action - applies changes
    Widget		overrideButton;	// To access current override state
    SbString		*errorString;	// Saves field read error

    //
    // Widget building
    //

    // Builds all widgets
    Widget		buildWidget(Widget parent);

    // Creates and returns a widget containing all field-editing widgets
    Widget		buildFieldForm(Widget parent);

    // Creates a widget for editing one field
    void		buildFieldWidget(Widget parent, FieldInfo *info);

    // Creates the widget for editing a multiple- or single-value field
    Widget		buildMultipleValueWidget(Widget parent,
						 FieldInfo *info);
    Widget		buildSingleValueWidget(Widget parent, FieldInfo *info);

    // Creates and returns a widget containing buttons
    Widget		buildButtonWidget(Widget parent);

    // Applies all changes made in editor to node it is editing
    void		apply();

    // Sets field editor based on the values in the node being edited.
    // If the passed field index is not -1 (the default), only the
    // value for the specified field is reverted.
    void		revert(int fieldIndex = -1);

    //
    // These all operate on one field of the node:
    //

    // Initializes given FieldInfo structure based on indexed field
    void		initInfo(FieldInfo *info, int i);

    // Sets up field editor from correct field in node being edited
    void		updateInfo(FieldInfo *info);

    // Makes field editor show default value for given field. Does NOT
    // change the node at all!
    void		showDefaultValue(FieldInfo *info);

    // Sets string in text widget to display field values. If it's a
    // multiple-value field, it also sets the scale and related strings.
    void		showFieldString(FieldInfo *info) const;

    // Updates scale value and display for multiple-value field based
    // on current editor settings (NOT what is in the node!)
    void		showScale(FieldInfo *info) const;

    // Changes scale value for field to given value
    void		changeScale(FieldInfo *info, int value);

    // Updates field value and flags in node from current editor settings
    void		updateNode(FieldInfo *info);

    // Fills in SbString with text containing field value
    void		getFieldString(FieldInfo *info,
				       SbString &string) const;

    // Fills in SbString with text containing current field value from editor
    void		getEditorString(FieldInfo *info,
					SbString &string) const;

    // Redefine this to delete the editor
    virtual void	windowCloseAction();

    // Updates the line-number widget whenever the text area changes
    // or is scrolled. The textChanged flag indicates why the line
    // number may be changing.
    void		updateLineNumbers(FieldInfo *info,
					  SbBool textChanged) const;

    //
    // Callbacks, etc.
    //

    // Callback for when the node we are editing changes
    static void		nodeSensorCB(void *data, SoSensor *sensor);

    // Callback for when a button is pressed or other action occurs
    static void		acceptButtonCB(Widget,	XtPointer, XtPointer);
    static void		applyButtonCB(Widget,	XtPointer, XtPointer);
    static void		revertButtonCB(Widget,	XtPointer, XtPointer);
    static void		cancelButtonCB(Widget,	XtPointer, XtPointer);
    static void		defaultButtonCB(Widget,	XtPointer, XtPointer);
    static void		textChangedCB(Widget,	XtPointer, XtPointer);

    // These are used only for multiple-value fields
    static void		scrollCB(Widget,	XtPointer, XtPointer);
    static void		lineCB(Widget,	XtPointer, XtPointer);

    // Reallocates fieldBuf and sets fieldBufSize
    static void		*reallocBuf(void *ptr, size_t newSize);

    // Callback for read errors that occur when a field's value is set
    static void		readErrorCB(const SoError *error, void *data);

    // Displays error message
    void		displayReadError();
};

#endif /* SDL_PORT */
