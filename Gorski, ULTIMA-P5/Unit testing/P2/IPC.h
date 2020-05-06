/*
Filename: IPC.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To define create a inter process communication mechanism (IPC).
                PHASE II
*/
#ifndef IPC_H
#define IPC_H

#include <iostream>
#include <string>
#include <ctime> //So that arrival time is output pretty

#include "LinkedList.h"
#include "Queue.h"
#include "MCB.h"
#include "Semaphore.h"

using namespace std;

#ifndef MSG_TYPE
#define MSG_TYPE
enum MSGTYPE
{
    TEXT,        //No action required.
    SERVICE,     //Request for a service by the task.
    NOTIFICATION // Sent to a task to indicate a service was completed.
};
#endif

struct Message
{
    int src_id;
    int dst_id;
    string arrived_time;
    MSGTYPE msg_type;
    string msg_content;
};

class IPC
{
private:
    MCB *mcb;
    char buffer[256];

public:
    LinkedList<Queue<Message>> mailboxes;
    int createMailbox(int task_id);
    int sendMessage(int src_id, int dst_id, MSGTYPE msg_type, string msg_content);
    void receiveMessage(int task_id);
    int messageCount(int task_id);
    void removeAMailbox(int task_id);
    void deleteAllMailboxes();
    void dumpIPC();
    void dumpMailbox(int task_id);
    void setMCB(MCB *mcb);
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//createMailbox(int) - Create a mailbox queue inside the
//                     linked list Mailboxes so that a
//                     Scheduler::createTask thread can
//                     have its own mailbox "address".
int IPC::createMailbox(int task_id)
{
    Queue<Message> newBox;
    mailboxes.insertBack(newBox, task_id);
    //Return 0 if successful created the box, or -1 if fails
    if (mailboxes.size() > 0)
        return 0;
    else
        return -1;
}
//----------------------------------------

//sendMessage() - Sends a message from a thread source
//                to a thread destination. The message
//                is enqueued in the destination thread's
//                mailbox.
int IPC::sendMessage(int src_id, int dst_id, MSGTYPE msg_type, string msg_content)
{
    time_t now = time(0);
   
    Message newMsg;
    newMsg.arrived_time = asctime(localtime(&now)); //https://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
    newMsg.dst_id = dst_id;
    newMsg.src_id = src_id;
    newMsg.msg_type = msg_type;
    newMsg.msg_content = msg_content;

    //Check if there the destination's mailbox exist
    if (mailboxes.isNode(dst_id))
    { 
        mcb->mysema1->down(src_id);
        mailboxes.searchNode(dst_id)->data.enqueue(newMsg, dst_id);
        mcb->mysema1->up();
        return 0;
    }
    else
    {
        mcb->mywin->write_window(mcb->mywin->Misc_Table, " Impossible to send message, mailbox does not exist!\n");
        return -1;
    }
}
//----------------------------------------

//receiveMessage(int) - Goes into a specific thread mailbox,
//                      retrieves its first message from its
//                      queue by displaying it, and removing
//                      it from its mailbox. This insures
//                      that the most recent message is read.
void IPC::receiveMessage(int task_id)
{
    
    //Check if there a mailbox for the task_id
    if (mailboxes.isNode(task_id) && mailboxes.searchNode(task_id)->data.size() > 0)
    {
        mcb->mywin->write_window(mcb->mywin->Misc_Table, " ...Receiving message...\n");
        Node<Message> *tmp = mailboxes.searchNode(task_id)->data.top();
        sprintf(buffer, "  ______________________________________________________________________________________________________\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
        sprintf(buffer, " |  | SourceID | DestID | Arrived Time                \t| MessageType\t| Content                    \t|\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
        sprintf(buffer, "  ------------------------------------------------------------------------------------------------------\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
        //Treating date so that \n is removed.
        string date;
        for (int i = 0; i < tmp->data.arrived_time.size(); i++)
            if (tmp->data.arrived_time[i] != '\n')
                date[i] = tmp->data.arrived_time[i];
        //--------
        //Treating message type so that display pretty.
        switch (tmp->data.msg_type)
        {
        case 0:
            sprintf(buffer, " |  |    %d \t|    %d \t | %s \t| TEXT \t\t| %s \t\t|\n", tmp->data.src_id, tmp->data.dst_id, date.c_str(), tmp->data.msg_content.c_str());
            break;
        case 1:
            sprintf(buffer, " |  |    %d \t|    %d \t | %s \t| SERVICE \t\t| %s \t\t|\n", tmp->data.src_id, tmp->data.dst_id, date.c_str(), tmp->data.msg_content.c_str());
            break;
        case 2:
            sprintf(buffer, " |  |    %d \t|    %d \t | %s \t| NOTIFICATION \t\t| %s \t\t|\n", tmp->data.src_id, tmp->data.dst_id, date.c_str(), tmp->data.msg_content.c_str());
            break;
        }
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
        //---------------
        sprintf(buffer, "  ------------------------------------------------------------------------------------------------------\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
        mcb->mysema1->down(task_id);
        mailboxes.searchNode(task_id)->data.dequeue(); //if yes, reads the first message and removes it
        mcb->mysema1->up();
    }
    else
    {
        mcb->mywin->write_window(mcb->mywin->Misc_Table, " Impossible to obtain message, mailbox do not exist or no messages!\n");
    }
}
//----------------------------------------

//messageCount(int) - Return how many messages there is
//                    in a thread's mailbox.
int IPC::messageCount(int task_id)
{
    if (mailboxes.isNode(task_id))
        return mailboxes.searchNode(task_id)->data.size();
    else
        return -1;
}
//----------------------------------------

//removeAMailbox(int) - Delete a specific thread mailbox.
void IPC::removeAMailbox(int task_id)
{
    if (mailboxes.isNode(task_id))
    {
        //Remove all messages from the mailbox.
        while (mailboxes.searchNode(task_id)->data.size() > 0)
            mailboxes.searchNode(task_id)->data.dequeue();
    }
}

//deleteAllMailboxes() - Delete all thread's mailboxes.
void IPC::deleteAllMailboxes()
{
    while (mailboxes.size() > 0)
        mailboxes.removeBack();
}
//----------------------------------------

//dumpIPC() - Output all mailboxes within IPC.
void IPC::dumpIPC()
{
    for (int i = 0; i < mailboxes.size(); i++)
    {
        dumpMailbox(i);
    }
}
//----------------------------------------

//dumpMailbox(int) - Output a thread specific mailbox.
void IPC::dumpMailbox(int task_id)
{
    sprintf(buffer, "\n ---- MAILBOX Thread # %d -----\n", task_id);
    mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);

    mcb->mysema1->down(task_id);
    Node<Message> *tmp = mailboxes.searchNode(task_id)->data.top();
    mcb->mysema1->up();
    
    sprintf(buffer, "  ______________________________________________________________________________________________________\n");
    mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
    sprintf(buffer, " | # | SourceID | DestID | Arrived Time                \t| MessageType\t| Content                    \t|\n");
    mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
    sprintf(buffer, "  ------------------------------------------------------------------------------------------------------\n");
    mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
    for (int j = 0; j < mailboxes.searchNode(task_id)->data.size(); j++)
    {
        //Treating date so that \n is removed.
        string date;
        for (int i = 0; i < tmp->data.arrived_time.size(); i++)
            if (tmp->data.arrived_time[i] != '\n')
                date[i] = tmp->data.arrived_time[i];
        //--------
        //Treating message type so that display pretty.
        switch (tmp->data.msg_type)
        {
        case 0:
            sprintf(buffer, " | %d |    %d \t|    %d \t | %s \t| TEXT \t\t| %s \t\t|\n", j, tmp->data.src_id, tmp->data.dst_id, date.c_str(), tmp->data.msg_content.c_str());
            break;
        case 1:
            sprintf(buffer, " | %d |    %d \t|    %d \t | %s \t| SERVICE \t\t| %s \t\t|\n", j, tmp->data.src_id, tmp->data.dst_id, date.c_str(), tmp->data.msg_content.c_str());
            break;
        case 2:
            sprintf(buffer, " | %d |    %d \t|    %d \t | %s \t| NOTIFICATION \t\t| %s \t\t|\n", j, tmp->data.src_id, tmp->data.dst_id, date.c_str(), tmp->data.msg_content.c_str());
            break;
        }
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
        //---------------
        sprintf(buffer, "  ------------------------------------------------------------------------------------------------------\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table, buffer);
        tmp = tmp->next;
    }

}
//----------------------------------------

//setMCB(MCB) - Register the Master Control Block
//              mechanism so that Semaphore class
//              have access to other mechanisms
//              such as the IPC, Scheduler...
void IPC::setMCB(MCB *mcb)
{
    this->mcb = mcb;
}
//----------------------------------------
#endif