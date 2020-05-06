/*
Filename: Semaphore.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To implement a small GUI for 
            Ultima using curses library.
*/
#ifndef WINDOW_H
#define WINDOW_H

#include <curses.h>
#include <iostream>
#include <curses.h>
#include <stdarg.h>
#include <fcntl.h>

#include "MCB.h"
class MCB;

using namespace std;

class Window
{
private:
    MCB *mcb;
    pthread_mutex_t mux_Win;

public:
    WINDOW *Heading_Win;
    WINDOW *Log_Win;
    WINDOW *Console_Win;
    WINDOW *Misc_Table;
    Window();
    ~Window();
    WINDOW *create_window(int height, int width, int starty, int startx);
    void write_window(WINDOW *Win, const char *text);
    void write_window(WINDOW *Win, int x, int y, const char *text);
    void display_help(WINDOW *Win);
    void setMCB(MCB *mcb);
    void cont_help(WINDOW *Win);
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//  create a windows to display thread data in
//  create a new windows:   WINDOW * win = newwin(nlines, ncols, y0, x0);

//Constructor
Window::Window()
{
    //Initializing mutex
    pthread_mutex_init(&mux_Win, NULL);

    // Start nCurses
    initscr();

    //Enable terminal color
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    wbkgd(Misc_Table, COLOR_PAIR(1));
    //End of terminal color.

    refresh();

    int Y, X;
    int Max_X, Max_Y;
    getmaxyx(stdscr, Max_Y, Max_X); //Get screen size
    wprintw(stdscr, "Current Y = %d , Current X = %d\nGiovanna Gorski - C435 Operating Systems", Y, X);
    refresh();

    //Starting Heading Window
    Heading_Win = create_window(12, 83, 3, 2);
    write_window(Heading_Win, 2, 28, "ULTIMA 2.0 (Spring 2020)");
    write_window(Heading_Win, 9, 2, "Press 'q' or Ctrl-C to exit the program...");
    //end of Heading Window

    //Starting Log_Win
    Log_Win = create_window(15, 60, 30, 2);
    write_window(Log_Win, 1, 5, ".......Log....\n");
    //----------------------------------------
    Console_Win = create_window(15, 23, 30, 62);
    write_window(Console_Win, 1, 1, "....Console....\n");
    write_window(Console_Win, 2, 1, "Ultima # ");

    Misc_Table = create_window(42, 83, 3, 85);
    write_window(Misc_Table, 1, 1, "...Miscellaneous...\n");

    cbreak();
    noecho();
    nodelay(Console_Win, true);
}
//----------------------------------------

//Destructor
Window::~Window()
{
    endwin(); //End the curses window
    pthread_mutex_destroy(&mux_Win);
}
//----------------------------------------

//create_window() - To create a box window.
WINDOW *Window::create_window(int height, int width, int starty, int startx)
{
    WINDOW *Win;
    Win = newwin(height, width, starty, startx);

    scrollok(Win, TRUE); // Allow scrolling of the window
    pthread_mutex_lock(&mux_Win);
    scroll(Win);    // scroll the window
    box(Win, 0, 0); // 0, 0 gives default characters
                    // for the vertical and horizontal lines
    wrefresh(Win);  // draw the window
    wclear(Win);
    pthread_mutex_unlock(&mux_Win);
    return Win;
}
//----------------------------------------

//write_window() - To write text in specific window.
void Window::write_window(WINDOW *Win, const char *text)
{
    pthread_mutex_lock(&mux_Win);
    wprintw(Win, text);
    box(Win, 0, 0);
    wrefresh(Win); // draw the window
    pthread_mutex_unlock(&mux_Win);
}
//----------------------------------------

//write_window() -  To write text in specific window location.
void Window::write_window(WINDOW *Win, int x, int y, const char *text)
{
    pthread_mutex_lock(&mux_Win);
    mvwprintw(Win, x, y, text);
    box(Win, 0, 0);
    wrefresh(Win); // draw the window
    pthread_mutex_unlock(&mux_Win);
}
//----------------------------------------

//display_help() - To construct a well formated
//                 help menu in console window.
void Window::display_help(WINDOW *Win)
{
    wclear(Win);
    write_window(Win, 1, 1, "...Help...");
    write_window(Win, 2, 1, "1= Kill 1");
    write_window(Win, 3, 1, "2= Kill 2");
    write_window(Win, 4, 1, "3= Kill 3");
    write_window(Win, 5, 1, "n= Next Menu");
    write_window(Win, 6, 1, "c= clear screen");
    write_window(Win, 7, 1, "h= help screen");
    write_window(Win, 8, 1, "q= Quit");
}
//------------------------------------------------------

//cont_help() - To construct a well formated
//                 help menu in console window.
void Window::cont_help(WINDOW *Win)
{
    wclear(Win);
    write_window(Win, 1, 1, "...Next Menu...");
    write_window(Win, 2, 1, "s= Semaphore");
    write_window(Win, 3, 1, "i= IPC");
    write_window(Win, 4, 1, "g= Garbage Collect");
    write_window(Win, 5, 1, "m= Memory Manager");
    write_window(Win, 6, 1, "b= Back to previous");
}
//----------------------------------------

//setMCB(MCB) - Register the Master Control Block
//              mechanism so that Semaphore class
//              have access to other mechanisms
//              such as the IPC, Scheduler...
void Window::setMCB(MCB *mcb)
{
    this->mcb = mcb;
}
//----------------------------------------
#endif