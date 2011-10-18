#ifndef __MENU
#define __MENU

#define __MENU_MENU_LIMIT 10
#define __MENU_NAME_LENGHT 10

typedef enum{
    MENU_STEP_SHOW_MENU = 0,
    MENU_STEP_SELECT,
    MENU_STEP_WAIT_BACK
}MENU_STEP;

typedef enum{
    MENU_BUTTON_ENTER = 0x01,
    MENU_BUTTON_CANCEL = 0x02,
    MENU_BUTTON_RIGHT = 0x04,
    MENU_BUTTON_LEFT = 0x08,
    MENU_BUTTON_UP = 0x10,
    MENU_BUTTON_DOWN = 0x20,
    MENU_BUTTON_NONE = 0x00
}MENU_BUTTON;

typedef enum{
  MENU_ACTION_NOTHING = 0,
  MENU_ACTION_DONE,
  MENU_ACTION_SELECT,
  MENU_ACTION_INCREMENT,
  MENU_ACTION_DECREMENT
}MENU_ACTION;

typedef enum{
    MENU_ARRANGE_RING = 0,
    MENU_ARRANGE_VERTICAL,
    MENU_ARRANGE_ONE_PER_PAGE
}MENU_ARRANGE;

typedef struct{
    char *Tittle;
    char *Items;
    char DoneFlag;
    int CurrentPage;
    int SelectedItem;
    int ItemCount;
    int VerticalLimit;
    int VerticalOffset;
    int HorizontalLimit;
    int HorizontalOffset;
    MENU_STEP Step;
    MENU_ACTION Action;
    MENU_ARRANGE Arrangement;
}MENU;

extern MENU Menus[__MENU_MENU_LIMIT];

void MENU_create();
void MENU_set_items(char* _Items);

MENU_ACTION MENU_process(MENU *MenuHandler);

#endif //__MENU

