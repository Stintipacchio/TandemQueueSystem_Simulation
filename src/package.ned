package simu;

import ned.IUnidirectionalChannel;
import ned.IdealChannel;
import simu.Qlib_Custom.Allocate;
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
        Q1: Queue {
            @display("p=204,260");
        }
        Q2: Queue {
            @display("p=431,260");
        }
        Server: Server {
            @display("p=318,140");
        }
        sink: Sink {
            @display("p=577,260;is=vr");
        }

    connections:
        Source.out --> IdealChannel --> Q1.in++;
        Q1.out2 --> IdealChannel --> Q2.in++;
        Q1.out1 --> IdealChannel --> Server.in++;
        Q2.out1 --> IdealChannel --> Server.in++;
        Q2.out2 --> IdealChannel --> sink.in++;
        Server.out2 --> IdealChannel --> Q2.in++;
        Server.out1 --> IdealChannel --> Q1.in++;
}
