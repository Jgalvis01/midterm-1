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
