#include "a4.h"
#include <iostream>
#include <stdio.h>

Subject::Subject() {}

//stores address of observer into observer list
void Subject::attach(Observer *o) {
    observersList.push_back(o);
}

//detaches the matching address observer
void Subject::detach(const Observer *o) {
    auto itr = std::find(observersList.begin(), observersList.end(), o);
    if (itr != observersList.end()) {
        observersList.erase(itr);
    }
}

//update observers internal state
void Subject::notify() {
    for (Observer *o: observersList) {
        o->update();
    }
}

Under::Under(double threshold) {
    this->threshold = threshold;
}

void Under::update() {
    this->underTime = (Timer::getInstance()->currtime());
}

bool Under::state() const {
    if (this->underTime < this->threshold) {
        return true;
    } else {
        return false;
    }
}

Over::Over(double threshold) {
    this->threshold = threshold;
}

void Over::update() {
    this->overTime = (Timer::getInstance()->currtime());
}

bool Over::state() const {
    if (this->overTime > this->threshold) {
        return true;
    } else {
        return false;
    }
}

Status::Status() {}

void Status::update() {
    paused = Timer::getInstance()->isPaused();
}

bool Status::state() const {
    if (paused)
        return true;
    else
        return false;
}

//memento
Memento::Memento() {
    //elapsed time
    storedTime = (Timer::getInstance()->currtime());
    //paused status
    pausedStatus = Timer::getInstance()->isPaused();
}

Memento::~Memento() {}

bool Memento::getPaused() {
    return this->pausedStatus;
}

double Memento::getTime() {
    return this->storedTime;
}

UndoStack::UndoStack() {}

//store up to 5 mementos
void UndoStack::save(Memento *m) {
    if (mementoStack.size() == 5) {
        //remove oldest object
        mementoStack.pop_back();
        mementoStack.push_front(m);
    } else {
        mementoStack.push_front(m);
    }
}

//return address of the newest memento
Memento *UndoStack::getMemento() {
    if (mementoStack.empty()) {
        return nullptr;
    } else {
        Memento *temp;
        temp = mementoStack.front();
        mementoStack.pop_front();
        return temp;
    }
}

Memento *Timer::createMemento() {
    createdMemento = new Memento();
    return createdMemento;
}

bool Timer::restoreFromMemento(Memento *m) {

    /*this block of code gets the current time depending on platform
    .. you will need this to restore your timer */
#if PLATFORM == WINDOWS
    clock_t curr=clock();
#else
    struct timespec curr;
    clock_gettime(CLOCK_MONOTONIC_RAW, &curr);
#endif


    if (m != nullptr) {
        createdMemento = m;
        this->starttime_ = curr;
        this->stoptime_ = curr;
        this->isstop_ = createdMemento->getPaused();
        this->elapsedTime_ = createdMemento->getTime();
        return true;
    } else {
        return false;
    }

}

// ADDED: Singleton Pattern
Timer *Timer::singletonInstance = nullptr;;

Timer *Timer::getInstance() {
    if (singletonInstance == nullptr) {
        singletonInstance = new Timer();
    }
    return singletonInstance;
}

/**************************************************************/
/*   Nothing below this comment should be modified            */
/**************************************************************/

/* Timer class functions.
  start();  starts the clock running from the current time
  currtime();  returns the current time
  stop();  stops the clock without reseting its value
  reset();  stops the clock and resets the timer to 0
  isPaused(); returns true if the timer is stopped, false otherwise
*/

Timer::Timer() {
    elapsedTime_ = 0;
    isstop_ = true;

#if PLATFORM == WINDOWS
    starttime_=0;
    stoptime_=0;
#else
    clock_gettime(CLOCK_MONOTONIC_RAW, &starttime_);
    stoptime_ = starttime_;
#endif


}

void Timer::reset() {
#if PLATFORM == WINDOWS
    starttime_=0;
    stoptime_=0;
#else
    clock_gettime(CLOCK_MONOTONIC_RAW, &starttime_);
    stoptime_ = starttime_;
#endif
    elapsedTime_ = 0;
    isstop_ = true;
}

void Timer::start() {
#if PLATFORM == WINDOWS
    clock_t curr=clock();
    if(isstop_){
        starttime_=curr;
        isstop_=false;
    }
    else{
        starttime_=curr;   //its like hitting reset and start
        stoptime_=starttime_;
        elapsedTime_=0;
    }
#else
    struct timespec curr;
    clock_gettime(CLOCK_MONOTONIC_RAW, &curr);
    if (isstop_) {
        starttime_ = curr;
        isstop_ = false;
    } else {
        clock_gettime(CLOCK_MONOTONIC_RAW, &starttime_);
        stoptime_ = starttime_;
        elapsedTime_ = 0;
    }
#endif

}

void Timer::stop() {
#if PLATFORM == WINDOWS
    stoptime_=clock();
    elapsedTime_+=stoptime_-starttime_;
#else
    clock_gettime(CLOCK_MONOTONIC_RAW, &stoptime_);
    elapsedTime_ += (stoptime_.tv_nsec - starttime_.tv_nsec) / 1000000000.0 +
                    (stoptime_.tv_sec - starttime_.tv_sec);
#endif
    isstop_ = true;
}

double Timer::currtime() {
    double rc;
#if PLATFORM == WINDOWS
    clock_t curr=clock();
    clock_t elapse=(isstop_)?elapsedTime_:(elapsedTime_+curr-starttime_);
    rc= double(elapse)/CLOCKS_PER_SEC;
#else
    struct timespec curr;
    clock_gettime(CLOCK_MONOTONIC_RAW, &curr);
    if (isstop_) {
        rc = elapsedTime_;
    } else {
        rc = elapsedTime_ + (curr.tv_nsec - starttime_.tv_nsec) / 1000000000.0 +
             (curr.tv_sec - starttime_.tv_sec);
    }
#endif
    return rc;
}

double Timer::starttime() {
#if PLATFORM == WINDOWS
    return (double)starttime_;
#else
    return starttime_.tv_nsec / 1000000000.0 + starttime_.tv_sec;
#endif
}

bool Timer::isPaused() {
    return isstop_;
}
