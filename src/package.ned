package simu;

import ned.IUnidirectionalChannel;
import ned.IdealChannel;
import simu.Qlib_Custom.Allocate;
import simu.Qlib_Custom.PassiveQueue;
import simu.Qlib_Custom.Queue;
import simu.Qlib_Custom.Router;
import simu.Qlib_Custom.Server;
import simu.Qlib_Custom.Sink;
import simu.Qlib_Custom.Source;


network TandemQueueSystem
{
    submodules:
        Source: Source {
            @display("p=66,260;is=l");
        }
        Server: Server {
            @display("p=318,140");
        }
        sink: Sink {
            @display("p=450,140;is=vr");
        }

        Q1: PassiveQueue {
            @display("p=227,260");
        }
        Q2: PassiveQueue {
            @display("p=418,260");
        }
    connections:
        Source.out --> IdealChannel --> Q1.in++;
        Q1.out++ --> IdealChannel --> Server.in++;
        Q1.out++ --> IdealChannel --> Server.in++;
        Server.out --> IdealChannel --> Q2.in++;
        Q2.out++ --> IdealChannel --> Server.in++;
        Server.out2 --> IdealChannel --> sink.in++;
}
