#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "delay.h"
#include "color.h"
#include "lcd6100.h"
#include "shorttypes.h"
#include "usefullibs.h"
#include "menu.h"
    
#define SETABAIXO   25
#define SETACIMA    24
#define SETALEFT    27
#define SETARIGHT   26

// remark: arrowpositionH must never be NULL or ZERO.
// se o menu contem items diferentes de SUBMENUS, nenhum parametro deve ser NULL.
MENU* menu_create(const char *tittle, Item *FirstItem,  const uchar *janelasize,  const uchar *barsposh,  const uchar *valueposh,  const uchar *arrowposh,  const uchar *barslen){
    MENU* oMenu = (MENU*)malloc(sizeof(MENU));
    if(oMenu != NULL){
        oMenu->oListItems = list_items_create(FirstItem, tittle);
        oMenu->JanelaSize = janelasize;
        oMenu->BarsPositionH = barsposh;
        oMenu->ValuePositionH = valueposh;
        oMenu->ArrowPositionH = arrowposh;
        oMenu->BarsLenght = barslen;
        oMenu->SelectedItem = 0;
        oMenu->FirstShowed = 0;
        oMenu->eMenuState = STATE_IDLE;
    }
    return oMenu;
}

uchar menu_add_item(MENU* oMenu, Item *it){
    unsigned char result = list_items_insert(oMenu->oListItems, it, oMenu->oListItems->Size);
    return result;
}

// remark: menus que contem somente SUBMENUS nao dever rodar esta rotina
void menu_refresh(MENU* oMenu){
    
    int i = oMenu->FirstShowed;
    unsigned int valor;
    int min, max, d, p;
    
    Item* oItem;
    
    // varre todos os items visiveis
    while((i < (oMenu->FirstShowed + *(oMenu->JanelaSize))) && (i < oMenu->oListItems->Size)){
        
        oItem = list_items_get_item(oMenu->oListItems,i);
        // se possuir barra
        if((oItem->ItemType & MASK_READ) && (oItem->ItemType & MASK_BAR)){
            min = *(oItem->MinVal);
            max = *(oItem->MaxVal);
            d = (max-min)/100;
            p = (*(oItem->Value)-*(oItem->MinVal))/d;
            if(p > 100){
                p = 100;
            }
            lcd_drawprogressbar(*(oMenu->BarsPositionH), 20+(8*(i-oMenu->FirstShowed)), *(oMenu->BarsLenght), 4, RED, BLACK, p);
        }
        else{ // se nao apaga o lugar da barra
            lcd_fillrect(*(oMenu->BarsPositionH),20+(8*(i-oMenu->FirstShowed)), *(oMenu->BarsLenght), 4, LCDBackColor);
        }

        lcd_goto(*(oMenu->ValuePositionH), 2+i-oMenu->FirstShowed); // move para posicao di valor
        if(((oItem->ItemType) & MASK_READ) != 0){ // if is value or bool readable
        //if(1){
            if(((oItem->ItemType) & MASK_VALUE) != 0){ // se for um valor
            //if(1){
                // valor
                printf("%d",*(oItem->Value));
                valor = *(oItem->Value);                
                
                printf("   ");
            }
            else{   // se nao for valor
                if((oItem->ItemType & MASK_BOOL) == MASK_BOOL){ // se for um boolean
                    printf("%c    ",*(oItem->Value));
                }
                else{ // se for submenu so apaga
                    printf("     ");
                }
            }
        }
        i++;
    }
}

void menu_draw(MENU* oMenu, uchar clear){
    uint16 i = 0;
    
    uchar SetaCima = 1;
    uchar SetaBaixo = 1;
    
    if(clear){
        lcd_clear(LCDBackColor);
        lcd_goto(0,0);
        printf(oMenu->oListItems->Label);
    }
    
    // coloca desde o inicio ate o tamanho de 2*Janela+1
    if(oMenu->SelectedItem < oMenu->FirstShowed && oMenu->FirstShowed > 0){
        oMenu->FirstShowed = oMenu->SelectedItem;
    }
    // verifica se o selected item ta fora da janela
    if(oMenu->SelectedItem >= (oMenu->FirstShowed + *(oMenu->JanelaSize)) && oMenu->SelectedItem < oMenu->oListItems->Size){
        oMenu->FirstShowed = oMenu->SelectedItem - *(oMenu->JanelaSize) + 1;
    }
    // config seta cima
    if(oMenu->FirstShowed == 0){
        SetaCima = 0;    
    }
    // config seta baixo
    if((*(oMenu->JanelaSize) + oMenu->FirstShowed) >= oMenu->oListItems->Size){
        SetaBaixo = 0;
    }
    
    // anda do i = primeiro mostrado (0) ate o ultimo da janela
    for(i = oMenu->FirstShowed; i < oMenu->FirstShowed + *(oMenu->JanelaSize); i++){
        if(i < oMenu->oListItems->Size){
            lcd_goto(0, 2 + i - oMenu->FirstShowed);
            if(i == oMenu->SelectedItem){
                printf("%c ",SETARIGHT);
            }
            else{
                printf("  ");    
            }
            // mostra o label do item
            printf(list_items_get_node(oMenu->oListItems, i)->Value->Label);
            LCDLineCount++;
        }
    }

    lcd_goto(0,LCD_NLINE - 1);
    printf("%d/%d  ",oMenu->SelectedItem + 1, oMenu->oListItems->Size);
    
    lcd_goto(*(oMenu->ArrowPositionH), 1);
    if(SetaCima){
        printf("%c",SETACIMA);
    }
    else{
        printf(" ");
    }
    
    // BUG: desenhou uma seta ao lado direito do MenuTittle quand era pra ta la em baixo
    // desenha NADA na mesma posicao quando nao deve desenhar seta
    // ha! ja sei, eh porque a continha da maior do q era pra dar
    lcd_goto(*(oMenu->ArrowPositionH), LCD_NLINE - 1);
    if(SetaBaixo){
        printf("%c",SETABAIXO);
    }
    else{
        printf(" ");
    }
    
    lcd_goto(LCD_NCHAR - 1, LCD_NLINE - 1);
    printf("%d",(int)oMenu->eMenuState);
    
}

MENU_RESPONSE menu_process(MENU* oMenu, ACTION act){
    Item* oItem = list_items_get_item(oMenu->oListItems, oMenu->SelectedItem);
    MENU_RESPONSE result = RESP_NONE;
    switch(oMenu->eMenuState){   
        case STATE_IDLE:
            switch(act){
                case ACTION_NONE:
                break;
                
                case ACTION_EMERGENCY:
                    result =  RESP_EMERGENCY;                  
                break;
                
                case ACTION_UP:
                    set_delay(DELAY_STICK_INDEX, STICK_DELAY);
                    oMenu->eMenuState = STATE_WAIT_BACK;
                    if(oMenu->SelectedItem > 0){
                        (oMenu->SelectedItem)--;
                        result = RESP_NONE;
                    }
                    else{
                        result = RESP_SEL_MIN_LIMIT;
                    }
                break;
                
                case ACTION_DOWN:
                    set_delay(DELAY_STICK_INDEX, STICK_DELAY);
                    oMenu->eMenuState = STATE_WAIT_BACK;
                    if(oMenu->SelectedItem < oMenu->oListItems->Size - 1){
                        (oMenu->SelectedItem)++;
                        result = RESP_DONE;
                    }
                    else{
                        result = RESP_SEL_MAX_LIMIT;
                    }
                break;
        
                case ACTION_LEFT:
                    set_delay(DELAY_STICK_INDEX, 50);
                    oMenu->eMenuState = STATE_WAIT_BACK;
                    if((oItem->ItemType == ITEMTYPE_VALUE_RW) || (oItem->ItemType == ITEMTYPE_BAR_RW) || (oItem->ItemType == ITEMTYPE_VALUE_BAR_RW)){
                        if(*(oItem->Value) - *(oItem->Interval) >= *(oItem->MinVal)){
                            *(oItem->Value) = *(oItem->Value) - *(oItem->Interval);
                            result = RESP_DONE;
                        }
                        else{
                            result = RESP_MIN_VALUE;
                        }
                    }
                    else{
                        if(oItem->ItemType == ITEMTYPE_BOOLEAN_RW){
                            *(oItem->Value) = UNCHECKED;
                            result = RESP_DONE;
                        }
                        else{ // submenu ou item not readable
                            oMenu->eMenuState = STATE_WAIT_SUBMENU_OUT;
                            result = RESP_BUSY;
                        }
                    }
                break;
                
                case ACTION_RIGHT:
                    set_delay(DELAY_STICK_INDEX, 50);
                    oMenu->eMenuState = STATE_WAIT_BACK;
                    if((oItem->ItemType == ITEMTYPE_VALUE_RW) || (oItem->ItemType == ITEMTYPE_BAR_RW) || (oItem->ItemType == ITEMTYPE_VALUE_BAR_RW)){
                        if(*(oItem->Value) + *(oItem->Interval) <= *(oItem->MaxVal)){
                            *(oItem->Value) = *(oItem->Value) + *(oItem->Interval);
                            result = RESP_DONE;
                        }
                        else{
                            result = RESP_MAX_VALUE;
                        }
                    }
                    else{
                        if(oItem->ItemType == ITEMTYPE_BOOLEAN_RW){
                            *(oItem->Value) = CHECKED;
                            result = RESP_DONE;
                        }
                        else{ // submenu ou readabble value
                            oMenu->eMenuState = STATE_WAIT_SUBMENU_IN;
                            result = RESP_BUSY;
                        }
                    }
                break;
            }        
                    
        break;
        
        case STATE_WAIT_BACK:
            if((act == ACTION_NONE) || (get_delay(DELAY_STICK_INDEX))){
                oMenu->eMenuState = STATE_IDLE;
                result = RESP_NONE;
            }
            else{
                result = RESP_BUSY;
            }
        break;
        
        case STATE_WAIT_SUBMENU_IN:
            if(act == ACTION_NONE){
                oMenu->eMenuState = STATE_IDLE;
                result = RESP_SUBMENU_IN;
            }
            else{
                result = RESP_BUSY;
            }
        break;
        
        case STATE_WAIT_SUBMENU_OUT:
            if(act == ACTION_NONE){
                oMenu->eMenuState = STATE_IDLE;
                result = RESP_SUBMENU_OUT;
            }
            else{
                result = RESP_BUSY;
            }
        break;
    }
    return result;
}
