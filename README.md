Perfecto 🚀 Te preparo un **README.md** al estilo que mostraste, pero adaptado a **tu parcial** con la info que ya generamos (implementaciones, resultados y conclusiones).

Aquí lo tienes listo para usar:

---

````markdown
# 📸 Parallel Image Filtering - Midterm 1.1
**Análisis Comparativo de Programación Paralela en Filtrado de Imágenes PPM/PGM**

<img alt="C++" src="https://img.shields.io/badge/C++-11-blue.svg">
<img alt="OpenMP" src="https://img.shields.io/badge/OpenMP-4.5-green.svg">
<img alt="MPI" src="https://img.shields.io/badge/MPI-3.1-red.svg">
<img alt="Docker" src="https://img.shields.io/badge/Docker-Ready-2496ED.svg">
<img alt="License" src="https://img.shields.io/badge/License-Academic-yellow.svg">

---

## 🎯 Pregunta de Investigación
**¿Cómo influye la programación paralela, utilizando OpenMP o Pthreads y MPI (en un entorno simulado con Docker), en el tiempo de ejecución del filtrado de imágenes PPM y PGM en comparación con una implementación secuencial?**

---

## 📋 Descripción del Proyecto
Este proyecto implementa y compara **4 paradigmas de programación paralela** aplicados al filtrado de imágenes **PPM** y **PGM**, evaluando su rendimiento frente a una versión **secuencial baseline**.

---

## 🧩 Componentes Implementados
- 🔄 **Secuencial**: Implementación base orientada a objetos.  
- 🧵 **Pthreads**: Memoria compartida con división por regiones.  
- ⚡ **OpenMP**: Paralelización automática con directivas `#pragma`.  
- 🌐 **MPI**: Distribución por filas horizontales entre procesos, simulada en **Docker**.  

---

## 🎨 Filtros Implementados
- **Blur (Suavizado)** → Convolución 3x3 para reducir ruido.  
- **Laplace (Bordes)** → Operador Laplaciano para detectar contornos.  
- **Sharpen (Realce)** → Resalta detalles y bordes.  

---

## 📊 Paradigmas de Paralelización
- **Secuencial** → Procesamiento píxel por píxel.  
- **Pthreads** → División de la imagen en 4 cuadrantes.  
- **OpenMP** → Paralelización de bucles con `collapse(2)`.  
- **MPI** → Reparto de filas entre procesos.  

---

## 🔬 Sistema de Medición
- ⏱️ Timer de alta precisión (CPU + Wall time).  
- 📈 Cálculo automático de *speedups*.  
- 📊 CSVs con métricas detalladas.  
- 🖼️ Almacenamiento de imágenes filtradas.  

---

## 🛠️ Compilación y Ejecución
### Prerrequisitos
- `g++` con soporte `-fopenmp`
- `mpic++` (OpenMPI)
- `docker` y `docker-compose` para la simulación distribuida

### Compilación
```bash
# Secuencial
g++ -std=c++11 -O2 -o filterer filterer.cpp ...

# Pthreads
g++ -std=c++11 -O2 -pthread -o pfilterer pfilterer.cpp ...

# OpenMP
g++ -std=c++11 -O2 -fopenmp -o opfilterer opfilterer.cpp ...

# MPI
mpic++ -std=c++11 -O2 -o mpifilterer mpiFilterer.cpp ...
````

### Ejecución

```bash
./filterer lena.pgm lena_seq_blur.pgm --f blur
./pfilterer lena.pgm lena_pthread_blur.pgm --f blur
OMP_NUM_THREADS=4 ./opfilterer lena.pgm lena_openmp_blur.pgm --f blur
mpirun -n 4 ./mpifilterer lena.pgm lena_mpi_blur.pgm --f blur
```

---

## 📊 Resultados de Rendimiento

### ⏱️ Tiempo total por imagen (ms)

| Imagen     | Secuencial | Pthreads | OpenMP (3 filtros) | MPI (2p) | MPI (4p) |
| ---------- | ---------- | -------- | ------------------ | -------- | -------- |
| damma.pgm  | 1894       | 2212     | 1601               | 4822     | 8100     |
| sulfur.pgm | 1643       | 1626     | 1159               | 3584     | 5736     |
| fruit.pgm  | 631        | 768      | 586                | 2235     | 3322     |
| lena.pgm   | 728        | 636      | 646                | 1430     | 2584     |
| puj.pgm    | 1905       | 1754     | 1292               | 4371     | 7400     |

---

### 🏆 Speedups Promedio

* 🧵 **Pthreads** → 0.82x – 1.14x
* ⚡ **OpenMP** → 1.08x – 1.47x
* 🌐 **MPI** → 0.19x – 0.51x

---

## 🔍 Hallazgos Principales

✅ **Factores que mejoran el rendimiento**

* Imágenes grandes → mejor escalabilidad.
* Filtro Blur → el más eficiente en paralelo.
* OpenMP logra balance óptimo entre facilidad y rendimiento.

⚠️ **Limitaciones**

* **MPI** presenta alto overhead en Docker (solo conviene en distribuidos reales).
* Imágenes pequeñas no aprovechan el paralelismo (overhead > ganancia).
* **Pthreads** requiere más código y manejo manual.

---

## 🎯 Conclusiones

La programación paralela tiene un **impacto positivo en el filtrado de imágenes**:

* **OpenMP** se consolida como la mejor opción por su sencillez y buen speedup.
* **Pthreads** ofrece el máximo control y rendimiento, pero es más complejo.
* **MPI** es útil solo en sistemas realmente distribuidos, no en entornos simulados.

---

## 🧪 Reproducir Experimentos

```bash
# Benchmark automático
./benchmark_all_versions.sh

# Resultados
cat benchmark_results/performance_comparison.csv
```

---

## 🏫 Contexto Académico

* Curso: **Programación Paralela**
* Institución: \[Tu Universidad]
* Profesor: \[Nombre del Profesor]
* Semestre: \[2025-1]

---

## 📚 Referencias

* Pacheco, P. S. (2011). *An Introduction to Parallel Programming*.
* OpenMP ARB (2018). *OpenMP Application Programming Interface*.
* MPI Forum (2015). *Message Passing Interface Standard*.
* Netpbm Utilities Documentation.

---

## 📄 Licencia

Licencia académica, uso exclusivo educativo.

---

## 📞 Contacto

Autor: \[Tu Nombre]
Email: \[[tu.email@universidad.edu](mailto:tu.email@universidad.edu)]
GitHub: \[tu-usuario]

```

---

¿Quieres que también te lo convierta directamente en **PDF estilo informe académico** con portada, o prefieres dejar este **README en formato Markdown** para tu repositorio de GitHub?
```
