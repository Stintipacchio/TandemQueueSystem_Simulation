#!/bin/bash

# Nome del file di configurazione .ini
INI_FILE="./omnetpp.ini"

# Estrai i nomi delle configurazioni dal file ini
CONFIGS=$(grep -oP '(?<=^\[Config ).*(?=\])' $INI_FILE)

# Path dove salvare i risultati
RESULTS_DIR="./results"

# Crea la directory per i risultati se non esiste
mkdir -p $RESULTS_DIR

# Ciclo attraverso ogni configurazione ed esegui la simulazione
for CONFIG in $CONFIGS; do
  echo "Running simulation for configuration: $CONFIG"

  # Numero di run per ogni configurazione
  RUN_COUNT=$(grep -oP '(?<=^repeat = ).*' $INI_FILE | tr -d '\r')

  # Ciclo attraverso ogni run
  for ((i=1; i<=RUN_COUNT; i++)); do
    echo "Running simulation $i for configuration: $CONFIG"

    # Rimuovi i vecchi file di output se esistono
    [ -f results.vec ] && rm results.vec
    [ -f results.sca ] && rm results.sca
    [ -f results.vci ] && rm results.vci

    # Esegui la simulazione con la configurazione corrente
    opp_runall -j1 ./TandemQueueSystem_Simulation.exe -u Cmdenv -c $CONFIG -r $((i-1)) -n . -f $INI_FILE

    # Rinomina e sposta i file di output nella directory dei risultati
    OUTPUT_VEC="${RESULTS_DIR}/${CONFIG}_run${i}.vec"
    OUTPUT_SCA="${RESULTS_DIR}/${CONFIG}_run${i}.sca"
    OUTPUT_VCI="${RESULTS_DIR}/${CONFIG}_run${i}.vci"

    if [ -f results.vec ]; then
      mv results.vec "$OUTPUT_VEC"
    fi

    if [ -f results.sca ]; then
      mv results.sca "$OUTPUT_SCA"
    fi

    if [ -f results.vci ]; then
      mv results.vci "$OUTPUT_VCI"
    fi
  done
done

echo "All simulations are completed."
