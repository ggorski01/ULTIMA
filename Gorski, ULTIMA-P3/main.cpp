#include <iostream>
#include <sched.h>
#include <time.h>
#include <pthread.h>

#include "Semaphore.h"
#include "Scheduler.h"
#include "MemoryMgmt.h"

using namespace std;

#include "MCB.h"

void startMCB()
{
    Scheduler *newScheduler = new Scheduler();
    Semaphore *newSemaphore = new Semaphore(1, "resourceTest");
    Semaphore *newSemaphore2 = new Semaphore(1, "memoryManager");
    IPC *newIPC = new IPC();
    Window *newWin = new Window();
    MemoryManager *newMemo = new MemoryManager();
    mcb = new MCB(newScheduler, newSemaphore, newSemaphore2, newIPC, newWin, newMemo);
    mcb->mysch->setMCB(mcb);
    mcb->mysema->setMCB(mcb);
    mcb->mysema2->setMCB(mcb);
    mcb->mymemo->setMCB(mcb);
    mcb->myIPC->setMCB(mcb);
}

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
    int error;
    //mcb->mysch->destroyTask(2);
    //mcb->mysch->garbageCollector();
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