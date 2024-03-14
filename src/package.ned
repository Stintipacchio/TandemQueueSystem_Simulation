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
    parameters:
//        double λ = [2.0, 1.4, 1.2, 1.0];
//        double m1 = [0.4, 0.3, 0.25, 0.20];
//        double m2 = [0.4, 0.3, 0.25, 0.20];
//        double p_min = [1, 4, 8]; // Minimo costo di servizio
//        double p_max = [3, 6, 10]; // Massimo costo di servizio
//        double V_min = [14, 18, 20]; // Minimo guadagno
//        double V_max = [16, 20, 22]; // Massimo guadagno
//        double CW = 1.0;
//        int N = [1, 2, 3, 4, 5]; // Numero di utenti
//        int numRuns = 10; // Numero di simulazioni da eseguire per ogni combinazione di parametri
        double λ = 2.0;
        double m1 = 0.4;
        double m2 = 0.4;
        double p_min = 1; // Minimo costo di servizio
        double p_max = 3; // Massimo costo di servizio
        double V_min = 14; // Minimo guadagno
        double V_max = 16; // Massimo guadagno
        double CW = 1.0;
        int N = 1; // Numero di utenti
        int numRuns = 10; // Numero di simulazioni da eseguire per ogni combinazione di parametri

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
        CustomerRouting: Allocate {
            @display("p=318,374");
        }

    connections:
        Source.out --> IdealChannel --> Q1.in++;
        Q1.out --> IdealChannel --> Server.in++;
        Server.out --> IdealChannel --> Q2.in++;
        Q2.out --> IdealChannel --> sink.in++;

        // Additional logic for customer behavior
        CustomerRouting.out --> Q2.in++ if size() > 1 && CustomerRouting.out >= 1;
        CustomerRouting.out --> Q1.in++ if size() > 1 && CustomerRouting.out >= 1 && uniform(0,1) >= λ;

        // Connect Q1.out to CustomerRouting.in based on condition
        Q1.out --> CustomerRouting.in[0] if uniform(0,1) < λ;

        // Logic for server behavior: loop for server to alternate between queues
        Server.out --> Q1.in++ if size() > 1;
        Server.out --> Q2.in++ if size() > 1;
}
