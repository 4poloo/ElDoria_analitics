# 📚 Eldoria Analytics

**Universidad Tecnológica Metropolitana — Computación Paralela y Distribuida**
**Profesor:** Sebastián Salazar Molina
**Entrega:** 02 de julio de 2025

---

## 🌍 Descripción

**Eldoria Analytics** es una suite de procesamiento **paralelo** y generación de informes que responde a las preguntas del Rey Aurelius IV para restaurar la **identidad ciudadana** y mejorar la **infraestructura de caminos mágicos** en el mundo de **Eldoria**.

Procesa un archivo de **100 millones de registros**, calcula métricas clave y genera gráficos que permiten tomar decisiones estratégicas para la construcción de caminos y políticas sociales.

---

## ⚙️ Funcionalidades

✅ Lectura y procesamiento paralelo de archivos enormes usando **C++ y OpenMP**
✅ División del procesamiento en **chunks paralelos** para no saturar la RAM
✅ Exportación de parciales y combinación optimizada
✅ Generación de **datos clave**:

* Conteo y porcentaje de estrato social
* Edad promedio y mediana por especie/género
* Distribución etaria
* Índice de dependencia
* Top 10 000 rutas origen-destino
* Top 10 pueblos más visitados
  ✅ **Gráficos automáticos** con Python + Matplotlib
  ✅ **Soporte para Docker**

---

## 📁 Estructura de carpetas

```
ElDoria_analitics/
│
├── data/                    # Entrada de datos (archivo CSV original)
├── outputs/                 # Resultados generados (CSV finales y gráficos)
├── scripts/                 # Scripts C++ y Python
│   ├── Codigo_Paralelizado.cpp
│   ├── conteo_estratos.cpp
│   ├── porcentaje_estratos.cpp
│   ├── edad_promedio.cpp
│   ├── edad_mediana.cpp
│   ├── distribucion_etaria.cpp
│   ├── indice_dependencia.cpp
│   ├── top_rutas_chunks.cpp
│   ├── top_rutas_global.cpp
│   ├── generate_all_graphs.py
│   └── ...
├── run.sh                   # Pipeline automatizado
├── entrypoint.sh            # Para Docker
├── Dockerfile               # Contenedor listo
└── README.md
```

---

## 🚀 Cómo usar sin Docker

### 1️⃣ Compila y ejecuta el pipeline

```bash
chmod +x run.sh
./run.sh
```

El script `run.sh`:

* Compila todos los programas C++
* Ejecuta el pipeline completo: genera datos, procesa chunks, fusiona y crea gráficos.

---

## 🐳 Cómo correr en Docker

### 1️⃣ Construye la imagen

```bash
docker build -t eldoria_analytics .
```

### 2️⃣ Ejecuta el contenedor

```bash
docker run --rm -v $PWD:/app eldoria_analytics
```

El contenedor ejecuta `entrypoint.sh` para compilar y correr **todo** automáticamente dentro del entorno controlado.

---

## 🔍 Requisitos

* Linux o WSL (recomendado Fedora)
* `g++` con soporte `OpenMP`
* Python 3 + `pandas` + `matplotlib`
* Docker (opcional)

---

## ✨ Créditos

* Desarrollado por **Max \[4poloo]**
* Asignatura: **Computación Paralela y Distribuida**
* Universidad Tecnológica Metropolitana

---

## 📌 Licencia

Este proyecto es de uso **académico**, diseñado para demostrar conceptos de **paralelismo práctico** y procesamiento de datos a gran escala.

---

**¡Gloria a Eldoria! 🌟**
