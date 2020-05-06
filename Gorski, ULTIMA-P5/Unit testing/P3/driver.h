/*
Filename: MCB.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To clean up main program as well as
            to implement some driver function for
            mechanism testing.
                    PHASE V
*/
#include "Scheduler.h"
#include "Semaphore.h"
#include "Window.h"
#include "MemoryMgmt.h"
#include "MCB.h"

//cleanWindows() - clean the Miscellaneous, Semaphore, and Console 
//                 windows of the GUI.
void cleanWindows()
{
    refresh(); //clear the console window
    wclear(mcb->mywin->Console_Win);
    wclear(mcb->mywin->Misc_Table);
    wclear(mcb->mywin->Sem_Win);
    refresh(); //clear the console window
    mcb->mywin->write_window(mcb->mywin->Console_Win, 1, 1, "Ultima # ");
    mcb->mywin->write_window(mcb->mywin->Misc_Table, 1, 1, "...Miscellaneous...\n");
    mcb->mywin->write_window(mcb->mywin->Sem_Win, 1, 1, "\n");
}

void killMenu(int input)
{
    char buff[256];
    if (input == '0')
        mcb->mysch->destroyTask(0);
    else if (input == '1')
        mcb->mysch->destroyTask(1);
    else if(input == '2')
        mcb->mysch->destroyTask(2);
    sprintf(buff, " Kill = %c\n", input);
    mcb->mywin->write_window(mcb->mywin->Console_Win, buff);
}

void memoryTest()
{
    int error;

    error = mcb->mymemo->Mem_alloc(128, 1);

            if (error == 0)
                mcb->mywin->write_window(mcb->mywin->Misc_Table, " Block allocated with success!\n");
            else
                mcb->mywin->write_window(mcb->mywin->Misc_Table, " Error allocating block!\n");

            //Writing test
            mcb->mywin->write_window(mcb->mywin->Misc_Table, " ....Writing test\n");
            error = mcb->mymemo->Mem_write(0, 0, 16, "this is task one", 1);
        
            //Reading test
            char *ch;
            mcb->mywin->write_window(mcb->mywin->Misc_Table, "  ....Reading test\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table, "  ....Char read\n");
            mcb->mymemo->Mem_read(0,ch,1);
            mcb->mywin->write_window(mcb->mywin->Misc_Table, "  ....Multibyte read \n");
            mcb->mymemo->Mem_read(0,0,3,ch,1);

            //Free test
            //mcb->mymemo->Mem_free(0,1);

            //Memory Dump
            mcb->mymemo->Mem_dump();        
}