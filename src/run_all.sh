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

  # Rimuovi i vecchi file di output se esistono
  [ -f results.vec ] && rm results.vec
  [ -f results.sca ] && rm results.sca

  # Esegui la simulazione con la configurazione corrente
  opp_runall -j1 ./TandemQueueSystem_Simulation.exe -u Cmdenv -c $CONFIG -n . -f $INI_FILE

  # Rinomina e sposta i file di output nella directory dei risultati
  OUTPUT_VEC="${RESULTS_DIR}/${CONFIG}.vec"
  OUTPUT_SCA="${RESULTS_DIR}/${CONFIG}.sca"

  if [ -f results.vec ]; then
    mv results.vec "$OUTPUT_VEC"
  fi

  if [ -f results.sca ]; then
    mv results.sca "$OUTPUT_SCA"
  fi
done

echo "All simulations are completed."
