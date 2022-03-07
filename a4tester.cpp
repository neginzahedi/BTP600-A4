/****************************************************/
/*                                                  */
/*  BTP600                                          */
/*  a4 tester                                       */
/*                                                  */
/*  To compile:                                     */
/*      c++ a4tester.cpp a4.cpp -std=c++0x          */
/*                                                  */
/****************************************************/
#include "a4.h"
#include <iostream>
void printObservers(Observer* obs[],int numObservers);
bool test1(std::string& msg);
bool test2(std::string& msg);
bool test3(std::string& msg);
typedef bool (*TestPtr)(std::string&);
int main(void){
    const int numTests=3;
    std::string msg;
    TestPtr runTest[numTests]={test1, test2, test3};
    std::string testSummary[numTests]={
            "Singleton Test",
            "Observer Test",
            "Memento Test"
    };
    bool result;
    int numPassed=0;
    for(int i=0;i<numTests;i++){
        std::cout << "Test " << i+1 <<" - "<< testSummary[i] << ": ";
        result=runTest[i](msg);
        if(!result){
            std::cout << "failed: " << msg << std::endl;
        }
        else{
            numPassed++;
            std::cout << " passed!" << std::endl;
        }
    }
    if(numPassed==numTests){
        std::cout << "Congratulations, your assignment has passed testing." << std::endl;
        return 0;
    }
    else{
        std::cout << "Looks like you still have some work to do." << std::endl;
        return 1;
    }
}


bool test1(std::string& msg){
    bool rc;
    Timer* t1=Timer::getInstance();
    t1->start();
    int first,second;
    while(t1->currtime()<1.5);
    first=t1->currtime();
    Timer* t2=Timer::getInstance();
    second=t2->currtime();
    if(first!=second){
        msg="Test 1a: failed.  Timer needs to be a singleton";
        rc=false;
    }
    else{
        t1->stop();
        if(!t2->isPaused()){
            msg="Test 1b: failed.  Timer is a singleton, all instances are the same instance";
            rc=false;
        }
        else{
            rc=true;
        }
    }
    return rc;
}
bool test2(std::string& msg){

    Timer* theTimer=Timer::getInstance();
    Observer* obs[6]={
            new Under(5), new Over(2), new Status(),
            new Under(2), new Over(5), new Status()
    };
    bool results[30]={
            true,false,false,                       //initial
            true, false, false,                     //1.1
            true,true,false,false,false, false,     //2.1
            true,true,false,false,true, false,      //5.1 (note that first 3 are detached)
            true,true,false,false,true, true        //5.1,paused (note that first 3 are detached)

    };
    int curr=0;
    bool isGood=true;
    for(int i=0;i<3;i++){
        theTimer->attach(obs[i]);
    }
    theTimer->reset();
    theTimer->start();
    theTimer->notify();
    for(int i=0;i<3;i++){
        if(results[curr++]!=obs[i]->state()){
            isGood=false;
            msg="test 2a: object's state() return is not correct";
        }
    }
    if(isGood){
        while(theTimer->currtime() < 1.1);
        theTimer->notify();
        for(int i=0;i<3;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 2b: object's state() return is not correct";
            }
        }

    }
    if(isGood){
        theTimer->attach(obs[3]);
        theTimer->attach(obs[4]);
        theTimer->attach(obs[5]);
        while(theTimer->currtime() < 2.1);
        theTimer->notify();
        for(int i=0;i<6;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 2c: object's state() return is not correct";

            }
        }
    }
    if(isGood){
        theTimer->detach(obs[0]);
        theTimer->detach(obs[1]);
        theTimer->detach(obs[2]);

        while(theTimer->currtime() < 5.1);
        theTimer->notify();
        for(int i=0;i<6;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 2d: object's state() return is not correct";
            }
        }

    }
    if(isGood){
        theTimer->stop();
        theTimer->notify();
        for(int i=0;i<6;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 2e: object's state() return is not correct";
            }
        }

    }
    return isGood;
}
bool test3(std::string& msg){
    Timer* theTimer=Timer::getInstance();
    theTimer->reset();
    Observer* obs[6]={
            new Over(1), new Under(2), new Under(3),
            new Under(4), new Under(5), new Status()
    };
    bool results[300]={
            false,true,true,true,true,false,     //~0 unpaused
            true, true,true,true,true,false,     //~1.1 unpaused
            true, true,true,true,true,true,      //~1.1 paused
            true, false,true,true,true,false,    //~2.1 unpaused
            true, false,true,true,true,true,     //~2.1 paused
            true, false,true,true,true,false,    //~2.1 unpaused - restored from memento state
            true,false,false,true,true,false,    //~3.1 unpaused
            true,false,false,false,true,false,    //~4.1 unpaused
            true,false,false,false,false,false,    //~5.1 unpaused
            true,false,false,false,false,false,    //~6.1 unpaused
            true,false,false,false,false,false,    //~5.1 unpaused
            true,false,false,false,true,false,    //~4.1 unpaused
            true,false,false,true,true,false,    //~3.1 unpaused
            true, true,true,true,true,true,      //~1.1 paused
            true, true,true,true,true,false     //~1.1 unpaused
    };
    UndoStack undos;

    int curr=0;
    bool isGood=true;

    for(int i=0;i<6;i++){
        theTimer->attach(obs[i]);
    }
    //saved 1, time 0, paused
    undos.save(theTimer->createMemento());
    theTimer->start();
    theTimer->notify();
    for(int i=0;i<6;i++){
        if(results[curr++]!=obs[i]->state()){
            isGood=false;
            msg="test 3a: object's state() return is not correct.  Initialization error perhaps?";
        }
    }
    if(isGood){
        while(theTimer->currtime() < 1.1);
        theTimer->notify();
        //saved 2, time  ~1.1, unpaused
        undos.save(theTimer->createMemento());
        for(int i=0;i<6;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 3b: object's state() return is not correct.";
            }
        }

    }
    if(isGood){
        theTimer->stop();
        theTimer->notify();
        //saved 3, time  ~1.1, paused
        undos.save(theTimer->createMemento());
        for(int i=0;i<6;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 3c: object's state() return is not correct.";
            }
        }
    }
    if(isGood){
        theTimer->start();
        //loop to over 2 seconds
        while(theTimer->currtime() < 2.1);
        theTimer->notify();
        //saved 4, time  ~2.1, unpaused
        undos.save(theTimer->createMemento());
        for(int i=0;i<6;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 3d: object's state() return is not correct.";
            }
        }

    }
    if(isGood){
        theTimer->stop();
        theTimer->notify();
        for(int i=0;i<6;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 3e: object's state() return is not correct.";
            }
        }
    }
    if(isGood){
        //save 4 used for restore, previous save still stored
        //restore most recent, memento should change state to ~2.1, unpaused
        theTimer->restoreFromMemento(undos.getMemento());
        theTimer->notify();
        for(int i=0;i<6;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 3f: object's state() return is not correct.  this state was restored from memento";
            }
        }
    }
    if(isGood){
        //loop 1 more seconds
        while(theTimer->currtime() < 3.1);
        theTimer->notify();
        //save 5, ~3.1 unpaused (4th memento stored)
        undos.save(theTimer->createMemento());
        for(int i=0;i<6;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 3g: object's state() return is not correct.";
            }
        }
    }
    if(isGood){
        //loop 1 more seconds
        while(theTimer->currtime() < 4.1);
        theTimer->notify();
        //save 6, ~4.1 unpaused (5th memento Stored)
        undos.save(theTimer->createMemento());
        for(int i=0;i<6;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 3h: object's state() return is not correct.";
            }
        }
    }
    if(isGood){
        //loop 1 more seconds
        while(theTimer->currtime() < 5.1);
        theTimer->notify();
        //save 7, ~5.1, unpaused ... watch out here.  read specs carefully
        undos.save(theTimer->createMemento());
        for(int i=0;i<6;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 3i: object's state() return is not correct.";
            }
        }
    }
    if(isGood){
        //loop 1 more seconds
        while(theTimer->currtime() < 6.1);
        theTimer->notify();
        for(int i=0;i<6;i++){
            if(results[curr++]!=obs[i]->state()){
                isGood=false;
                msg="test 3j: object's state() return is not correct.";
            }
        }
    }
    if(isGood){
        for(int i=0;i<5;i++){
            theTimer->restoreFromMemento(undos.getMemento());
            theTimer->notify();
            for(int j=0;j<6;j++){
                if(results[curr++]!=obs[j]->state()){
                    isGood=false;
                    msg="test 3k: object's state() return is not correct.";
                }
            }
        }
    }
    if(isGood){
        if(theTimer->restoreFromMemento(undos.getMemento())){
            isGood=false;
            msg = "test 3l: undo stack should only have 5 items in it at most.\n Restore should not have been successful from empty stack";
        }
    }
    return isGood;
}