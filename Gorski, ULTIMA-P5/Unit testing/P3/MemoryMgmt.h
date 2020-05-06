/*
Filename: MemoryMgmt.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To create a memory management mechanism.
                    Phase III
*/
#ifndef MEMORY_MGMT_H
#define MEMORY_MGMT_H

#include <iostream>
#include <fstream>
using namespace std;

#include "LinkedList.h"
#include "MCB.h"
class MCB;

enum MEMORY_LABEL
{
    FREE,
    USED
};

struct Segment
{
    int base;            //Start of the memory segment.
    int limit;           //End of the memory segment.
    int size;            //Number of bytes requested by task.
    int ownerID;         //ID of the owner task.
    int handler;         //set by memory manager class.
    MEMORY_LABEL status; //0 - free, 1 - used.
    int write_cursor;
    int read_cursor;
};

class MemoryManager
{
private:
    int nextAvailableHandler;
    int freeSpace; //How much free memory I have.
    LinkedList<Segment> MemoManager;
    char *memory; //Actually memory resource.
    MCB *mcb;
    char buffer[1026];
    int Mem_left();
    int Mem_largest();
    int Mem_smallest();
    int Mem_coalesce();
    int Mem_burp();

public:
    MemoryManager();
    ~MemoryManager();
    int Mem_dump();
    int Mem_alloc(int size, int taskID);
    int Mem_free(int memory_handle, int taskID);
    int Mem_read(int memory_handle, char *ch, int taskID);
    int Mem_write(int memory_handle, char ch, int taskID);

    //Overloaded multi-byte read and write
    int Mem_read(int memory_handle, int offset_from_beg, int text_size, char *text, int taskID);
    int Mem_write(int memory_handle, int offset_from_beg, int text_size, char *text, int taskID);
    void setMCB(MCB *mcb) { this->mcb = mcb; }
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor - Initialize some basic variables
//as well as insert first segment in memory which
//is equivalent of the whole memory as free status
//.i.e. a block of status free from 0 to 1023.
MemoryManager::MemoryManager()
{
    //Initialize basic variables.
    nextAvailableHandler = 0;
    freeSpace = 1024;

    //Initialize actual memory by
    //setting capacity to 1024 bytes
    //and overwritting with dots.
    memory = new char[1024];
    for (int i = 0; i < 1024; i++)
        memory[i] = '.';

    //First segment of the manager should display
    //the entire empty memory.
    Segment newSeg;
    newSeg.base = 0;
    newSeg.limit = 1023;
    newSeg.ownerID = -1;
    newSeg.handler = -1;
    newSeg.size = 1024;
    newSeg.status = FREE;
    newSeg.write_cursor = newSeg.read_cursor = 0;

    MemoManager.insertFront(newSeg, newSeg.handler);
}
//----------------------------------------

//Destructor.
MemoryManager::~MemoryManager()
{
    nextAvailableHandler = 0;
    freeSpace = 1024; 
    MemoManager.~LinkedList();
    for (int i = 0; i < 1024; i++)
    {
        memory[i] = '.';
        buffer[i]=' ';
    }
    mcb = NULL;
}
//----------------------------------------

//Mem_alloc() - Uses first fit approach.
//Finds the first hole big enough then allocate,
//if not then throw -1 for error.
int MemoryManager::Mem_alloc(int size, int taskID)
{
    if (freeSpace < size)
        return -1; //Segment fault. No available memory.
    else
    {
        Node<Segment> *tmp = MemoManager.back();

        //Create new segments at the from of the linked
        //list manager.
        Segment newSeg;
        newSeg.base = tmp->data.base;
        if (MemoManager.size() == 0)
            newSeg.base = 0;
        else
            newSeg.limit = newSeg.base + 127;
        newSeg.ownerID = taskID;
        newSeg.handler = nextAvailableHandler;
        newSeg.size = size;
        newSeg.status = USED;
        newSeg.write_cursor = newSeg.read_cursor = newSeg.base;

        //Adjust the last node which would be the free
        //location by setting new values for base and size.
        tmp->data.base = newSeg.limit + 1;
        tmp->data.size = tmp->data.limit - tmp->data.base;
        tmp->data.write_cursor = tmp->data.base;
        tmp->data.read_cursor = tmp->data.base;
        freeSpace = tmp->data.size;

        MemoManager.insertFront(newSeg);

        //Handle increment
        nextAvailableHandler++;

        return 0; //Success allocating memory.
    }
}
//----------------------------------------

//Mem_free() - Check if there is such segment in memory
//and if requesting task owns the segment then if
//yes free the respective segment. If not of those
//apply, throw -1 for error.
int MemoryManager::Mem_free(int memory_handle, int taskID)
{
    //Check if there is such segment in memory.
    if (MemoManager.isNode(memory_handle))
    {
        Node<Segment> *tmp = MemoManager.front();
        while (tmp)
        {
            if (tmp->data.handler == memory_handle)
                break;
            tmp = tmp->next;
        }
        //Check if taskID is the owner(have permission).
        if (tmp->data.ownerID == taskID)
        {
            //Start of free up memory:
            for (int i = tmp->data.base; i <= tmp->data.limit; i++)
                memory[i] = '#';

            //Reset cursor's location to point to the beginning of the block.
            tmp->data.write_cursor = tmp->data.read_cursor = tmp->data.base;
            tmp->data.status = FREE;
            tmp->data.size = 0;
            freeSpace += (tmp->data.limit - tmp->data.base);

            
            int err = Mem_coalesce();
            if (err == -1)
            {
                sprintf(buffer, " No coalesce!\n");
                mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
            }
                
            return 0; //Success.
        }
        else
            return -1; //Access denied, task does not own the memory block.
    }
    else
        return -1; //Segment not found.
}
//----------------------------------------

//Mem_read() - Overloaded function / Single Byte Read
//The below reads ONE character then increments read_cursor location
//(current_location adjustment).
int MemoryManager::Mem_read(int memory_handle, char *ch, int taskID)
{
    mcb->mysema2->down(taskID);
    //Check if there is such segment in memory.
    if (MemoManager.isNode(memory_handle))
    {
        Node<Segment> *tmp = MemoManager.front();
        while (tmp)
        {
            if (tmp->data.handler == memory_handle)
                break;
            tmp = tmp->next;
        }

        //Check if taskID is the owner(have permission).
        if (tmp->data.ownerID == taskID)
        {
            ch = &memory[tmp->data.read_cursor]; //Read single character.
            tmp->data.read_cursor++;

            sprintf(buffer, " -----------------\n Content of Read: %c\n-----------------\n", *ch);
            mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
            //Checks if the new read_cursor position goes beyond
            //the writer_cursor, if yes then set read_cursor to the base
            //of the block.
            if (tmp->data.read_cursor >= tmp->data.limit)
            {
                tmp->data.read_cursor = tmp->data.base;
            }
            mcb->mysema2->up();
            return 0; //Success reading.
        }
        else
        {
            mcb->mysema2->up();
            return -1; //Access denied, task does not own the memory block.
        }
    }
    else
    {
        mcb->mysema2->up();
        return -1; //Segment not found.
    }    
}
//----------------------------------------

//Mem_write() - Overloaded function / Single Byte Write
//The below writes ONE character then increments write_cursor location.
int MemoryManager::Mem_write(int memory_handle, char ch, int taskID)
{
    mcb->mysema2->down(taskID);
    //Check if there is such segment in memory.
    if (MemoManager.isNode(memory_handle))
    {
        Node<Segment> *tmp = MemoManager.front();
        while (tmp)
        {
            if (tmp->data.handler == memory_handle)
                break;
            tmp = tmp->next;
        }

        //Check if taskID is the owner(have permission).
        if (tmp->data.ownerID == taskID)
        {

            //Check if write_cursor reached the limit of the segment block.
            if (tmp->data.write_cursor == tmp->data.limit)
            {    
                mcb->mysema2->up();
                return -1; //Segment is full.
            }
            else
            {
                memory[tmp->data.write_cursor] = ch; //Write in it.
                tmp->data.write_cursor++;            //adjust write_cursor.
                mcb->mysema2->up();
                return 0;                            //Success writing.
            }
        }
        else
        {
            mcb->mysema2->up();
            return -1; //Access denied, task does not own the memory block.
        }
    }
    else
    {
        mcb->mysema2->up();
        return -1; //Segment not found
    }
}
//----------------------------------------

//Overloaded multi-byte read and write

//Mem_read() - Read a large memory location.
int MemoryManager::Mem_read(int memory_handle, int offset_from_beg, int text_size, char *text, int taskID)
{
    mcb->mysema2->down(taskID);
    //Check if there is such segment in memory.
    if (MemoManager.isNode(memory_handle))
    {
        Node<Segment> *tmp = MemoManager.front();
        while (tmp)
        {
            if (tmp->data.handler == memory_handle)
                break;
            tmp = tmp->next;
        }
        //Check if TaskID owns the block
        if (tmp->data.ownerID == taskID)
        {
            //Check if what I am reading goes beyond my block limit
            if (tmp->data.limit < offset_from_beg + tmp->data.read_cursor + text_size)
            {
                mcb->mysema2->up();
                return -1; //Trying to reach more than it should.        
            }
            else
            {
                text = new char[offset_from_beg + 1];
                text[offset_from_beg + 1] = '\0';

                for (int i = 0; i < text_size; i++)
                {
                    text[i] = memory[tmp->data.read_cursor];
                    tmp->data.read_cursor++;
                }
                mcb->mywin->write_window(mcb->mywin->Misc_Table," -----------------\n Content of read: \n");
                strcpy(buffer, " ");
                strcat(buffer,text);
                strcat(buffer,"\n-----------------\n");
                mcb->mywin->write_window(mcb->mywin->Misc_Table,buffer);

                mcb->mysema2->up();
                return 0; //Success reading.
            }
        }
        else{
                mcb->mysema2->up();
                return -1; //Access denied, task does not own the memory block.
            }
    }
    else
    {
        mcb->mysema2->up();
        return -1; //Segment not found.
    }
}
//----------------------------------------

//Mem_write() - Write a large text into memory.
int MemoryManager::Mem_write(int memory_handle, int offset_from_beg, int text_size, char *text, int taskID)
{
    mcb->mysema2->down(taskID);
    //Check if there is such segment in memory.
    if (MemoManager.isNode(memory_handle))
    {
        Node<Segment> *tmp = MemoManager.front();
        while (tmp)
        {
            if (tmp->data.handler == memory_handle)
                break;
            tmp = tmp->next;
        }

        //Check if TaskID owns the block
        if (tmp->data.ownerID == taskID)
        {
            //Check if what I am writing goes beyond my block limit
            if (tmp->data.limit < offset_from_beg + tmp->data.write_cursor + text_size)
                return -1; //Trying to reach more than it should.
            else
            {
                for (int i = 0; i < text_size; i++)
                {
                    memory[tmp->data.write_cursor + offset_from_beg] = text[i];
                    tmp->data.write_cursor++;
                }
                tmp->data.write_cursor += offset_from_beg;

                mcb->mysema2->up();
                return 0; //Success writing.
            }
        }
        else
        {
            mcb->mysema2->up();
            return -1; //Access denied, task does not own the memory block.
        }
    }
    else
    {
        mcb->mysema2->up();
        return -1; //Segment not found.
    }
}
//----------------------------------------

//Mem_left() - Return how much free space is
//still available.
int MemoryManager::Mem_left()
{
    return freeSpace;
}

//Mem_largest - Return largest block in size.
int MemoryManager::Mem_largest()
{
    Node<Segment> *tmp = MemoManager.front();

    int largest = tmp->data.size; //Set the lasgest to be the first block found.

    for (int i = 0; i < MemoManager.size(); i++)
    {
        if (tmp->data.size > largest && tmp->data.status == FREE)
            largest = tmp->data.size;
        tmp = tmp->next;
    }
    return largest;
}
//----------------------------------------

//Mem_smallest() - Return smallest block in size.
int MemoryManager::Mem_smallest()
{
    Node<Segment> *tmp = MemoManager.front();

    int smallest = tmp->data.size; //Set the lasgest to be the first block found.

    for (int i = 0; i < MemoManager.size(); i++)
    {
        if (tmp->data.size > smallest && tmp->data.status == FREE)
            smallest = tmp->data.size;
        tmp = tmp->next;
    }
    return smallest;
}

//Mem_coalesce() - Merge two adjacent free blocks of memory.
int MemoryManager::Mem_coalesce()
{
    Mem_dump();
    Node<Segment> *curr = MemoManager.front();
    Node<Segment> *prev;

    int i = -1;
    while (curr)
    {
        if (prev->data.status == FREE && curr->data.status == FREE)
        {

            for (int i = prev->data.base; i <= prev->data.limit; i++)
                memory[i] = '.';
            for (int i = curr->data.base; i <= curr->data.limit; i++)
                memory[i] = '.';

            curr->data.base = prev->data.base;
            curr->data.size = 0;
            curr->data.write_cursor = curr->data.read_cursor = 0;

            MemoManager.removeNode(i);
            //Mem_dump();
            return 0; //Success coalesce.
        }
        i++;
        prev = curr;
        curr = curr->next;
    }
    return -1; //Error coalesce.
}
//----------------------------------------

//Mem_burp() - Move empty holes to the end of MemoryManager list.
int MemoryManager::Mem_burp()
{
    //Check if there are empty holes.
    if (freeSpace > 0)
    {
        Node<Segment> *tmp = MemoManager.front();
        for (int i = 0; i < MemoManager.size(); i++)
        {
            //Check if is an empty hole.
            if (tmp->data.status == FREE)
            {
                //Copy the empty hole
                Segment newSeg;
                newSeg.base = tmp->data.base;
                newSeg.handler = tmp->data.handler;
                newSeg.limit = tmp->data.limit;
                newSeg.ownerID = tmp->data.ownerID;
                newSeg.read_cursor = tmp->data.read_cursor;
                newSeg.size = tmp->data.size;
                newSeg.status = tmp->data.status;
                newSeg.write_cursor = tmp->data.write_cursor;

                //Remove respective empty hole
                MemoManager.removeNode(i);
                //Add it at the end of MemoryManager list.
                MemoManager.insertBack(newSeg);
            }
            tmp = tmp->next;
        }
        return 0; //Success in moving holes.
    }
    else
        return -1; //No empty holes.
}
//----------------------------------------

//Mem_dump() - Display all information over the memory manager.
int MemoryManager::Mem_dump()
{
    Node<Segment> *tmp = MemoManager.front();

    mcb->mywin->write_window(mcb->mywin->Misc_Table, "\n------------------\n ---- Memory Dump ----\n");
    for (int i = 0; i < MemoManager.size(); i++)
    {

        sprintf(buffer, " Handler: %d\n", tmp->data.handler);
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

        sprintf(buffer, " Owner:  %d\n", tmp->data.ownerID);
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

        sprintf(buffer, " Base: %d\n", tmp->data.base);
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

        sprintf(buffer, " Limit: %d\n", tmp->data.limit);
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

        sprintf(buffer, " Size: %d\n", tmp->data.size);
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

        sprintf(buffer, " Status: ");
        if (tmp->data.status == FREE)
            strcat(buffer, " FREE\n");
        else
            strcat(buffer, " USED\n");

        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

        sprintf(buffer, " Read_cursor: %d\n", tmp->data.read_cursor);
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

        sprintf(buffer, " Write_cursor: %d\n", tmp->data.write_cursor);
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

        mcb->mywin->write_window(mcb->mywin->Misc_Table, "---------------------------\n");
        tmp = tmp->next;
    }
    mcb->mywin->write_window(mcb->mywin->Misc_Table, "-----------------\n");
    strcat(buffer, " ");
    strcat(buffer,memory);
    strcat(buffer,"\n");
    mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
}

#endif