#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/FileSB.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#define MENUS_IN_POPUP

enum MenuEntries {
    MM_FILE = 0,
    MM_FILE_NEW,
    MM_FILE_OPEN,
    MM_FILE_SAVE,
    MM_FILE_SAVE_AS,
    MM_FILE_QUIT,

    MM_EDIT,
    MM_EDIT_DELETE,
    MM_EDIT_UNDELETE,
    MM_EDIT_SEP,

    MM_OPTIONS,
    MM_OPTIONS_SHADOW,
    MM_OPTIONS_SOUND,
    MM_OPTIONS_TABLE,
    MM_OPTIONS_TRANSPARENCY,

    MM_HELP,
    MM_HELP_HELP,

    MM_MENU_NUM		// this must be the last entry
};

enum ButtonTypes {
    MM_SEPARATOR,
    MM_PUSH_BUTTON,
    MM_TOGGLE_BUTTON,
    MM_RADIO_BUTTON,
};

struct mmButton {
    char *name;
    int   id;
    int   buttonType;  // PUSH, TOGGLE, RADIO
    SbBool isOn;       // relevant for toggles.
    char *accelerator; // e.g. "Alt <Key> p"
    char *accelText;   // text that appears in the menu item
};

struct mmMenuItem {
    int    id;
    Widget widget;
};

struct mmMenu {
    char  *name;
    int    id;
    struct mmButton *subMenu;
    int    subItemCount;
};

static mmButton fileData[] = {
   {"New...",     MM_FILE_NEW,     MM_PUSH_BUTTON, 0, "Alt <Key> n", "Alt+n" },
   {"Open...",    MM_FILE_OPEN,    MM_PUSH_BUTTON, 0, "Alt <Key> o", "Alt+o" },
   {"Save",       MM_FILE_SAVE,    MM_PUSH_BUTTON, 0, "Alt <Key> s", "Alt+s" },
   {"Save As...", MM_FILE_SAVE_AS, MM_PUSH_BUTTON, 0, "Alt <Key> a", "Alt+a" },
   {"Quit",       MM_FILE_QUIT,    MM_PUSH_BUTTON, 0, "Alt <Key> q", "Alt+q" },
};

static mmButton editData[] = {
   {"Delete",     MM_EDIT_DELETE,   MM_PUSH_BUTTON, 0, "Alt <Key> d", "Alt+d" },
   {"Undelete",   MM_EDIT_UNDELETE, MM_PUSH_BUTTON, 0, "Alt <Key> u", "Alt+u" },
   {"",           MM_EDIT_SEP, 	    MM_SEPARATOR,   0, 0, 0 },
};

static mmButton optionsData[] = {
   {"Shadows", MM_OPTIONS_SHADOW, 
					MM_TOGGLE_BUTTON, 0, 0, 0 },
   {"Sound",   MM_OPTIONS_SOUND, 
					MM_TOGGLE_BUTTON, 1, 0, 0 },
   {"Table",   MM_OPTIONS_TABLE, 
					MM_TOGGLE_BUTTON, 1, 0, 0 },
   {"High quality transparency", MM_OPTIONS_TRANSPARENCY, 
					MM_TOGGLE_BUTTON, 0, 0, 0 },
};

static mmButton helpData[] = {
   {"Help...",   MM_HELP_HELP,   MM_PUSH_BUTTON, 0, "Alt <Key> h", "Alt+h" },
};

static mmMenu pulldownData[] = {
// {name, id, subMenu, subItemCount}
   {"File",  MM_FILE, fileData, XtNumber(fileData) },
   {"Edit",  MM_EDIT, editData, XtNumber(editData) },
   {"Options",  MM_OPTIONS, optionsData, XtNumber(optionsData) },
   {"Help",  MM_HELP, helpData, XtNumber(helpData) },
};

