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
    uint16 LimiteMin = oMenu->JanelaSize;
    uint16 LimiteMax = oMenu->oListItems->Size - 1 - oMenu->JanelaSize;
    
    uchar SetaCima = 1;
    uchar SetaBaixo = 1;
    
    if(clear){
        lcd_clear(LCDBackColor);
        lcd_goto(0,0);
        printf(oMenu->oListItems->Label);
    }
    
    if(oMenu->SelectedItem < LimiteMin){ // se utrapassou o limite inferior da janela
        SetaCima = 0;
        // coloca desde o inicio ate o tamanho de 2*Janela+1
        for(i = 0; i < oMenu->JanelaSize * 2 + 2; i++){
            if(i < oMenu->oListItems->Size){
                lcd_goto(0, 2 + i);
                if(i == oMenu->SelectedItem){
                    printf("> ");
                }
                else{
                    printf("  ");    
                }
                printf(list_items_get_node(oMenu->oListItems, i)->Value->Label);
                LCDLineCount++;
            }
            else{
                SetaBaixo = 0;
            }
        }
    }
    else{
        SetaBaixo = 0;
        if(oMenu->SelectedItem > LimiteMax){
            // coloca a parte do final
            if(i < oMenu->oListItems->Size){ // precaucao
                for(i = oMenu->oListItems->Size - oMenu->JanelaSize * 2 - 2; i < oMenu->oListItems->Size; i++){
                    lcd_goto(0, (i - oMenu->oListItems->Size - oMenu->JanelaSize * 2));
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
        }
        else{ // significa que o item selecionado esta bem no meio da janela
            // coloca o selected no meio, inicia em selected-janela e vai ate selected+janela
            for(i = oMenu->SelectedItem - oMenu->JanelaSize; i < oMenu->SelectedItem - oMenu->JanelaSize + 1; i++){
                if(i < oMenu->oListItems->Size){            
                    lcd_goto(0, i - oMenu->oListItems->Size - oMenu->JanelaSize * 2);
                    if(i == oMenu->SelectedItem){
                        printf("> ");
                    }
                    else{
                        printf("  ");    
                    }
                    printf(list_items_get_node(oMenu->oListItems, i)->Value->Label);
                    LCDLineCount++;
                }
                else{
                    SetaBaixo = 0;
                }
            }
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
