#include "Server.h"
#include "Job.h"
#include "SelectionStrategies.h"
#include "PassiveQueue.h"
#include <iostream>
#include <omnetpp.h>

namespace queueing {

Define_Module(Server);

Server::~Server()
{
    delete selectionStrategy;
    delete jobServiced;
    cancelAndDelete(endServiceMsg);
}

void Server::initialize()
{
    busySignal = registerSignal("busy");
    emit(busySignal, false);

    endServiceMsg = new cMessage("end-service");
    jobServiced = nullptr;
    allocated = false;
    selectionStrategy = SelectionStrategy::create(par("fetchingAlgorithm"), this, true);
    if (!selectionStrategy)
        throw cRuntimeError("invalid selection strategy");

    N = 5;

    customersServedQ1 = 0;
    customersServedQ2 = 0;

    fromQueue1 = true;  // Start serving from Queue 1
    free = false;

    customersServedQ1buffer = 0;
    strategy = 0;
}

int Exact_N(int N, int *customersServedQ1, int *customersServedQ2, bool *fromQueue1){
    // switch to the other queue if N customers are served
    if (*customersServedQ1 == N) {
        *fromQueue1 = false;  // toggle between queues
        *customersServedQ1 = 0;       // reset the counter
    }
    else if (*customersServedQ2 == N){
        *fromQueue1 = true;  // toggle between queues
        *customersServedQ2 = 0;       // reset the counter
    }

    // select the next queue based on the current state
    int k = *fromQueue1 ? 0 : 2;  // queue index

    return k;

}

int N_Limited(int N, int *customersServedQ1, int *customersServedQ2, bool *fromQueue1, bool *isQ1Empty, bool *isQ2Empty, int *customersServedQ1buffer) {
    // switch to the other queue if N customers are served
    if ((*customersServedQ1 == N || *isQ1Empty) && *fromQueue1) {
        *customersServedQ1buffer = *customersServedQ1;
        *fromQueue1 = false;  // toggle between queues
        *customersServedQ1 = 0;       // reset the counter
    }
    else if ((*customersServedQ2 == N || *customersServedQ2 == *customersServedQ1buffer) && !*fromQueue1){
        *fromQueue1 = true;  // toggle between queues
        *customersServedQ2 = 0;       // reset the counter
        *customersServedQ1buffer = 0;
    }

    // select the next queue based on the current state
    int k = *fromQueue1 ? 0 : 2;  // queue index

    return k;

}

bool checkEmptyQueue(PassiveQueue *Q, int num){
    bool isQEmpty;
    if (!Q) {
           EV << "Error: Unable to find Q"<< num <<"!" << endl;
           // Error handling if Q1 is not found
           }
       else {
           // Now you can call the isEmptyQueue() method on q1 and use the returned value as you wish
           isQEmpty = Q->isEmptyQueue();
           return isQEmpty;
       }
    return -1;
}

void Server::handleMessage(cMessage *msg)
{
    free = false;

    PassiveQueue *Q1 = dynamic_cast<PassiveQueue *>(getModuleByPath("^.Q1"));
    isQ1Empty = checkEmptyQueue(&*Q1, 1);

    PassiveQueue *Q2 = dynamic_cast<PassiveQueue *>(getModuleByPath("^.Q2"));
    isQ2Empty = checkEmptyQueue(&*Q2, 2);

    if (msg == endServiceMsg) {
        ASSERT(jobServiced != nullptr);
        ASSERT(allocated);
        simtime_t d = simTime() - endServiceMsg->getSendingTime();
        jobServiced->setTotalServiceTime(jobServiced->getTotalServiceTime() + d);

        if (!fromQueue1) { // If the job is going to the sink
            jobServiced->V = uniform(14, 22);
            EV << "V è: " << jobServiced->V << endl;
        }

        if (fromQueue1){
            send(jobServiced, "out");
        }
        else {
            send(jobServiced, "out2");
        }
        jobServiced = nullptr;

        int k = 0;

        if (strategy == 0)
            k = Exact_N(N, &customersServedQ1, &customersServedQ2, &fromQueue1);
        else
            k = N_Limited(N, &customersServedQ1, &customersServedQ2, &fromQueue1, &isQ1Empty, &isQ2Empty, &customersServedQ1buffer);

        if(!isQ1Empty || k == 2){
            allocated = false;
            emit(busySignal, false);

            int servingQueue = 1;

            if (k == 0){
                servingQueue = 1;
            }
            else {
                servingQueue = 2;
            }

            EV << "Sta venendo servita la coda Q" << servingQueue << endl;
            cGate *gate = selectionStrategy->selectableGate(k);
            if (strategy == 0 || fromQueue1){
                check_and_cast<PassiveQueue *>(gate->getOwnerModule())->request(gate->getIndex());
            }
            else if (!isQ2Empty && !fromQueue1){
                check_and_cast<PassiveQueue *>(gate->getOwnerModule())->request(gate->getIndex());
            }

        } else {
            free = true;
        }

    } else {
        if (!allocated)
            error("job arrived, but the sender did not call allocate() previously");
        if (jobServiced)
            throw cRuntimeError("a new job arrived while already servicing one");

        jobServiced = check_and_cast<Job *>(msg);

        if (fromQueue1) {
            jobServiced->P = uniform(1, 10);
            EV << "P è: " << jobServiced->P << endl;
        }

        simtime_t serviceTime = par("serviceTime");
        scheduleAt(simTime() + serviceTime, endServiceMsg);
        emit(busySignal, true);

        // increase the number of customers served from the current queue
        if (fromQueue1){
            customersServedQ1++;
        }
        else {
            customersServedQ2++;
        }
    }
}

void Server::refreshDisplay() const
{
    getDisplayString().setTagArg("i2", 0, jobServiced ? "status/execute" : "");
}

void Server::finish()
{
}

bool Server::isIdle()
{
    return !allocated;  // we are idle if nobody has allocated us for processing
}

bool Server::isFree()
{
    return free;
}

void Server::allocate()
{
    allocated = true;
}

void Server::deallocate()
{
    allocated = false;
}

}; // namespace
