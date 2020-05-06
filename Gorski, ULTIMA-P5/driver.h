/*
Filename: driver.h
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
#include "IPC.h"
#include "MemoryMgmt.h"
#include "Ufs.h"
#include "MCB.h"

//phI_deadlock () - Phase 1 testing scenario:
//                  Two worker functions engage in circular wait.
//                  First task ask for resource1.down(), gets it
//                  and tries to ask for resource2.down(), but it does
//                  not get so it becomes state BLOCKED and has to wait
//                  for it be released. Second task asks for resource2.down()
//                  , gets it then asks for resource2.down() gets BLOCKED.
//                  Both blocked tasks should not run.
void phI_deadlock()
{
    int t_id;
    char buff[265];

    //---------- DOWN
    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to obtain semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema->down(t_id); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to obtain semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);    
    mcb->mysema->down(t_id); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to obtain semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema0->down(t_id); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to obtain semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema0->down(t_id); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    //---------- UP
    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to release semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema0->up(); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to release semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema0->up(); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to release semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema0->up(); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to release semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema->up(); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to release semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema->up(); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to release semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema->up(); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();
}

//semaphore_run () - provides a simple Semaphore test.
void semaphore_run()
{
    int t_id;
    char buff[256];

    //---------- DOWN
    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to obtain semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema->down(t_id); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to obtain semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema->down(t_id); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to obtain semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema->down(t_id); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    //---------- UP
    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to release semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema->up(); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to release semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema->up(); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buff, " TaskID:%d is trying to release semaphore.\n", t_id);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
    mcb->mysema->up(); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();
}

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

void IPCTest()
{
    srand (time(NULL));
    int t_id, error;
    char buffer[256];
    t_id = mcb->mysch->getCurrTaskID();
    for(int i = 0; i < 5 ; i++)
    {
        int dest = rand() % 3;
        error = mcb->myIPC->sendMessage(t_id, dest, TEXT, "Testing my main...");
        if (error == 0)
        {
            sprintf(buffer," TaskID #%d - Sending message to TaskID #%d...\n",t_id, dest);
            mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
        }
    }
    mcb->myIPC->dumpIPC();

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buffer, " TaskID #%d - Message count: %d messages\n",t_id,mcb->myIPC->messageCount(t_id));
    mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

    t_id = mcb->mysch->getCurrTaskID();
    sprintf(buffer, " TaskID #%d - Receiving message...\n",t_id);
    mcb->mywin->write_window(mcb->mywin->Misc_Table,buffer);
    mcb->myIPC->receiveMessage(t_id);

    t_id = mcb->mysch->getCurrTaskID();
    mcb->myIPC->dumpMailbox(t_id);
    sprintf(buffer, " TaskID #%d - Delete all messages...\n",t_id);
    mcb->mywin->write_window(mcb->mywin->Misc_Table,buffer);
    mcb->myIPC->removeAMailbox(t_id);
    mcb->myIPC->dumpMailbox(t_id);
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