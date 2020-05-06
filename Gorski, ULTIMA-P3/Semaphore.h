/*
Filename: Semaphore.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To implement a binary semaphore.
*/
#ifndef Semaphore_H
#define Semaphore_H

#include <iostream>
#include <string>
using namespace std;

#include "Queue.h"
#include "Scheduler.h"
#include "MCB.h"

class Semaphore
{
    private:
        string resourceName; // The name of the resource being managed.
        int semaValue;       // 0 or 1 in the case of a binary Semaphore.
        int luckyTask;       // Preserve the task-id of the task that got the resource (for debbugging).
        pthread_mutex_t mux;  // Critical section lock mechanism.
        Queue<int> semaQueue;
        MCB *mcb;
        char buffer[256];
    public:
        Semaphore(int starting_value, string name);
        ~Semaphore();
        void down(int taskID);
        void up();
        void dump(int level);
        void setMCB(MCB* mcb);
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor
Semaphore::Semaphore(int starting_value, string name)
{
    semaValue = starting_value;
    resourceName = name;
    luckyTask = -1;
    pthread_mutex_init(&mux,NULL);
}
//---------------------------------------------

//Destructor
Semaphore::~Semaphore()
{
    while(semaQueue.size() > 0)
        semaQueue.dequeue();
    semaValue = -99;
    resourceName = "";
    luckyTask = -1;
    pthread_mutex_destroy(&mux);
}
//---------------------------------------------

//down() - Get the hold of the resource or get queued.
void Semaphore::down(int taskID)
{
    pthread_mutex_lock(&mux);
    if (taskID == luckyTask)
    {
        sprintf(buffer," Task# %d own the resource. REQUEST IGNORED!\n", luckyTask);
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
        dump(1);
    }
    else
    {
        if (semaValue >= 1)
        {
            semaValue--;
            luckyTask = taskID; // preserve the task-id who got the resource
            dump(1);
        }
        else
        {
            semaQueue.enqueue(taskID);
            mcb->mysch->setState(taskID, BLOCKED);
            dump(1);
            mcb->mysch->yield();
            dump(1);
        }
    }
    pthread_mutex_unlock(&mux);
}
//---------------------------------------------

//up() - Release the hold of the resource.
void Semaphore::up()
{
    pthread_mutex_lock(&mux);
    int task_id;

    sprintf(buffer," TaskID: %d LuckyID: %d \n", mcb->mysch->getCurrTaskID(), luckyTask);
    mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

    if (mcb->mysch->getCurrTaskID() == luckyTask) // check to see if the correct task is doing the up()
    {
        if (semaQueue.empty())
        {
            semaValue++;
            luckyTask = -1;
            dump(1);
        }
        else
        {
            task_id = semaQueue.top()->data; // Remove from queue and unblock
            semaQueue.dequeue();
            mcb->mysch->setState(task_id, READY); // set the task to READY

            sprintf(buffer," UnBlock: %d nd release from the queue.\n", task_id);
            mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

            luckyTask = task_id; //
            sprintf(buffer," Luck Task = %d \n", luckyTask);
            mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
            dump(1);
            mcb->mysch->yield();
            dump(1);
        }
    }
    else
    {
        sprintf(buffer," Invalid UP() - TaskID : %d does not own the resource.\n", mcb->mysch->getCurrTaskID());
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
        dump(1);
    }
    pthread_mutex_unlock(&mux);
}
//---------------------------------------------

//dump() - Output information over the Semaphore.
void Semaphore::dump(int level)
{
    sprintf(buffer," \n-------Semaphore DUMP-------\n");
    mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

    switch (level)
    {
    case 0: 
        sprintf(buffer," Sema_Value: %d\n", semaValue);
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
        
        sprintf(buffer," Sema_Name: ");
        strcat(buffer,resourceName.c_str());
        strcat(buffer,"\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

        sprintf(buffer," Obtained by Task-ID: %d \n", luckyTask);
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

        break;
    case 1:
        sprintf(buffer," Sema_Value: %d\n", semaValue);
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
        
        sprintf(buffer," Sema_Name: ");
        strcat(buffer,resourceName.c_str());
        strcat(buffer,"\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

        sprintf(buffer," Obtained by Task-ID: %d \n", luckyTask);
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

        //semaQueue.dumpQueue(mcb->mywin->Misc_Table);
        break;
    default:
        sprintf(buffer," ERROR in Semaphore DUMP level\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
    }
    sprintf(buffer,"----------------------------\n");
    mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
}

//setMCB(MCB) - Register the Master Control Block
//              mechanism so that Semaphore class
//              have access to other mechanisms 
//              such as the IPC, Scheduler...
void Semaphore::setMCB(MCB* mcb)
{
    this->mcb = mcb;
}
//---------------------------------------------

#endif