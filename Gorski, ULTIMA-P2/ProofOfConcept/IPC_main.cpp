#include<iostream>
#include <ctime>
using namespace std;

enum MSGTYPE
{
    TEXT,
    SERVICE,
    NOTIFICATION
};

struct Message
{
    int src_id;
    int dst_id;
    string arrived_time;
    MSGTYPE msg_type;
    string msg_content;
};

#include "LinkedList.h"
#include "Queue.h"

int main()
{
    time_t now = time(0);

    LinkedList<Queue<Message>> mailboxes;
    Queue<Message> box0;
    Queue<Message> box1;
    Queue<Message> box2;
    mailboxes.insertBack(box0,0);
    mailboxes.insertBack(box1,1);
    mailboxes.insertBack(box2,2);
    
    //Testing if was created right
    cout << "Number of mailboxes: " << mailboxes.size() << endl;


    Message newMsg;
    newMsg.arrived_time = asctime(localtime(&now)); //https://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
    newMsg.dst_id = 0;
    newMsg.src_id = 1;
    newMsg.msg_type = TEXT;
    newMsg.msg_content = "Hello";
    mailboxes.front()->data.enqueue(newMsg,0);

    //Accessing variables within my Message
    cout << "\n\n----MESSAGE---" << endl;
    cout << "Source ID: " << mailboxes.front()->data.top()->data.src_id << endl;
    cout << "Dest ID: " << mailboxes.front()->data.top()->data.dst_id << endl;
    cout << "Arrival Time: " << mailboxes.front()->data.top()->data.arrived_time;
    cout << "Message Type: ";
    switch(mailboxes.front()->data.top()->data.msg_type)
    {
        case 0:  cout << "TEXT" << endl; break;
        case 1:  cout << "SERVICE" << endl; break;
        case 2:  cout << "NOTIFICATION" << endl; break;
    }
    cout << "Content: " << mailboxes.front()->data.top()->data.msg_content << endl;
    cout << "----END MESSAGE---\n\n" << endl;

    Node<Message> *tmp = mailboxes.front()->data.top();
    cout << "Different way to go in message: ";
    cout << tmp->data.msg_content << endl;

    cout << "My sub mailbox 1 size: " << mailboxes.searchNode(1)->data.size() << endl;

    cout << "Test if there is such a mailbox: " << mailboxes.isNode(4) << endl;


    mailboxes.searchNode(2)->data.enqueue(newMsg,2);
    cout << mailboxes.searchNode(2)->data.size();

    return 0;
}