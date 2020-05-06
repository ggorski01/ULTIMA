#include <iostream>

#include <time.h>
#include <pthread.h>

#include "Semaphore.h"
#include "Scheduler.h"
#include "Ufs.h"
#include "MCB.h"
#include "driver.h"

using namespace std;

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
    mcb->mywin->display_help(mcb->mywin->Console_Win);
    mcb->mywin->write_window(mcb->mywin->Console_Win, 8, 1, "Ultima # ");
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
            killMenu(input);
            break;
        case 'c':
            cleanWindows();
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
            mcb->mywin->write_window(mcb->mywin->Misc_Table, " ....IPC Run\n");
            break;
        case 'n':
            mcb->mywin->cont_help(mcb->mywin->Console_Win);
            mcb->mywin->write_window(mcb->mywin->Console_Win, 8, 1, "Ultima # ");
            break;
        case 'm':
            //Allocation test
            mcb->mywin->write_window(mcb->mywin->Misc_Table, " ....Allocation test\n");
            break;
        case 's':
            mcb->mywin->write_window(mcb->mywin->Misc_Table, " Semaphore Run...\n");

            break;
        case 'd':
            mcb->mywin->write_window(mcb->mywin->Misc_Table, " Semaphore Deadlock Run...\n");
            break;
        case 'p':
            mcb->SUSPEND = true;
            break;
        case 'r':
            mcb->SUSPEND = false;
            break;
        case 'u':
            fileSysTestI();
            break;
        case 'j':
            fileSysTestII();
            break;
        case 'l':
            fileSysTestIII();
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
    Semaphore *newSemaphore3 = new Semaphore(1, "FileSystem");
    Window *newWin = new Window();

    Ufs *newUfs = new Ufs("root", 16, 128, '^');

    mcb = new MCB(newScheduler, newSemaphore3, newWin, newUfs);
    mcb->mysch->setMCB(mcb);   //Scheduler
    mcb->mysema3->setMCB(mcb); //Filesystem semaphore.
    mcb->myufs->setMCB(mcb);   //Filesystem.
}