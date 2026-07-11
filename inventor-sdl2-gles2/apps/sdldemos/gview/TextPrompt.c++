//
// TextPrompt: minimal line-input helper for the SDL port of gview.
// See TextPrompt.h.
//

#include <cstdio>
#include <cstring>

#include "TextPrompt.h"

bool		 TextPrompt::active = false;
char		 TextPrompt::buf[1024];
int		 TextPrompt::len = 0;
const char	*TextPrompt::label = NULL;
TextPrompt::LineCB   *TextPrompt::lineCB = NULL;
TextPrompt::CancelCB *TextPrompt::cancelCB = NULL;
void		*TextPrompt::userData = NULL;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Starts a prompt.
//

void
TextPrompt::activate(const char *newLabel, LineCB *onLine,
		     CancelCB *onCancel, void *newUserData)
//
////////////////////////////////////////////////////////////////////////
{
    active   = true;
    label    = newLabel;
    lineCB   = onLine;
    cancelCB = onCancel;
    userData = newUserData;
    len      = 0;
    buf[0]   = '\0';

    SDL_StartTextInput();

    echo();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Ends the prompt.
//

void
TextPrompt::deactivate()
//
////////////////////////////////////////////////////////////////////////
{
    if (! active)
	return;

    active = false;
    lineCB = NULL;
    cancelCB = NULL;
    userData = NULL;

    SDL_StopTextInput();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Feeds one SDL event to the prompt. While a prompt is active all
//    keyboard events are consumed.
//

bool
TextPrompt::handleEvent(const SDL_Event *event)
//
////////////////////////////////////////////////////////////////////////
{
    if (! active)
	return false;

    switch (event->type) {

      case SDL_TEXTINPUT: {
	int n = (int) strlen(event->text.text);
	if (len + n < (int) sizeof(buf) - 1) {
	    memcpy(buf + len, event->text.text, n + 1);
	    len += n;
	}
	echo();
	return true;
      }

      case SDL_KEYDOWN:
	switch (event->key.keysym.sym) {

	  case SDLK_RETURN:
	  case SDLK_KP_ENTER: {
	    printf("\n");
	    // The callback may deactivate the prompt or print output;
	    // hand it a stable copy and reset the buffer first.
	    char line[sizeof(buf)];
	    strcpy(line, buf);
	    len = 0;
	    buf[0] = '\0';
	    LineCB *cb = lineCB;
	    void *ud = userData;
	    if (cb != NULL)
		cb(ud, line);
	    if (active)
		echo();
	    return true;
	  }

	  case SDLK_BACKSPACE:
	    if (len > 0)
		buf[--len] = '\0';
	    echo();
	    return true;

	  case SDLK_ESCAPE: {
	    printf("\n");
	    CancelCB *cb = cancelCB;
	    void *ud = userData;
	    if (cb != NULL)
		cb(ud);
	    return true;
	  }

	  default:
	    // Swallow all other key presses so single-key shortcuts
	    // don't fire while the user is typing
	    return true;
	}

      case SDL_KEYUP:
	return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reprints the prompt line on the terminal. The trailing space-
//    backspace pair erases the character removed by a backspace.
//

void
TextPrompt::echo()
//
////////////////////////////////////////////////////////////////////////
{
    printf("\r%s %s \b", label, buf);
    fflush(stdout);
}
