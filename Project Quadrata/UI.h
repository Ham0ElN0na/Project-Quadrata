
#ifndef UI_H
#define UI_H

#include <string>
#include "SessionManager.h"
using namespace std;

class ui {
public:
    void displaywelcomescreen();
    int displaymainmenu();
    void userlogin(SessionManager& session);
    void userSignUp();
};

#endif 

