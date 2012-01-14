/**********************************************************
 * menu.h
 * 
 * Gerencia menus 
 * - os menus sao ponteiros criados pela funcao menu_create
 * - as funcoes recebem sempre um Handler que eh um ponteiro
 * para um menu externo.
 * - os menus podem ter valor, check e/ou barrinha
 * - a funcao processa menu retorna o resultado de um comando 
 * dado e faz as alteracoes necessarias no menu
 *
 * REVIEW LOG
 * 
 * 31/12/2011, Flaviohpo:
 *      Added:
 *          1. unsigned char SETABAIXO = 25;
 *          2. unsigned char SETACIMA  = 24;
 *          3. unsigned char SETALEFT  = 27;
 *          4. unsigned char SETARIGHT = 26;
 *          5. unsigned char CHECKED   = 23;
 *          6. unsigned char UNCHECKED = 22;
 *          7. void menu_set_special_characters(...);
 *          8. function pointer para poll_delay(...)
 *          9. function pointer para set_delay(...)
 *          10. function pointer para lcd_drawprogressbar 
 *          11. function pointer para lcd_fill00ct
 *          12. function pointer para lcd_clear
 *          13. function pointer para lcd_goto
 *          13. function pointer para 'int printf ( const char * format, ... )' no LCD
 *          unsigned char LCDBackColor;
 *          unsigned char LCDForeColor;
 *          unsigned char LCDLineCount;
 *          unsigned char LCDCharCount;
 *          unsigned char LCD_NLINE;
 *      Removed:    
 *          1. delay.h
 *          2. lcd6100.h
 *          3. #define SETABAIXO               25
 *          4. #define SETACIMA                24
 *          5. #define SETALEFT                27
 *          6. #define SETARIGHT               26
 *          7. #define CHECKED                 23
 *          8. #define UNCHECKED               22
 *      1. As funcoes que alteram a tela mudaram para ponteiros de
 *          funcoes que devem ser passados pelo usuario na rotina
 *          de inicializacao.
 *
 *
 *
 *****************************(C) by Flaviohpo 09/2011****/

// desenhos da tabela de characteres do lcd utilizado
// deve ser modificado pelo usuario
#define SETABAIXO               25
#define SETACIMA                24
#define SETALEFT                27
#define SETARIGHT               26
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
