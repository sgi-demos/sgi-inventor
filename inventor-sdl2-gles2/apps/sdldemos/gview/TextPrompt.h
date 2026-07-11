//  -*- C++ -*-

//
// TextPrompt: minimal line-input helper for the SDL port of gview.
// Replaces the Motif text dialogs (FieldEditor, NodeCreator): one
// prompt is active at a time, fed by SDL text-input events routed
// through GraphViewer::processEvent, and echoed to the terminal.
//
// Only exists in the SDL port.
//

#ifndef GVIEW_TEXT_PROMPT_H
#define GVIEW_TEXT_PROMPT_H

#include <SDL2/SDL.h>

class TextPrompt {

  public:
    // Called with the completed line when the user hits Enter. The
    // callback decides when the prompt is finished (by calling
    // deactivate()); otherwise the prompt stays up for another line.
    typedef void	LineCB(void *userData, const char *line);

    // Called when the user hits Escape.
    typedef void	CancelCB(void *userData);

    // Starts a prompt. Only one can be active; a second activate()
    // replaces the first.
    static void		activate(const char *label, LineCB *onLine,
				 CancelCB *onCancel, void *userData);

    // Ends the prompt (normally called from within the callbacks).
    static void		deactivate();

    static bool		isActive()	{ return active; }

    // Feeds one SDL event to the prompt. Returns true if the event was
    // consumed (all keyboard events are, while a prompt is active, so
    // single-key shortcuts don't fire while the user types).
    static bool		handleEvent(const SDL_Event *event);

  private:
    static bool		active;
    static char		buf[1024];
    static int		len;
    static const char	*label;
    static LineCB	*lineCB;
    static CancelCB	*cancelCB;
    static void		*userData;

    // Reprints the prompt line on the terminal
    static void		echo();
};

#endif /* GVIEW_TEXT_PROMPT_H */
