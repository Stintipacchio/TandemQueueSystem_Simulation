#!/bin/bash

# Directory di input e output
input_dir="./results"
output_dir="./results_CSV"

# Crea la directory di output se non esiste
mkdir -p "$output_dir"

# Funzione per convertire file
convert_file() {
    local file_path="$1"
    local output_dir="$2"
    local file_name=$(basename "$file_path")
    local file_extension="${file_name##*.}"
    local file_base_name="${file_name%.*}"

    case "$file_extension" in
        sca)
            output_file="${output_dir}/${file_base_name}_sca.csv"
            ;;
        vec)
            output_file="${output_dir}/${file_base_name}_vec.csv"
            ;;
        vci)
            output_file="${output_dir}/${file_base_name}_vci.csv"
            ;;
        *)
            echo "Formato file non supportato: $file_path"
            return
            ;;
    esac

    ../../../bin/opp_scavetool.exe export -o "$output_file" "$file_path"
    echo "Convertito: $file_path -> $output_file"
}

# Itera attraverso tutti i file nella directory di input
for file_path in "$input_dir"/*; do
    if [[ -f "$file_path" ]]; then
        case "$file_path" in
            *.sca | *.vec | *.vci)
                convert_file "$file_path" "$output_dir"
                ;;
            *)
                echo "Saltato: $file_path"
                ;;
        esac
    fi
done