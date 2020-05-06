/*
Filename: IPC.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To define create a inter process communication mechanism (IPC).
*/
#ifndef IPC_H
#define IPC_H

#include<iostream>
#include<string>
#include<ctime> //So that arrival time is output pretty

#include "LinkedList.h"
#include "Queue.h"

using namespace std;

#ifndef MSG_TYPE
#define MSG_TYPE
enum MSGTYPE
{
    TEXT, //No action required.
    SERVICE, //Request for a service by the task.
    NOTIFICATION// Sent to a task to indicate a service was completed.
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
    mailboxes.insertBack(newBox,task_id);
    //Return 0 if successful created the box, or -1 if fails
    if(mailboxes.size() > 0)
        return 0;
    else
        return -1;
}
//----------------------------------------

//sendMessage() - Sends a message from a thread source
//                to a thread destination. The message
//                is enqueued in the destination thread's
//                mailbox.
int IPC::sendMessage(int src_id, int dst_id, MSGTYPE msg_type,string msg_content)
{
    time_t now = time(0);

    Message newMsg;
    newMsg.arrived_time = asctime(localtime(&now)); //https://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
    newMsg.dst_id = dst_id;
    newMsg.src_id = src_id;
    newMsg.msg_type = msg_type;
    newMsg.msg_content = msg_content;

    //Check if there the destination's mailbox exist
    if(mailboxes.isNode(dst_id))
    {
        mailboxes.searchNode(dst_id)->data.enqueue(newMsg,dst_id);
        return 0;
    }
    else
    {
        cout << "Impossible to send message, mailbox does not exist!" << endl;
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
    if(mailboxes.isNode(task_id))
    {
        cout << "\n\n----MESSAGE---" << endl;
        cout << "Source ID: " << mailboxes.searchNode(task_id)->data.top()->data.src_id << endl;
        cout << "Dest ID: " << mailboxes.searchNode(task_id)->data.top()->data.dst_id << endl;
        cout << "Arrival Time: " << mailboxes.searchNode(task_id)->data.top()->data.arrived_time;
        cout << "Message Type: ";
        switch(mailboxes.searchNode(task_id)->data.top()->data.msg_type)
        {
            case 0:  cout << "TEXT" << endl; break;
            case 1:  cout << "SERVICE" << endl; break;
            case 2:  cout << "NOTIFICATION" << endl; break;
        }
        cout << "Content: " << mailboxes.searchNode(task_id)->data.top()->data.msg_content << endl;
        cout << "----END MESSAGE---\n\n" << endl;
        mailboxes.searchNode(task_id)->data.dequeue(); //if yes, reads the first message and removes it
    }
    else
    {
        cout << "Impossible to obtain message, mailbox do not exist!" << endl;
    } 
}
//----------------------------------------

//messageCount(int) - Return how many messages there is
//                    in a thread's mailbox.
int IPC::messageCount(int task_id)
{
    if(mailboxes.isNode(task_id))
        return mailboxes.searchNode(task_id)->data.size();
    else
        return -1;
}
//----------------------------------------

//removeAMailbox(int) - Delete a specific thread mailbox.
void IPC::removeAMailbox(int task_id)
{
     if(mailboxes.isNode(task_id))
     {
        //Remove all messages from the mailbox.
        while(mailboxes.searchNode(task_id)->data.size() > 0 )
            mailboxes.searchNode(task_id)->data.dequeue();
    }
    mailboxes.removeNode(task_id);
}

//deleteAllMailboxes() - Delete all thread's mailboxes.
void IPC::deleteAllMailboxes()
{
    while(mailboxes.size() > 0)
        mailboxes.removeBack();
}
//----------------------------------------

//dumpIPC() - Output all mailboxes within IPC.
void IPC::dumpIPC()
{
    for(int i = 0; i < mailboxes.size(); i++)
    {
        dumpMailbox(i);
        cout << "--------------------------------------------------" << endl;
    }
}
//----------------------------------------

//dumpMailbox(int) - Output a thread specific mailbox.
void IPC::dumpMailbox(int task_id)
{
    cout << "---- MAILBOX Thread # " << task_id << " -----" << endl;
    Node<Message> *tmp = mailboxes.searchNode(task_id)->data.top();
    for(int j = 0; j < mailboxes.searchNode(task_id)->data.size(); j++)
    {
        cout << "\n\n----MESSAGE---" << endl;
        cout << "Source ID: " << tmp->data.src_id << endl;
        cout << "Dest ID: " << tmp->data.dst_id << endl;
        cout << "Arrival Time: " << tmp->data.arrived_time;
        cout << "Message Type: ";
        switch(tmp->data.msg_type)
        {
            case 0:  cout << "TEXT" << endl; break;
            case 1:  cout << "SERVICE" << endl; break;
            case 2:  cout << "NOTIFICATION" << endl; break;
        }
        cout << "Content: " << tmp->data.msg_content << endl;
        cout << "----END MESSAGE---\n\n" << endl;
        tmp = tmp->next;
    }
    cout << "--------------------------------------------------" << endl;
}
//----------------------------------------

#endif