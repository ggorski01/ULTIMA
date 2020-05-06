/*
Filename: IPC.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To implement a Round Robin scheduler mechanism.
*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

#ifndef THREAD_STATES
#define THREAD_STATES
enum STATE
{
    READY,
    RUNNING,
    BLOCKED,
    DEAD
};
#endif

const int NO_OF_THREADS = 3;

#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "ThreadTable.h"

#include "MCB.h"
#include "IPC.h"

using namespace std;

class Scheduler
{
private:
    int currentThread;
    long currentQuantum;
    int nextAvailableThreadID;
    ThreadTable threadTable;
    MCB *mcb;
    char buffer[256];
    pthread_mutex_t mux;

public:
    Scheduler();
    ~Scheduler();
    int create_task();
    void *worker(void *arguments);
    void start();
    void yield();
    void setQuantum(long quantum);
    long getQuantum();
    void setState(int task_id, STATE the_state);
    int getCurrTaskID();
    TCB *getTCB(int task_id);
    void dump();
    void wasteTime(int x);
    void destroyTask(int task_id);
    void garbageCollector();
    void setMCB(MCB *mcb);
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor
Scheduler::Scheduler()
{
    currentThread = -1; //No active process.
    nextAvailableThreadID = 0;
    currentQuantum = 300; //default quantum 300ms.
}
//----------------------------------------

//Destructor
Scheduler::~Scheduler()
{
    currentThread = -1; //No active process.
    nextAvailableThreadID = 0;
    currentQuantum = 300; //default quantum 300ms.
}
//----------------------------------------

//CreateTask() -- Create an operating system process.
int Scheduler::create_task()
{
    char buffer[256];
    if (nextAvailableThreadID < NO_OF_THREADS)
    {
        sprintf(buffer, "Creating task # %d\n", nextAvailableThreadID);
        mcb->mywin->write_window(mcb->mywin->Heading_Win, 5 + nextAvailableThreadID, 6, buffer);

        WINDOW *newTaskWIN = mcb->mywin->create_window(15, 27, 15, 2 + (threadTable.getThreadTableSize() * 28));
        threadTable.insertTCB(nextAvailableThreadID, clock(), READY, newTaskWIN);
        //Source: https://thispointer.com/c-how-to-pass-class-member-function-to-pthread_create/
        typedef void *(*THREADFUNCPTR)(void *);
        int err = pthread_create(threadTable.getTCBProcessID(nextAvailableThreadID), NULL, (THREADFUNCPTR)&Scheduler::worker, this);
        assert(!err);
        mcb->myIPC->createMailbox(nextAvailableThreadID);
        nextAvailableThreadID++;
        return (nextAvailableThreadID - 1);
    }
    else
    {
        //sprintf(buffer,"Create task FAILED: Number of available threads exceeded the limit..\n");
        return (-1); // return error in case cannot create a task.
    }
}
//----------------------------------------

//*worker() - Context swapping function.
void *Scheduler::worker(void *arguments)
{
    int mythreadNum = 0;
    char buffer[256];
    //Find the TCB related to the thread running in the OS background.
    while (*threadTable.getTCBProcessID(mythreadNum) != pthread_self())
        mythreadNum++;

    //As long as the OS background running thread is not DEAD
    //and it is running do:
    while ((threadTable.getTCBState(mythreadNum) != DEAD))
    {
        if (threadTable.getTCBState(mythreadNum) == RUNNING)
        {
            //Prints the pthread_t of the running thread.
            sprintf(buffer, " Task#%d running..\n", currentThread);
            mcb->mywin->write_window(threadTable.getTCBWIN(currentThread), buffer);

            //Print all information over the Scheduler class.
            dump();
            //Call Scheduler::yield() so that current thread can
            //voluntary give up the CPU to the next ready task.
            yield();
        }
        else
        {
            //If not, just relinquish the CPU by the calling thread
            //, insuring for example that the calling thread of
            //a createTask() function call do not hold the CPU forever.
            pthread_yield();
        }
    }
}
//----------------------------------------

//start() - Initiate scheduling mechanism by
//          setting the first task in thread table
//          to running.
void Scheduler::start()
{

    sprintf(buffer, " ... STARTING SCHEDULING...\n");
    mcb->mywin->write_window(mcb->mywin->Heading_Win, 4, 1, buffer);

    TCB *tmp = threadTable.getTCBHead();
    //Find the first READY process
    //by jumping through DEAD processes
    while (tmp->threadState == DEAD)
        tmp = tmp->next;
    //Set the first found READY process to state RUNNING, then begin scheduling
    threadTable.setTCBStartTime(tmp->threadID, clock());
    threadTable.setTCBState(tmp->threadID, RUNNING);
    currentThread = tmp->threadID;
    setQuantum(1000 / NO_OF_THREADS); // Set quantum to 1sec/number of threads
    wasteTime(2);
}
//----------------------------------------

//yield() - runs the thread with set RUNNING state until
//          its quantum runs out. When quantum runs out,
//          set current RUNNING to READY then finds the next
//          READY thread then set its state to RUNNING. Finally,
//          puts the OS thread current running in background to sleep
//          so that the next can have its turn to run.
void Scheduler::yield()
{

    sprintf(buffer, " ---------------------\n");
    mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);

    int counter = 0;

    sprintf(buffer, " Current Task # %d is trying to yield.\n", currentThread);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);

    sprintf(buffer, " Current Quantum : %li\n", currentQuantum);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);

    // calculate elapsed_time since the thread last started to run.
    clock_t elapsed_time = clock() - threadTable.getTCBStartTime(currentThread);

    sprintf(buffer, " Elapsed time: %li\n", elapsed_time);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);

    if (elapsed_time >= currentQuantum) // if quantum has run out
    {
        sprintf(buffer, " yielding....(Switching from task #%d to next ready task).\n", currentThread);
        mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);

        // if current thread is RUNNING make it READY (its quantum has run out)
        if (threadTable.getTCBState(currentThread) == RUNNING)
            threadTable.setTCBState(currentThread, READY);

        // now find the next READY thread and make it running
        // watch out for deadlocks. (no ready processes)
        currentThread = (currentThread + 1) % NO_OF_THREADS;
        if (threadTable.findTCB(currentThread) == false)
            currentThread = (currentThread + 1) % NO_OF_THREADS;

        while (threadTable.getTCBState(currentThread) != READY && counter < NO_OF_THREADS - 1)
        {
            currentThread = (currentThread + 1) % NO_OF_THREADS;
            if (threadTable.findTCB(currentThread) == false)
                currentThread = (currentThread + 1) % NO_OF_THREADS;
        }
        // if we find a READY threads re-set the quantum and set the task to running
        if (counter < NO_OF_THREADS - 1 && threadTable.getTCBState(currentThread) == READY)
        {
            threadTable.setTCBStartTime(currentThread, clock()); // restart the quantum
            threadTable.setTCBState(currentThread, RUNNING);

            sprintf(buffer, " Started Running task # %d \n", currentThread);
            mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);

            //Sleep(1) in here, insures syncronization between the
            //threads and the yield function

            sleep(1);
        }
        else
        {
            sprintf(buffer, "----- P O S S I B L E   D E A D   L O C K -----");
            mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);
        }
    }
    else
    {
        sprintf(buffer, "---- N O   Y I E L D -----\n");
        mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);
    }
}
//----------------------------------------

//setQuantum(long quantum) - Assign a passed value
//                           to currentQuantum.
void Scheduler::setQuantum(long quantum)
{
    currentQuantum = quantum;
}
//----------------------------------------

//getQuantum() - Return currentQuantum value
long Scheduler::getQuantum()
{
    return (currentQuantum);
}
//----------------------------------------

//setState(int,STATE) - Set the state of a specific task to
//                      a specific state.
void Scheduler::setState(int task_id, STATE the_state)
{
    threadTable.setTCBState(task_id, the_state);
}
//----------------------------------------

//getTaskID() - Return currentThread ID.
int Scheduler::getCurrTaskID()
{
    return currentThread;
}
//----------------------------------------

//getTCB() - Return a pointer to TCB searched
//           by threadID.
TCB *Scheduler::getTCB(int task_id)
{
    TCB *tmp = threadTable.getTCBHead();
    while (tmp)
    {
        if (task_id == tmp->threadID)
            return tmp;
        tmp = tmp->next;
    }
    //sprintf(buffer,"getTCB - TCB not found! Return exit(-1).");
    //mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);

    exit(-1);
}
//----------------------------------------

//dump() - Ouput important messages over the Scheduler.
void Scheduler::dump()
{
    sprintf(buffer, " -----PROCESS Table -----\n");
    mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);

    clock_t elapsed_time = clock() - threadTable.getTCBStartTime(currentThread);

    sprintf(buffer, " Quantum = %li \n", currentQuantum);
    mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);

    TCB *tmp = threadTable.getTCBHead();
    while (tmp != NULL)
    {
        if (tmp->threadState == READY)
            sprintf(buffer, " process id %li task state: READY ", tmp->processID);
        else if (tmp->threadState == RUNNING)
            sprintf(buffer, " process id %li task state: RUNNING ", tmp->processID);
        else if (tmp->threadState == BLOCKED)
            sprintf(buffer, " process id %li task state: BLOCKED ", tmp->processID);
        else if (tmp->threadState == DEAD)
            sprintf(buffer, " process id %li task state: DEAD ", tmp->processID);

        char curr[] = "<--CURRENT\n";
        if (tmp->threadID == currentThread)
            strcat(buffer, curr);
        else
            strcat(buffer, "\n");

        mcb->mywin->write_window(mcb->mywin->Log_Win, buffer);
        tmp = tmp->next;
    }
}
//----------------------------------------

//wasteTime(int) - runs a for loop for (x *
//                 10240) amount of times.
void Scheduler::wasteTime(int x)
{
    unsigned long long Int64 = 0;
    for (unsigned short i = 0; i < 10240 * x; ++i)
    {
        for (unsigned short j = i; j > 0; --j)
            Int64 += j + i;
    }
}
//----------------------------------------

//garbageCollector() - Goes through the thread
//                     table finding all the
//                     state DEAD threads,
//                     after finding it, removes
//                     its TCB from thread table.
void Scheduler::garbageCollector()
{
    //sprintf(buffer,"Garbage collector ");
    TCB *tmp = threadTable.getTCBHead();
    while (tmp != NULL)
    {
        if (tmp->threadState == DEAD)
        {
            threadTable.removeATCB(tmp->threadID);
            mcb->myIPC->removeAMailbox(tmp->threadID);
        }
        tmp = tmp->next;
    }
}
//----------------------------------------

//destroyTask(int) - Sets the state of a task
//                   to DEAD, then calls a pthread
//                   library function pthread_cancel
//                   to cancel the running task in the
//                   background.
void Scheduler::destroyTask(int task_id)
{
    threadTable.setTCBState(task_id, DEAD);
    pthread_cancel(*threadTable.getTCBProcessID(task_id));
}
//----------------------------------------

//setMCB(MCB) - Register the Master Control Block mechanism
//              within the scheduler so that Scheduler
//              can access other mechanisms such as IPC,Semaphore,
//              ...
void Scheduler::setMCB(MCB *mcb)
{
    this->mcb = mcb;
}
//----------------------------------------

#endif