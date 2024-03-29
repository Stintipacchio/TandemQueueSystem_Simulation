//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_SERVER_H
#define __QUEUEING_SERVER_H

#include "IServer.h"

namespace queueing {

class Job;
class SelectionStrategy;

/**
 * The queue server. It cooperates with several Queues that which queue up
 * the jobs, and send them to Server on request.
 *
 * @see PassiveQueue
 */
class QUEUEING_API Server : public cSimpleModule, public IServer
{
    private:
        simsignal_t busySignal;
        bool allocated = false;

        SelectionStrategy *selectionStrategy = nullptr;

        Job *jobServiced = nullptr;
        cMessage *endServiceMsg = nullptr;

        int customersServedQ1;  // Variable to keep track of the number of customers served in Q1
        int customersServedQ2;  // Variable to keep track of the number of customers served in Q2
        bool fromQueue1;      // Variable to indicate whether the server is currently serving Queue 1 or Queue 2
        int N;  // Number of customers to be served from each queue
        bool isQ1Empty;
        bool isQ2Empty;
        bool free;
        int customersServedQ1buffer;
        int strategy;

    public:
        virtual ~Server();

    protected:
        virtual void initialize() override;
        virtual int numInitStages() const override {return 2;}
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
        virtual void finish() override;

    public:
        virtual bool isIdle() override;
        virtual void allocate() override;
        virtual void deallocate();
        virtual bool isFree();
};

}; //namespace

#endif


