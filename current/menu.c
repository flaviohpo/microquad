#include "stdio.h"
#include "stdlib.h"
#include "lcd6100.h"
#include "shorttypes.h"
#include "usefullibs.h"
#include "menu.h"
    
MENU* menu_create(char *tittle, Item *FirstItem, uchar janelasize, char autorefresh, 
                    uchar barsposh, uchar valueposh, uchar arrowposh, uchar barslen){
    MENU* oMenu = (MENU*)malloc(sizeof(MENU));
    if(oMenu != NULL){
        oMenu->oListItems = list_items_create(FirstItem, tittle);
        oMenu->JanelaSize = janelasize;
        oMenu->AutoRefresh = autorefresh;
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

void menu_refresh(MENU* menu){
    
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
                printf("> ");
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
        printf("/\\");
    }
    else{
        printf("  ");
    }
    
    lcd_goto(oMenu->ArrowPositionH, 2 * oMenu->JanelaSize + 3);
    if(SetaBaixo){
        printf("\\/");
    }
    else{
        printf("  ");
    }
}

uint16 menu_process(ACTION act){
    
    
    
    return 1;
}
