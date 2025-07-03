#!/bin/bash

# ============================================
# Eldoria Analytics 🚀 - Pipeline Completo
# ============================================

INPUT="data/eldoria.csv"
OUTPUT="outputs"
THREADS=8
ARCHIVO_COMPLETO="$OUTPUT/datos_completos.txt"
MIN_SIZE_BYTES=$((3100000000))  # ~3.1GB en bytes

echo "============================================"
echo "🚀 Iniciando Eldoria Analytics..."
echo "============================================"

# ⏱️ Inicio del cronómetro global
START_TIME=$(date +%s)

# --------------------------------------------
# 🔨 Compilación de todos los programas
# --------------------------------------------
echo "🔨 [#] Compilando programas C++..."

g++ -O3 -fopenmp -std=c++17 scripts/Codigo_Paralelizado.cpp -o scripts/parallel_program || { echo "❌ Error al compilar Codigo_Paralelizado"; exit 1; }
g++ -O3 -fopenmp -std=c++17 scripts/conteo_estratos.cpp -o scripts/conteo_estratos || { echo "❌ Error al compilar conteo_estratos"; exit 1; }
g++ -O3 -fopenmp -std=c++17 scripts/porcentaje_estratos.cpp -o scripts/porcentaje_estratos || { echo "❌ Error al compilar porcentaje_estratos"; exit 1; }
g++ -O3 -fopenmp -std=c++17 scripts/edad_promedio.cpp -o scripts/edad_promedio || { echo "❌ Error al compilar edad_promedio"; exit 1; }
g++ -O3 -fopenmp -std=c++17 scripts/edad_mediana.cpp -o scripts/edad_mediana || { echo "❌ Error al compilar edad_mediana"; exit 1; }
g++ -O3 -fopenmp -std=c++17 scripts/distribucion_etaria.cpp -o scripts/distribucion_etaria || { echo "❌ Error al compilar distribucion_etaria"; exit 1; }
g++ -O3 -fopenmp -std=c++17 scripts/indice_dependencia.cpp -o scripts/indice_dependencia || { echo "❌ Error al compilar indice_dependencia"; exit 1; }
g++ -O3 -fopenmp -std=c++17 scripts/top_rutas_global.cpp -o scripts/top_rutas_global || { echo "❌ Error al compilar top_rutas_global"; exit 1; }
g++ -O3 -fopenmp -std=c++17 scripts/top_pueblos.cpp -o scripts/top_pueblos || { echo "❌ Error al compilar top_pueblos"; exit 1; }

export OMP_NUM_THREADS=$THREADS

# --------------------------------------------
# ✅ Verificar datos_completos.txt
# --------------------------------------------
if [[ -f "$ARCHIVO_COMPLETO" ]]; then
  FILE_SIZE=$(stat -c%s "$ARCHIVO_COMPLETO")
  if (( FILE_SIZE > MIN_SIZE_BYTES )); then
    echo "⏭️ [!] $ARCHIVO_COMPLETO existe y pesa >3.1GB. Saltando generación..."
  else
    echo "⚙️ [#] Ejecutando Código Paralelizado..."
    scripts/parallel_program "$INPUT" "$ARCHIVO_COMPLETO" || { echo "❌ Error en Codigo_Paralelizado"; exit 1; }
  fi
else
  echo "⚙️ [#] Generando datos_completos.txt..."
  scripts/parallel_program "$INPUT" "$ARCHIVO_COMPLETO" || { echo "❌ Error en Codigo_Paralelizado"; exit 1; }
fi

# --------------------------------------------
# ⚙️ Análisis base
# --------------------------------------------
echo "--------------------------------------------"
echo "⚙️ [#] Conteo y porcentaje estrato..."
scripts/conteo_estratos "$ARCHIVO_COMPLETO" "$OUTPUT/A_conteo_estratos.csv" || { echo "❌ Error en conteo_estratos"; exit 1; }
scripts/porcentaje_estratos "$ARCHIVO_COMPLETO" "$OUTPUT/B_porcentaje_estratos.csv" || { echo "❌ Error en porcentaje_estratos"; exit 1; }

echo "⚙️ [#] Edad promedio..."
scripts/edad_promedio "$ARCHIVO_COMPLETO" "$OUTPUT/C_edad_promedio.csv" || { echo "❌ Error en edad_promedio"; exit 1; }

echo "⚙️ [#] Edad mediana..."
scripts/edad_mediana "$ARCHIVO_COMPLETO" "$OUTPUT/D_edad_mediana.csv" || { echo "❌ Error en edad_mediana"; exit 1; }

echo "⚙️ [#] Distribución etaria..."
scripts/distribucion_etaria "$ARCHIVO_COMPLETO" "$OUTPUT/E_distribucion_etaria.csv" || { echo "❌ Error en distribucion_etaria"; exit 1; }

echo "⚙️ [#] Índice de dependencia..."
scripts/indice_dependencia "$ARCHIVO_COMPLETO" "$OUTPUT/F_indice_dependencia.csv" || { echo "❌ Error en indice_dependencia"; exit 1; }

# --------------------------------------------
# ⚙️ Generar top rutas (GLOBAL)
# --------------------------------------------
echo "--------------------------------------------"
echo "🚦 [#] Ejecutando top_rutas_global..."
scripts/top_rutas_global "$ARCHIVO_COMPLETO" "$OUTPUT/top_10000_rutas_origen_destino.csv" || { echo "❌ Error en top_rutas_global"; exit 1; }
echo "⚙️ [#] Ejecutando top_pueblos..."
scripts/top_pueblos outputs/datos_completos.txt outputs/top_pueblos_mas_visitados.csv || { echo "❌ Error en top_pueblos"; exit 1; }
# --------------------------------------------
# 📊 Generar gráficos finales
# --------------------------------------------

# ⏱️ Fin del cronómetro
END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))

echo "--------------------------------------------"
echo "📊 [#] Generando gráficos Python..."
python3 scripts/generate_all_graphs.py "$OUTPUT" || { echo "❌ Error generando gráficos"; exit 1; }

echo "========================================================="
echo "✅ 🎉 ¡Proceso completado! Resultados en: $OUTPUT/"
echo "⏱️ Tiempo total de ejecución: $((DURATION / 60)) min $((DURATION % 60)) s"
echo "========================================================="