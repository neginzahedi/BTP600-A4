#include <time.h>
#include <vector>   //store observers
#include <list>     //store mementos

#define POSIX 1
#define WINDOWS 2
//Generally you want to use the POSIX version.  Its better, more accurate.  But, the
//function won't work on non-posix systems... so switch to WINDOWS if you are using VS
//The matrix version is POSIX
#define PLATFORM POSIX

/*This class represents a timer.  A timer can be used to calculate the
time it took for a piece of code to be run.  It uses the actual time in
processing as opposed to the wall clock so that differences in load on a
system will not affect the timing of the code.  This object is like a stop
watch.

When initialized, the timer is set to 0.  In order to start timing, the
start() function must be used.

The following are the member functions of Timer and their usage:

  start();  starts the clock running from the current time
  currtime();  returns the current time
  stop();  stops the clock without reseting its value
  reset();  stops the clock and resets the timer to 0
*/


class Observer;

class Subject {
    std::vector<Observer *> observersList;

public:
    Subject();

    void attach(Observer *o);

    void detach(const Observer *o);

    void notify();
};

//Abstract Observer
class Observer {
public:
    virtual void update() = 0;

    virtual bool state() const = 0;
};

//Concrete observers. subject of these observers is the Timer(a singleton)
//the
class Under : public Observer {
    double threshold;
    double underTime;
public:
    Under(double threshold);

    virtual void update();

    virtual bool state() const;
};

class Over : public Observer {
    double threshold;
    double overTime;
public:
    Over(double threshold);

    virtual void update();

    virtual bool state() const;

};

class Status : public Observer {
    bool paused;
public:
    Status();

    virtual void update();

    virtual bool state() const;
};

/*This class is completely empty..
It is up to you to define its interface
and implementation with accordance to
the Memento Pattern*/
class Memento {
private:
    double storedTime;
    bool pausedStatus;
public:
    Memento();

    ~Memento();

    bool getPaused();

    double getTime();
};

//the Undo Stack-Caretaker
class UndoStack {
    std::list<Memento *> mementoStack;
public:
    UndoStack();

    void save(Memento *m);

    Memento *getMemento();
};

//Observer Pattern-concrete subject
//Memento Pattern-originator
class Timer : public Subject {
#if PLATFORM == WINDOWS
    clock_t starttime_;
    clock_t stoptime_;
#else
    struct timespec starttime_;
    struct timespec stoptime_;
#endif
    double elapsedTime_;
    bool isstop_;

    //Singleton pattern
    Timer();

    static Timer *singletonInstance;

    //Memento Pattern
    Memento *createdMemento;

public:
    void reset();

    void start();

    void stop();

    bool isPaused();

    double currtime();

    double starttime();

    //Singleton pattern
    static Timer *getInstance();

    //Memento Pattern
    Memento *createMemento();

    bool restoreFromMemento(Memento *m);
};