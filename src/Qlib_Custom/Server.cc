//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

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
    if (*fromQueue1) {
        //std::cout << "Il numero di clienti serviti in Q1 è: " << customersServedQ1 << std::endl;
        EV << "Switching to Queue 1" << endl;
    } else {
        //std::cout << "Il numero di clienti serviti in Q2 è: " << customersServedQ2 << std::endl;
        EV << "Switching to Queue 2" << endl;
    }

    return k;

}

void Server::handleMessage(cMessage *msg)
{

    bool isQ1Empty;
    bool isQ2Empty;

    if (msg == endServiceMsg) {

        PassiveQueue *Q1 = dynamic_cast<PassiveQueue *>(getModuleByPath("^.Q1"));

        if (!Q1) {
            EV << "Error: Unable to find Q1!" << endl;
            // Error handling if Q1 is not found
            }
        else {
            // Now you can call the isEmptyQueue() method on q1 and use the returned value as you wish
            isQ1Empty = Q1->isEmptyQueue();
            // You can use the isQ1Empty variable as you wish, for example, printing a message
              if (isQ1Empty) {
                  EV << "Q1 is empty!" << endl;
              }
              else {
                  EV << "Q1 is not empty." << endl;
              }
        }

        PassiveQueue *Q2 = dynamic_cast<PassiveQueue *>(getModuleByPath("^.Q2"));

        if (!Q2) {
            EV << "Error: Unable to find Q1!" << endl;
            // Error handling if Q1 is not found
            }
        else {
            // Now you can call the isEmptyQueue() method on q1 and use the returned value as you wish
            isQ2Empty = Q2->isEmptyQueue();
            // You can use the isQ1Empty variable as you wish, for example, printing a message
              if (isQ2Empty) {
                  EV << "Q2 is empty!" << endl;
              }
              else {
                  EV << "Q2 is not empty." << endl;
              }
        }



        ASSERT(jobServiced != nullptr);
        ASSERT(allocated);
        simtime_t d = simTime() - endServiceMsg->getSendingTime();
        jobServiced->setTotalServiceTime(jobServiced->getTotalServiceTime() + d);
        if (fromQueue1)
            send(jobServiced, "out");
        else
            send(jobServiced, "out2");
        jobServiced = nullptr;
        allocated = false;
        emit(busySignal, false);




        int k = Exact_N(N, &customersServedQ1, &customersServedQ2, &fromQueue1);

        int servingQueue = 1;

        if (k == 0){
            servingQueue = 1;
        }
        else{
            servingQueue = 2;
        }

        std::cout << "Sta venendo servita la coda Q" << servingQueue << std::endl;
        cGate *gate = selectionStrategy->selectableGate(k);
        check_and_cast<PassiveQueue *>(gate->getOwnerModule())->request(gate->getIndex());

    }
    else {
        if (!allocated)
            error("job arrived, but the sender did not call allocate() previously");
        if (jobServiced)
            throw cRuntimeError("a new job arrived while already servicing one");

        jobServiced = check_and_cast<Job *>(msg);
        simtime_t serviceTime = par("serviceTime");
        scheduleAt(simTime()+serviceTime, endServiceMsg);
        emit(busySignal, true);

        // increase the number of customers served from the current queue
        if (fromQueue1){
            customersServedQ1++;
        }
        else{
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

void Server::allocate()
{
    allocated = true;
}

}; //namespace


