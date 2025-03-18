#include<string>
 

//********************************** 
#//?Functions
int f_option;
void Mainmenu();
void Events();
void Check_Rgs();
void Clear_Rgs();
//********************************* *
//Global Variables
float registers[4]; //? How many registers?
int globalregisters=0;
//! navigation_boton=?

//********************************
#//!registers
#//* Height
float height_event1, height_event2;
float *ptr_event1, *ptr_event2;
*ptr_event_h1 = &height_event1;
*ptr_event_h2 = &height_event2;

#//* Time
float time_event1, time_event2;
float *ptr_event1, *ptr_event2;
*ptr_event_t1 = &time_event1;
*ptr_event_t2 = &time_event2;

int main(){

    #//!First Block
    while(1){
        Mainmenu();
        printf("->");
        scanf("%d",&f_option);
        if (f_option == 1 || f_option == 2 || f_option == 3){break;}
        else{printf("Invalid option");}   
    }

    #//!Second block (Selection)
    if (f_option == 1){Events();}
    if (f_option == 2){Chek_Rgs();}
    if (f_option == 2){Clear_Rgs();}
}

void MainMenu(){
        printf("--Menu--\n");
        printf("1. Trigger an event\n");
        printf("2. Check registers\n");
        printf("3. Clear registers\n");
}
void Events(){
    int type_event;
    
    while (1)
    {
    printf("Trigger events\n");
    printf("1. Height\n");
    printf("2. Time\n");
    printf("->");
    scanf("%d", &type_event);
    if (type_event == 1)
    {
        printf("Height\n");
        printf("Type event 1\n");
        printf("->");
        scanf("%f", *ptr_event_h1);
        printf("Type event 2\n");
        printf("->");
        scanf("%f", *ptr_event_h2);
        printf("Successful height event");
        break;
    }
    else if (type_event == 2)
    {
        printf("Time\n");
        printf("Type event 1\n");
        printf("->");
        scanf("%f", *ptr_event_t1);
        printf("Type event 2\n");
        printf("->");
        scanf("%f", *ptr_event_t2);
        printf("Successful time event");
        break; 
    }
    else{printf("Unknown event\n");}
    }
    
    
}

void Check_Rgs(){
    if (globalregisters =! 0)
    {   printf("register");
    }
    else
    {
    printf("There are not events yet\n");
    }

}
void Clear_Rgs(){
    if (globalregisters =! 0)
    {   printf("");
    }
    else
    {
    printf("There are not events yet\n");
    }
}