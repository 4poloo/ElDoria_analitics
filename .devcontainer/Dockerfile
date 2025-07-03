FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Dependencias
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    libomp-dev \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Crear directorio de trabajo
WORKDIR /app

# Copiar todos los archivos de la app
COPY . .

# Marcar scripts como ejecutables
RUN chmod +x run.sh entrypoint.sh

# Instalar Python deps
RUN pip3 install pandas matplotlib

# Compilar TODOS los binarios
RUN g++ -O3 -fopenmp -std=c++17 scripts/Codigo_Paralelizado.cpp -o scripts/parallel_program && \
    g++ -O3 -fopenmp -std=c++17 scripts/conteo_estratos.cpp -o scripts/conteo_estratos && \
    g++ -O3 -fopenmp -std=c++17 scripts/porcentaje_estratos.cpp -o scripts/porcentaje_estratos && \
    g++ -O3 -fopenmp -std=c++17 scripts/edad_promedio.cpp -o scripts/edad_promedio && \
    g++ -O3 -fopenmp -std=c++17 scripts/edad_mediana.cpp -o scripts/edad_mediana && \
    g++ -O3 -fopenmp -std=c++17 scripts/distribucion_etaria.cpp -o scripts/distribucion_etaria && \
    g++ -O3 -fopenmp -std=c++17 scripts/indice_dependencia.cpp -o scripts/indice_dependencia && \
    g++ -O3 -fopenmp -std=c++17 scripts/top_rutas_chunks.cpp -o scripts/top_rutas_chunks && \
    g++ -O3 -std=c++17 scripts/top_rutas_global.cpp -o scripts/top_rutas_global && \
    g++ -O3 -std=c++17 scripts/pueblos_mas_visitados.cpp -o scripts/pueblos_mas_visitados

# ENTRYPOINT que delega a tu pipeline
ENTRYPOINT ["./entrypoint.sh"]
