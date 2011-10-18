#include "menu.h"
#include "lcd6100.h"

MENU Menus[MAX_MENUS];

char MenuCreated = 0;

int ItemsPerPage = 0;

MENU_ACTION MENU_process(MENU *MenuHandler, MENU_BUTTON PressedButton){
    if((*menu_handler).action == NOTHING){
      int i, j;
      char cAux[__MENU_NAME_LENGHT];
      switch ((*menu_handler).step){
        case SHOW_MENU:
        
          for(i=0; i < (*menu_handler).ItemCount; i++){
            if(i == (*menu_handler).SelectedItem){
              // posiciona a setinha ali na frente do item
              lcd_goto((*menu_handler).HorizontalOffset, (*menu_handler).VerticalOffset + (*menu_handler).SelectedItem + 2);
              lcd_set_colour(RED, BLACK);
              printf(">");
              lcd_set_colour(LIME, BLACK);
            }
            else{
                lcd_goto((*menu_handler).HorizontalOffset + 1, (*menu_handler).VerticalOffset + i + 2);
                printf(" ");
            }
            
            for(j = 0; j < __MENU_NAME_LENGHT; j++){
              cAux[j]=(*menu_handler).items[i * 10 + j];
            }
            // escreve o nome do item
          }
          (*menu_handler).step = SELECT;
        break;
        
        case SELECT:
            switch (PressedButton){
                case MENU_BUTTON_UP:
                    if((*menu_handler).selected > 0){
                       (*menu_handler).selected--;
                       (*menu_handler).step = WAIT_BACK;
                    }
                    break;
                    
                case MENU_BUTTON_DOWN:
                    if((*menu_handler).selected < (*menu_handler).qty-1){
                        (*menu_handler).selected++;
                        (*menu_handler).step = WAIT_BACK;
                    }
                    break;
                    
                case MENU_BUTTON_LEFT:
                    (*menu_handler).action = DECREMENT;
                    (*menu_handler).step = WAIT_BACK;
                    break;
                    
                case MENU_BUTTON_RIGHT:
                    (*menu_handler).action = INCREMENT;
                    (*menu_handler).step = WAIT_BACK;
                    break;
     
                case MENU_BUTTON_SEL:
                    // apaga as titulo e opcoes do menu atual pra nao ficar por baixo do novo
                   (*menu_handler).doneFlag = 1;
                   (*menu_handler).step = WAIT_BACK;
                    break;
            }
            
        break;
        
        case WAIT_BACK:
          if(PressedButton == MENU_BUTTON_NONE){
            (*menu_handler).step =  SHOW_MENU;
            if((*menu_handler).doneFlag == 1){
               (*menu_handler).action = DONE;
               (*menu_handler).doneFlag = 0;
            }
          }
        break;
      }
    }
    return (*menu_handler).action;
}

