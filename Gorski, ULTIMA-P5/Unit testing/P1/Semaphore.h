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
        void dump();
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
        mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);
        dump();
    }
    else
    {
        if (semaValue >= 1)
        {
            semaValue--;
            luckyTask = taskID; // preserve the task-id who got the resource
            dump();
        }
        else
        {
            semaQueue.enqueue(taskID);
            mcb->mysch->setState(taskID, BLOCKED);
            dump();
            mcb->mysch->yield();
            dump();
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
    mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);

    if (mcb->mysch->getCurrTaskID() == luckyTask) // check to see if the correct task is doing the up()
    {
        if (semaQueue.empty())
        {
            semaValue++;
            luckyTask = -1;
            dump();
        }
        else
        {
            task_id = semaQueue.top()->data; // Remove from queue and unblock
            semaQueue.dequeue();
            mcb->mysch->setState(task_id, READY); // set the task to READY

            sprintf(buffer," Unblock: %d ,and release from the queue.\n", task_id);
            mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);

            luckyTask = task_id; //
            sprintf(buffer," Lucky Task = %d \n", luckyTask);
            mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);
            dump();
            mcb->mysch->yield();
            dump();
        }
    }
    else
    {
        sprintf(buffer," Invalid UP() - TaskID : %d does not own the resource.\n", mcb->mysch->getCurrTaskID());
        mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);
        dump();
    }
    pthread_mutex_unlock(&mux);
}
//---------------------------------------------

//dump() - Output information over the Semaphore.
void Semaphore::dump()
{
    mcb->mywin->write_window(mcb->mywin->Sem_Win,1,1, " SEMAPHORE DUMP\n-------------------\n");

    sprintf(buffer," Value: %d\n", semaValue);
    mcb->mywin->write_window(mcb->mywin->Sem_Win, buffer);
    
    sprintf(buffer," Name: ");
    strcat(buffer,resourceName.c_str());
    strcat(buffer,"\n");
    mcb->mywin->write_window(mcb->mywin->Sem_Win, buffer);

    sprintf(buffer," Owned by Task#: %d \n", luckyTask);
    mcb->mywin->write_window(mcb->mywin->Sem_Win, buffer);

    sprintf(buffer," Queue: ");
    if(semaQueue.size() > 0)
    {
    char buffer2[256];
    Node<int> *tmp = semaQueue.top();
    
    while(tmp!=NULL)
    {
        sprintf(buffer2," %d | ",tmp->data);
        strcat(buffer,buffer2);
        tmp = tmp->next;
    }
    strcat(buffer,"\n");
    
    }else
    {
        strcat(buffer," EMPTY\n");
    }
    mcb->mywin->write_window(mcb->mywin->Sem_Win, buffer);
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