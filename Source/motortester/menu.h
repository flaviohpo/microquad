/**********************************************************/
// menu.h
// 
// Gerencia menus 
// - os menus sao ponteiros criados pela funcao menu_create
// - as funcoes recebem sempre um Handler que eh um ponteiro
// para um menu externo.
// - os menus podem ter valor, check e/ou barrinha
// - a funcao processa menu retorna o resultado de um comando 
// dado e faz as alteracoes necessarias no menu
//
/***********************************************************/

#include "shorttypes.h"

#define CHECKED                 23
#define UNCHECKED               22
#define STICK_DELAY             25

typedef enum{
    ACTION_NONE = 0,
    ACTION_EMERGENCY,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT
}ACTION;

typedef enum{
    STATE_IDLE = 0,
    STATE_WAIT_BACK,
    STATE_WAIT_SUBMENU_IN,
    STATE_WAIT_SUBMENU_OUT
}MENU_STATE;

typedef enum{
    RESP_NONE,
    RESP_SEL_MIN_LIMIT,
    RESP_SEL_MAX_LIMIT,
    RESP_MAX_VALUE,
    RESP_MIN_VALUE,
    RESP_CHECKED,
    RESP_UNCHECKED,
    RESP_DONE,
    RESP_SUBMENU_IN,
    RESP_SUBMENU_OUT,
    RESP_BUSY,
    RESP_EMERGENCY
}MENU_RESPONSE;

typedef struct{
    // characteristics
    ListItems *oListItems;
    uint16 SelectedItem;
    const uchar* JanelaSize;
    uint16 FirstShowed;
    // attributes
    const uchar* BarsPositionH;
    const uchar* ValuePositionH;
    const uchar* ArrowPositionH;
    const uchar* BarsLenght;
    // states
    MENU_STATE eMenuState;
}MENU;

void menu_draw(MENU* menu, uchar clear); // redesenha todo o menu
MENU* menu_create(const char *tittle, Item *FirstItem, const uchar *JanelaSize, const uchar *barsposh, const uchar *valueposh,  const uchar *arrowposh,  const uchar *barslen);
uchar menu_add_item(MENU* menu, Item *it);
void menu_refresh(MENU* menu);
MENU_RESPONSE menu_process(MENU* oMenu, ACTION eAct);
