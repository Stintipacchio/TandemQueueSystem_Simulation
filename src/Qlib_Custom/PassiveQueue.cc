//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "PassiveQueue.h"
#include "Job.h"
#include "IServer.h"
#include "Server.h"
#include <string>
#include <iostream>

namespace queueing {

Define_Module(PassiveQueue);

PassiveQueue::~PassiveQueue()
{
    delete selectionStrategy;
}

void PassiveQueue::initialize()
{
    droppedSignal = registerSignal("dropped");
    queueingTimeSignal = registerSignal("queueingTime");
    queueLengthSignal = registerSignal("queueLength");
    emit(queueLengthSignal, 0);

    capacity = par("capacity");
    queue.setName("queue");

    fifo = par("fifo");

    selectionStrategy = SelectionStrategy::create(par("sendingAlgorithm"), this, false);
    if (!selectionStrategy)
        throw cRuntimeError("invalid selection strategy");
}

void PassiveQueue::handleMessage(cMessage *msg)
{
    // Check if the queue was empty before entering the job
    int oldLength = length(); // Stores the current length of the queue

    std::string str1 = getFullName();
    std::string str2 = "Q1";
    int result = str1.compare(str2);

    if (result == 0) {
        // Find the server connected to Q1 and set its allocate field to false
        cModule *serverModule = getParentModule()->getSubmodule("Server");
        if (serverModule) {
            Server *server = check_and_cast<Server *>(serverModule);
            if(server->isIdle() || server->isFree()){
                //EV << "LIBERO" << endl;
                server->deallocate();
            }
            else{
                //EV << "OCCUPATO" << endl;
                server->allocate();
            }
        } else {
            EV << "Server module not found!" << endl;
        }
    }

    Job *job = check_and_cast<Job *>(msg);
    job->setTimestamp();

    // check for container capacity
    if (capacity >= 0 && queue.getLength() >= capacity) {
        EV << "Queue full! Job dropped.\n";
        if (hasGUI())
            bubble("Dropped!");
        emit(droppedSignal, 1);
        delete msg;
        return;
    }

    int k = selectionStrategy->select();
    if (k < 0) {
        // enqueue if no idle server found
        queue.insert(job);
        emit(queueLengthSignal, length());
        job->setQueueCount(job->getQueueCount() + 1);
    }
    else if (length() == 0) {
        // send through without queueing
        sendJob(job, k);
    }
    else
        throw cRuntimeError("This should not happen. Queue is NOT empty and there is an IDLE server attached to us.");
}

void PassiveQueue::refreshDisplay() const
{
    // change the icon color
    getDisplayString().setTagArg("i", 1, queue.isEmpty() ? "" : "cyan");
}

int PassiveQueue::length()
{
    return queue.getLength();
}

void PassiveQueue::request(int gateIndex)
{
    Enter_Method("request()!");

    ASSERT(!queue.isEmpty());

    Job *job;
    if (fifo) {
        job = (Job *)queue.pop();
    }
    else {
        job = (Job *)queue.back();
        // FIXME this may have bad performance as remove uses linear search
        queue.remove(job);
    }
    emit(queueLengthSignal, length());

    job->setQueueCount(job->getQueueCount()+1);
    simtime_t d = simTime() - job->getTimestamp();
    job->setTotalQueueingTime(job->getTotalQueueingTime() + d);
    emit(queueingTimeSignal, d);

    sendJob(job, gateIndex);
}

void PassiveQueue::sendJob(Job *job, int gateIndex)
{
    cGate *out = gate("out", gateIndex);
    send(job, out);
    check_and_cast<IServer *>(out->getPathEndGate()->getOwnerModule())->allocate();
}

bool PassiveQueue::isEmptyQueue() const {
    return queue.isEmpty();
}

}; //namespace

