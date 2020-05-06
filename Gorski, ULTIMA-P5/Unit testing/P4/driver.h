/*
Filename: Driver.h
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
#include "Ufs.h"
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
    else if (input == '2')
        mcb->mysch->destroyTask(2);
    sprintf(buff, " Kill = %c\n", input);
    mcb->mywin->write_window(mcb->mywin->Console_Win, buff);
}

void fileSysTestI()
{
    
    mcb->mywin->write_window(mcb->mywin->Misc_Table, " ...Testing filesystem...\n");
    mcb->mywin->write_window(mcb->mywin->Misc_Table, " ...Creating a file...\n");

    mcb->myufs->Create_file(0, "f1", 75, "1100");

    mcb->myufs->Create_file(1, "f2", 130, "1101");

    mcb->mywin->write_window(mcb->mywin->Misc_Table, " ...Dump Metada file...\n");

    mcb->myufs->Dir();

}
void fileSysTestII()
{
    char *ch;
    //Uncomment below to dump data file.
    //mcb->mywin->write_window(mcb->mywin->Misc_Table,"...Dump Data file...\n");

    //mcb->myufs->dump();

    //Uncomment below to test access permission.
    /* int f_id;
            mcb->mywin->write_window(mcb->mywin->Misc_Table," ...Testing Permissions... \n");
            //mcb->myufs->Dir(0);
            mcb->mywin->write_window(mcb->mywin->Misc_Table," Testing USER READ \n");
            f_id = mcb->myufs->Open(0,0,"f1",READ);

            mcb->mywin->write_window(mcb->mywin->Misc_Table," Testing USER WRITE \n");
            f_id = mcb->myufs->Open(0,0,"f1",WRITE);

            mcb->mywin->write_window(mcb->mywin->Misc_Table," Testing OTHERS READ \n");
            f_id = mcb->myufs->Open(1,0,"f1",READ);

            mcb->mywin->write_window(mcb->mywin->Misc_Table," Testing OTHERS WRITE \n");
            f_id = mcb->myufs->Open(1,0,"f1",WRITE); */

    //Uncomment below to dump data file.
    mcb->mywin->write_window(mcb->mywin->Misc_Table, "...Dump Data file...\n");

    mcb->myufs->dump();

    //cout << newFS->Change_permission(0,"f1","0000") << endl;

    char b[256];
    sprintf(b, " Open f_id: %d\n", mcb->myufs->Open(1, 1, "f2", WRITE));
    mcb->mywin->write_window(mcb->mywin->Misc_Table, b);

    sprintf(b, " Open f_id: %d\n", mcb->myufs->Open(0, 0, "f1", READ));
    mcb->mywin->write_window(mcb->mywin->Misc_Table, b);

    mcb->mywin->write_window(mcb->mywin->Misc_Table, "...Write char...\n");

    mcb->myufs->Write_char(1, 0, 'H');
    mcb->myufs->Write_char(1, 0, 'i');

    //Uncomment below to dump data file.
    mcb->mywin->write_window(mcb->mywin->Misc_Table, "...Dump Data file...\n");
    mcb->myufs->dump();

    mcb->mywin->write_window(mcb->mywin->Misc_Table, "...Read char...\n");
    mcb->myufs->Read_char(0, 1, ch);
}

void fileSysTestIII()
{
    mcb->myufs->Dir();
}