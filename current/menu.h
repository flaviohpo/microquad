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

#define CHECKED     23
#define UNCHECKED   22

typedef enum{
    ACTION_NONE = 0,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT
}ACTION;

typedef enum{
    STATE_IDLE,
    STATE_WAIT_UP,
    STATE_WAIT_DOWN,
    STATE_WAIT_LEFT,
    STATE_WAIT_RIGHT
}MENU_STATE;

typedef enum{
    RESP_NONE,
    RESP_SEL_MIN_LIMIT,
    RESP_SEL_MAX_LIMIT,
    RESP_MAX_VALUE,
    RESP_MIN_VALUE,
    RESP_ENTERED,
    RESP_RETURNED,
    RESP_CHECKED,
    RESP_UNCHECKED,
    RESP_DONE,
    RESP_SUBMENU_IN,
    RESP_SUBMENU_OUT,
    RESP_ITEMTYPE_FAIL,
    RESP_BUSY
}MENU_RESPONSE;

typedef struct{
    // characteristics
    ListItems *oListItems;
    uint16 SelectedItem;
    uchar JanelaSize;
    uint16 FirstShowed;
    // attributes
    uchar BarsPositionH;
    uchar ValuePositionH;
    uchar CheckPositionH;
    uchar ArrowPositionH;
    uchar BarsLenght;
    // states
    MENU_STATE eMenuState;
}MENU;

void draw_menu(MENU* menu, uchar clear); // redesenha todo o menu
MENU* menu_create(char *tittle, Item *FirstItem, uchar JanelaSize, uchar barsposh, uchar valueposh, uchar arrowposh, uchar barslen);
uchar menu_add_item(MENU* menu, Item *it);
void menu_refresh(MENU* menu);
uint16 menu_process(MENU* oMenu, ACTION eAct);



