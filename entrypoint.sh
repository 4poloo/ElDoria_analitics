#!/bin/bash

# ========================================
# Eldoria Analytics 🚀 ENTRYPOINT SCRIPT
# ========================================

# ✅ Forzar salida ante errores
set -e

# Limitar memoria (opcional, solo afecta este shell)
ulimit -v 8388608  # ~8GB en KB

# -----------------------------
# Crear carpetas si faltan
# -----------------------------
mkdir -p outputs
mkdir -p data

# -----------------------------
# Lanzar pipeline principal
# -----------------------------
echo "============================================"
echo "🚀 ENTRYPOINT: Iniciando pipeline Eldoria..."
echo "============================================"

# Dar permisos por si acaso
chmod +x run.sh
chmod +x scripts/*

# Ejecutar el pipeline
./run.sh

echo "✅ ENTRYPOINT: Pipeline finalizado."
