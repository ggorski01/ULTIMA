#include <iostream>
#include <sched.h>
#include <time.h>
#include <pthread.h>

#include "Semaphore.h"
#include "Scheduler.h"
#include "MemoryMgmt.h"
#include "Ufs.h"
#include "IPC.h"

using namespace std;

#include "MCB.h"

void startMCB();

int main()
{
    startMCB();
    char buff[256];
    int input = -1;
    int CPU_Quantum = 1;

    int t_id;
    t_id = mcb->mysch->create_task();
    t_id = mcb->mysch->create_task();
    t_id = mcb->mysch->create_task();
    //t_id = mcb->mysch->create_task(); //It should display error if uncomment 
    int error;
    mcb->mysch->start();

    //---------------------------------------------------------------------------------
    while (input != 'q')
    {
        input = wgetch(mcb->mywin->Console_Win);

        mcb->mysch->wasteTime(3);
        mcb->mysch->yield();
        switch (input)
        {
        case '0':
        case '1':
        case '2':
            if (input == '0')
                mcb->mysch->destroyTask(0);
            else if (input == '1')
                mcb->mysch->destroyTask(1);
            else
                mcb->mysch->destroyTask(2);

            sprintf(buff, " %c\n", input);
            mcb->mywin->write_window(mcb->mywin->Console_Win, buff);
            sprintf(buff, " Kill = %c\n", input);
            mcb->mywin->write_window(mcb->mywin->Console_Win, buff);
            wclear(mcb->mywin->Log_Win);
            mcb->mywin->write_window(mcb->mywin->Log_Win, buff);

            sleep(4);
            wclear(mcb->mywin->Console_Win);
            mcb->mywin->write_window(mcb->mywin->Console_Win, 1, 1, "Ultima # ");
            break;
        case 'c':
            refresh(); //clear the console window
            wclear(mcb->mywin->Console_Win);
            mcb->mywin->write_window(mcb->mywin->Console_Win, 1, 1, "Ultima # ");
            break;
        case 'g':
            mcb->mysch->garbageCollector();
            break;
        case 'h':
        case 'b':
            mcb->mywin->display_help(mcb->mywin->Console_Win);
            mcb->mywin->write_window(mcb->mywin->Console_Win, 8, 1, "Ultima # ");
            break;
        case 'i':
            error = mcb->myIPC->sendMessage(0, 1, TEXT, "Testing my main...");
            if (error == 0)
                mcb->mywin->write_window(mcb->mywin->Misc_Table, " Sending message...\n");
            mcb->myIPC->dumpIPC();
            //mcb->myIPC->receiveMessage(1);
            break;
        case 'n':
            mcb->mywin->cont_help(mcb->mywin->Console_Win);
            mcb->mywin->write_window(mcb->mywin->Console_Win, 8, 1, "Ultima # ");
            break;
        case 'm':
            //Allocation test
            wclear(mcb->mywin->Misc_Table);
            mcb->mywin->write_window(mcb->mywin->Misc_Table, " ....Allocation test\n");
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
            mcb->mymemo->Mem_read(0,ch,1);

            mcb->mymemo->Mem_read(0,0,3,ch,1);

            //Free test
            //mcb->mymemo->Mem_free(0,1);

            //Memory Dump
            mcb->mymemo->Mem_dump();        

            break;
        case 's':
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
            mcb->mysema->up(); // hold on to the resource
            mcb->mysch->wasteTime(3);
            mcb->mysch->yield();

            t_id = mcb->mysch->getCurrTaskID();
            mcb->mysema->up(); // hold on to the resource
            mcb->mysch->wasteTime(3);
            mcb->mysch->yield();

            t_id = mcb->mysch->getCurrTaskID();
            mcb->mysema->up(); // hold on to the resource
            mcb->mysch->wasteTime(3);
            mcb->mysch->yield();
            break;
        case 'u':
            mcb->mywin->write_window(mcb->mywin->Misc_Table," ...Testing filesystem...\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table," ...Creating a file...\n");

            mcb->myufs->Create_file(0,"f1",75,"1100");

            mcb->myufs->Create_file(1,"f2",130,"1101");

            mcb->mywin->write_window(mcb->mywin->Misc_Table," ...Dump Metada file...\n");
            
            mcb->myufs->Dir();
            
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
            mcb->mywin->write_window(mcb->mywin->Misc_Table,"...Dump Data file...\n");

            mcb->myufs->dump();

            //cout << newFS->Change_permission(0,"f1","0000") << endl;

            char b[256];
            sprintf(b," Open f_id: %d\n",mcb->myufs->Open(1,1,"f2",WRITE));
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);

            sprintf(b," Open f_id: %d\n",mcb->myufs->Open(0,0,"f1",READ));
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);

            mcb->mywin->write_window(mcb->mywin->Misc_Table,"...Write char...\n");
            
            mcb->myufs->Write_char(1,0,'H');
            mcb->myufs->Write_char(1,0,'i');

            //Uncomment below to dump data file.
            mcb->mywin->write_window(mcb->mywin->Misc_Table,"...Dump Data file...\n");
            mcb->myufs->dump();

            mcb->mywin->write_window(mcb->mywin->Misc_Table,"...Read char...\n");
            mcb->myufs->Read_char(0,1,ch);

            break;
        case 'q':
            //end the loop, and end the program.

        case ERR:
            //if wgetch() return ERR, that means no keys were pressed
            //earlier we enabled non-blocking input using nodelay() see above
            //this allows the program to continue to inspect the keyboard without
            //having to wait for the key to be pressed;
            break;
        default:
            sprintf(buff, " %c\n", input);
            mcb->mywin->write_window(mcb->mywin->Console_Win, buff);
            mcb->mywin->write_window(mcb->mywin->Console_Win, " - Invalid Command\n");
            mcb->mywin->write_window(mcb->mywin->Log_Win, buff);
            mcb->mywin->write_window(mcb->mywin->Log_Win, " - Invalid Command\n");
            mcb->mywin->write_window(mcb->mywin->Console_Win, " Ultima #");
            break;
        }
        //sleep(1);
        CPU_Quantum++;
    }
    endwin(); //End the curses window
    return 0;
}


void startMCB()
{
    Scheduler *newScheduler = new Scheduler();
    Semaphore *newSemaphore = new Semaphore(1, "resourceTest");
    Semaphore *newSemaphore1 = new Semaphore(1,"IpC");
    Semaphore *newSemaphore2 = new Semaphore(1, "memoryManager");
    Semaphore *newSemaphore3 = new Semaphore(1,"FileSystem");
    IPC *newIPC = new IPC();
    Window *newWin = new Window();
    MemoryManager *newMemo = new MemoryManager();
    
    
    Ufs *newUfs = new Ufs("root",16,128,'^');
    
    mcb = new MCB(newScheduler, newSemaphore,newSemaphore1, newSemaphore2, newSemaphore3, newIPC, newWin, newMemo,newUfs);
    mcb->mysch->setMCB(mcb);    //Scheduler
    mcb->mysema->setMCB(mcb);   //Regular semaphore.
    mcb->mysema1->setMCB(mcb);  //IPC semaphore.
    mcb->mysema2->setMCB(mcb);  //Memory Manager semaphore
    mcb->mysema3->setMCB(mcb);  //Filesystem semaphore.
    mcb->mymemo->setMCB(mcb);   //Memory manager.
    mcb->myIPC->setMCB(mcb);    //Inter process communication.
    mcb->myufs->setMCB(mcb);    //Filesystem.
}