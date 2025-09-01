#!/bin/bash

echo "=================================================================="
echo "           ANÁLISIS COMPARATIVO DE RENDIMIENTO"
echo " Secuencial vs Pthreads vs OpenMP (3 filtros simultáneos) vs MPI"
echo "=================================================================="
echo ""

# Crear directorios para resultados
mkdir -p benchmark_results
mkdir -p benchmark_images
mkdir -p benchmark_metrics

# Función para medir tiempo de ejecución
measure_time() {
    local command="$1"
    local output_file="$2"
    
    # Ejecutar 3 veces y tomar el promedio
    total_time=0
    for i in {1..3}; do
        start_time=$(date +%s%3N)
        eval $command > /dev/null 2>&1
        end_time=$(date +%s%3N)
        execution_time=$((end_time - start_time))
        total_time=$((total_time + execution_time))
    done
    
    avg_time=$((total_time / 3))
    echo "$avg_time" > "$output_file"
    echo $avg_time
}

# Compilar todas las versiones
echo "🔧 Compilando todas las versiones..."

# Secuencial
echo "   Compilando versión secuencial..."
g++ -std=c++11 -Wall -Wextra -O2 -o filterer \
    filterer.cpp image.cpp imagesPGM.cpp imagesPPM.cpp \
    filter.cpp blurFilter.cpp laplaceFilter.cpp sharpenFilter.cpp timer.cpp

# Pthreads
echo "   Compilando versión pthreads..."
g++ -std=c++11 -Wall -Wextra -O2 -pthread -o pfilterer \
    pfilterer.cpp image.cpp imagesPGM.cpp imagesPPM.cpp \
    filter.cpp pfilter.cpp pfilterBlur.cpp pfilterLaplace.cpp pfilterSharpen.cpp timer.cpp

# OpenMP
echo "   Compilando versión OpenMP..."
g++ -std=c++11 -Wall -Wextra -O2 -fopenmp -o opfilterer \
    opfilterer.cpp image.cpp imagesPGM.cpp imagesPPM.cpp \
    filter.cpp opfilter.cpp blurFilter.cpp laplaceFilter.cpp sharpenFilter.cpp timer.cpp

# MPI
echo "   Compilando versión MPI..."
mpic++ -std=c++11 -Wall -Wextra -O2 -o mpifilterer_fixed \
    mpiFilterer.cpp image.cpp imagesPGM.cpp imagesPPM.cpp \
    filter.cpp blurFilter.cpp laplaceFilter.cpp sharpenFilter.cpp timer.cpp

echo "✅ Compilación completada"
echo ""

# Lista de imágenes para pruebas
images=("damma.pgm" "sulfur.pgm" "fruit.pgm" "lena.pgm" "puj.pgm")
filters=("blur" "laplace" "sharpen")

echo "📊 Iniciando benchmarks sistemáticos..."
echo "   Imágenes: ${images[*]}"
echo ""

# Crear archivo de resultados CSV
echo "Image,Sequential_total_ms,Pthreads_total_ms,OpenMP_3filters_ms,MPI2_total_ms,MPI4_total_ms,Pthread_Speedup,OpenMP_Speedup,MPI2_Speedup,MPI4_Speedup" > benchmark_results/performance_comparison.csv

for img in "${images[@]}"; do
    if [ -f "$img" ]; then
        echo "🖼️  Procesando imagen: $img"
        img_size=$(ls -lh "$img" | awk '{print $5}')
        img_dimensions=$(head -3 "$img" | tail -1)
        echo "   Tamaño: $img_size, Dimensiones: $img_dimensions"
        
        # Inicializar acumuladores de tiempo
        seq_total=0
        pthread_total=0
        mpi2_total=0
        mpi4_total=0
        openmp_time=0

        # Ejecutar los 3 filtros en Secuencial, Pthreads y MPI
        for filter in "${filters[@]}"; do
            echo "   🔄 Filtro: $filter"
            
            # Secuencial
            if [ -f "filterer" ]; then
                seq_time=$(measure_time "./filterer $img benchmark_images/${img%.p*}_seq_${filter}.${img##*.} --f $filter" "benchmark_metrics/seq_${img}_${filter}.time")
                seq_total=$((seq_total + seq_time))
                echo "      📈 Secuencial: ${seq_time}ms"
            fi
            
            # Pthreads
            if [ -f "pfilterer" ]; then
                pthread_time=$(measure_time "./pfilterer $img benchmark_images/${img%.p*}_pthread_${filter}.${img##*.} --f $filter" "benchmark_metrics/pthread_${img}_${filter}.time")
                pthread_total=$((pthread_total + pthread_time))
                echo "      🧵 Pthreads: ${pthread_time}ms"
            fi
            
            # MPI 2
            if [ -f "mpifilterer_fixed" ]; then
                mpi_2_time=$(measure_time "mpirun -n 2 ./mpifilterer_fixed $img benchmark_images/${img%.p*}_mpi2_${filter}.${img##*.} --f $filter" "benchmark_metrics/mpi2_${img}_${filter}.time")
                mpi2_total=$((mpi2_total + mpi_2_time))
                echo "      📡 MPI(2): ${mpi_2_time}ms"
            fi

            # MPI 4
            if [ -f "mpifilterer_fixed" ]; then
                mpi_4_time=$(measure_time "mpirun -n 4 ./mpifilterer_fixed $img benchmark_images/${img%.p*}_mpi4_${filter}.${img##*.} --f $filter" "benchmark_metrics/mpi4_${img}_${filter}.time")
                mpi4_total=$((mpi4_total + mpi_4_time))
                echo "      📡 MPI(4): ${mpi_4_time}ms"
            fi
        done

        # OpenMP (una sola ejecución aplica los 3 filtros)
        echo "   🔀 OpenMP (3 filtros simultáneos)..."
        if [ -f "opfilterer" ]; then
            openmp_time=$(measure_time "OMP_NUM_THREADS=4 ./opfilterer $img benchmark_images/${img%.p*}_openmp.${img##*.}" "benchmark_metrics/openmp_${img}.time")
            echo "      Tiempo total OpenMP (3 filtros): ${openmp_time}ms"
        fi

        # Calcular speedups comparando contra secuencial total
        if [ $seq_total -gt 0 ]; then
            pthread_speedup=$(awk "BEGIN {printf \"%.2f\", $seq_total / $pthread_total}" 2>/dev/null || echo "N/A")
            openmp_speedup=$(awk "BEGIN {printf \"%.2f\", $seq_total / $openmp_time}" 2>/dev/null || echo "N/A")
            mpi2_speedup=$(awk "BEGIN {printf \"%.2f\", $seq_total / $mpi2_total}" 2>/dev/null || echo "N/A")
            mpi4_speedup=$(awk "BEGIN {printf \"%.2f\", $seq_total / $mpi4_total}" 2>/dev/null || echo "N/A")
        else
            pthread_speedup="N/A"
            openmp_speedup="N/A"
            mpi2_speedup="N/A"
            mpi4_speedup="N/A"
        fi

        # Guardar en CSV
        echo "${img},${seq_total},${pthread_total},${openmp_time},${mpi2_total},${mpi4_total},${pthread_speedup},${openmp_speedup},${mpi2_speedup},${mpi4_speedup}" >> benchmark_results/performance_comparison.csv

        echo "   ⚡ Comparativa Global (3 filtros):"
        echo "      - Secuencial total: ${seq_total}ms"
        echo "      - Pthreads total: ${pthread_total}ms"
        echo "      - OpenMP (3 en paralelo): ${openmp_time}ms"
        echo "      - MPI(2) total: ${mpi2_total}ms"
        echo "      - MPI(4) total: ${mpi4_total}ms"
        echo "      Speedups -> Pthreads: ${pthread_speedup}x | OpenMP: ${openmp_speedup}x | MPI(2): ${mpi2_speedup}x | MPI(4): ${mpi4_speedup}x"
        echo ""
    else
        echo "⚠️  Imagen $img no encontrada, saltando..."
    fi
done

echo "✅ Benchmarks completados!"
echo "📁 Resultados en: benchmark_results/"
echo "🖼️  Imágenes procesadas en: benchmark_images/"
echo "📊 Métricas en: benchmark_metrics/"
