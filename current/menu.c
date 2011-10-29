#include "stdio.h"
#include "delay.h"
#include "stdlib.h"
#include "color.h"
#include "lcd6100.h"
#include "shorttypes.h"
#include "usefullibs.h"
#include "menu.h"
    
#define SETABAIXO   25
#define SETACIMA    24
#define SETALEFT    27
#define SETARIGHT   26

MENU* menu_create(char *tittle, Item *FirstItem, uchar janelasize, uchar barsposh, uchar valueposh, uchar arrowposh, uchar barslen){
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
    }
    return oMenu;
}

uchar menu_add_item(MENU* oMenu, Item *it){
    unsigned char result = list_items_insert(oMenu->oListItems, it, oMenu->oListItems->Size);
    return result;
}

void menu_refresh(MENU* oMenu){
    int i = oMenu->FirstShowed;
    unsigned int barPercent = 0;
    Item* oItem;
    while(i < (oMenu->FirstShowed + oMenu->JanelaSize) && i < oMenu->oListItems->Size){
        oItem = list_items_get_item(oMenu->oListItems,i);
        // bar
        if((oItem->ItemType & MASK_READ) && (oItem->ItemType & MASK_BAR)){
            barPercent = ((((*(oItem->Value) - *(oItem->MinVal)) * 10) / (*(oItem->MaxVal) - *(oItem->MinVal))) * 10);
            lcd_drawprogressbar(oMenu->BarsPositionH, 20+(8*(i-oMenu->FirstShowed)), oMenu->ValuePositionH*6 - oMenu->BarsPositionH, 4, RED, BLACK, 100-barPercent);
        }
        else{
            lcd_fillrect(oMenu->BarsPositionH,20+(8*(i-oMenu->FirstShowed)), oMenu->ValuePositionH*6 - oMenu->BarsPositionH, 4, LCDBackColor);
        }
        lcd_goto(oMenu->ValuePositionH, 2+i-oMenu->FirstShowed);
        if(oItem->ItemType & MASK_READ){ // if is value or bool readable
            if(oItem->ItemType & MASK_VALUE){
                // valor
                printf("%d    ",*(oItem->Value));
            }
            else{
                if(oItem->ItemType & MASK_BOOL){
                    printf("%c",*(oItem->Value));
                }
            }
        }
        else{ // so sobrou submenu
            printf("     ");
        }
        i++;
    }
}

void draw_menu(MENU* oMenu, uchar clear){
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
    if(oMenu->SelectedItem >= (oMenu->FirstShowed + oMenu->JanelaSize) && oMenu->SelectedItem < oMenu->oListItems->Size){
        oMenu->FirstShowed = oMenu->SelectedItem - oMenu->JanelaSize + 1;
    }
    if(oMenu->FirstShowed == 0){
        SetaCima = 0;    
    }
    if((oMenu->JanelaSize + oMenu->FirstShowed) >= oMenu->oListItems->Size){
        SetaBaixo = 0;
    }
    for(i = oMenu->FirstShowed; i < oMenu->FirstShowed + oMenu->JanelaSize; i++){
        if(i < oMenu->oListItems->Size){
            lcd_goto(0, 2 + i - oMenu->FirstShowed);
            if(i == oMenu->SelectedItem){
                printf("%c",SETARIGHT);
                printf(" ");
            }
            else{
                printf("  ");    
            }
            printf(list_items_get_node(oMenu->oListItems, i)->Value->Label);
            LCDLineCount++;
        }

    }

    lcd_goto(oMenu->ArrowPositionH, 1);
    if(SetaCima){
        printf("%c",SETACIMA);
    }
    else{
        printf(" ");
    }
    
    lcd_goto(oMenu->ArrowPositionH, 2 * oMenu->JanelaSize + 3);
    if(SetaBaixo){
        printf("%c",SETABAIXO);
    }
    else{
        printf(" ");
    }
}

MENU_RESPONSE menu_process(MENU* oMenu, ACTION act){
    Item* oItem = list_items_get_item(oMenu->oListItems, oMenu->SelectedItem);
    MENU_RESPONSE result = RESP_NONE;
    switch(oMenu->eMenuState){   
        case STATE_IDLE:
            switch(act){
                case ACTION_NONE:
                
                break;
                
                case ACTION_UP:
                    oMenu->eMenuState = STATE_WAIT_DOWN;
                    if(oMenu->SelectedItem > 0){
                        (oMenu->SelectedItem)--;
                        result = RESP_NONE;
                    }
                    else{
                        result = RESP_SEL_MIN_LIMIT;
                    }
                break;
                
                case ACTION_DOWN:
                    oMenu->eMenuState = STATE_WAIT_UP;
                    if(oMenu->SelectedItem < oMenu->oListItems->Size){
                        (oMenu->SelectedItem)++;
                        result = RESP_DONE;
                    }
                    else{
                        result = RESP_SEL_MAX_LIMIT;
                    }
                break;
        
                case ACTION_LEFT:
                    oMenu->eMenuState = STATE_WAIT_RIGHT;
                    if(oItem->ItemType & (MASK_VALUE | MASK_BAR) & MASK_WRITE){
                        if(*(oItem->Value - *(oItem->Interval)) > (*(oItem->MinVal))){
                            *(oItem->Value) = *(oItem->Value) - *(oItem->Interval);
                            result = RESP_DONE;
                        }
                        else{
                            result = RESP_MIN_VALUE;
                        }
                    }
                    else{
                        if(oItem->ItemType & MASK_BOOL & MASK_WRITE){
                            *(oItem->Value) = UNCHECKED;
                            result = RESP_DONE;
                        }
                        else{ // submenu
                            result = RESP_SUBMENU_OUT;
                        }
                    }
                break;
                
                case ACTION_RIGHT:
                    oMenu->eMenuState = STATE_WAIT_LEFT;
                    if(oItem->ItemType & (MASK_VALUE | MASK_BAR) & MASK_WRITE){
                        if(*(oItem->Value + *(oItem->Interval)) < (*(oItem->MaxVal))){
                            *(oItem->Value) = *(oItem->Value) + *(oItem->Interval);
                            result = RESP_DONE;
                        }
                        else{
                            result = RESP_MAX_VALUE;
                        }
                    }
                    else{
                        if(oItem->ItemType & MASK_BOOL & MASK_WRITE){
                            *(oItem->Value) = CHECKED;
                            result = RESP_DONE;
                        }
                        else{ // submenu
                            result = RESP_SUBMENU_IN;
                        }
                    }
                break;
            }        
                    
        break;
        
        case STATE_WAIT_UP:
            if(act == ACTION_UP){
                oMenu->eMenuState = STATE_IDLE;
                result = RESP_NONE;
            }
            else{
                result = RESP_BUSY;
            }
        break;
        
        case STATE_WAIT_DOWN:
            if(act == ACTION_DOWN){
                oMenu->eMenuState = STATE_IDLE;
                result = RESP_NONE;
            }
            else{
                result = RESP_BUSY;
            }
        break;
        
        case STATE_WAIT_LEFT:
            if(act == ACTION_LEFT){
                oMenu->eMenuState = STATE_IDLE;
                result = RESP_NONE;
            }
            else{
                result = RESP_BUSY;
            }
        break;
        
        case STATE_WAIT_RIGHT:
            if(act == ACTION_RIGHT){
                oMenu->eMenuState = STATE_IDLE;
                result = RESP_NONE;
            }
            else{
                result = RESP_BUSY;
            }
        break;
    }
    return 1;
}
