#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

class EldoriaGraphGenerator:
    def __init__(self, output_dir: str = "outputs"):
        self.OUTPUT_DIR = Path(output_dir)
        if not self.OUTPUT_DIR.exists():
            print(f"[!] La carpeta {self.OUTPUT_DIR} no existe. Creando...")
            self.OUTPUT_DIR.mkdir(parents=True)

    def graficar_conteo_estratos(self):
        """Gráfico de barras del conteo de estratos sociales"""
        print("\n[+] Generando gráfico de Conteo de Estratos Sociales...")
        csv_path = self.OUTPUT_DIR / 'A_conteo_estratos.csv'
        if not csv_path.exists():
            print("[!] Archivo A_conteo_estratos.csv no encontrado.")
            return

        df = pd.read_csv(csv_path)
        df = df.sort_values(by='estrato')

        fig, ax = plt.subplots(figsize=(10, 6))
        bars = ax.bar(df['estrato'].astype(str), df['cantidad'], color='#1f77b4')
        for bar in bars:
            ax.text(bar.get_x() + bar.get_width()/2,
                    bar.get_height() + max(df['cantidad'])*0.01,
                    f"{int(bar.get_height()):,}",
                    ha='center', fontsize=9)

        ax.set_title("Conteo de Población por Estrato Social")
        ax.set_xlabel("Estrato Social (0 = Nobleza Suprema, 9 = Desposeídos)")
        ax.set_ylabel("Cantidad de Personas")
        plt.tight_layout()
        fig.savefig(self.OUTPUT_DIR / 'grafico_conteo_estratos.png', dpi=100)
        plt.close(fig)
        print("[✔] Gráfico Conteo de Estratos generado.")

    def graficar_porcentaje_estratos(self):
        """Gráfico de porcentaje de población por estrato social"""
        print("\n[+] Generando gráfico de Porcentaje de Estratos Sociales...")
        csv_path = self.OUTPUT_DIR / 'B_porcentaje_estratos.csv'
        if not csv_path.exists():
            print("[!] Archivo B_porcentaje_estratos.csv no encontrado.")
            return

        df = pd.read_csv(csv_path)
        df = df.sort_values(by='estrato')

        fig, ax = plt.subplots(figsize=(10, 6))
        bars = ax.bar(df['estrato'].astype(str), df['porcentaje'], color='#ff7f0e')
        for bar in bars:
            ax.text(bar.get_x() + bar.get_width()/2,
                    bar.get_height() + max(df['porcentaje'])*0.01,
                    f"{bar.get_height():.2f}%",
                    ha='center', fontsize=9)

        ax.set_title("Porcentaje de Población por Estrato Social")
        ax.set_xlabel("Estrato Social (0 = Nobleza Suprema, 9 = Desposeídos)")
        ax.set_ylabel("Porcentaje (%)")
        plt.tight_layout()
        fig.savefig(self.OUTPUT_DIR / 'grafico_porcentaje_estratos.png', dpi=100)
        plt.close(fig)
        print("[✔] Gráfico Porcentaje de Estratos generado.")

    def graficar_indice_dependencia(self):
        """Gráfico de índice de dependencia"""
        print("\n[+] Generando gráfico de Índice de Dependencia...")
        csv_path = self.OUTPUT_DIR / 'F_indice_dependencia.csv'
        if not csv_path.exists():
            print("[!] Archivo F_indice_dependencia.csv no encontrado.")
            return

        df = pd.read_csv(csv_path)
        valores = [df['menores_15'].iloc[0], df['mayores_64'].iloc[0], df['trabajadores'].iloc[0]]
        labels = ['Menores de 15', 'Mayores de 64', 'Población Trabajadora']

        fig, ax = plt.subplots(figsize=(8, 6))
        bars = ax.bar(labels, valores, color=['skyblue', 'salmon', 'lightgreen'])
        for bar in bars:
            ax.text(bar.get_x() + bar.get_width()/2,
                    bar.get_height() + max(valores)*0.01,
                    f"{int(bar.get_height()):,}",
                    ha='center', fontsize=10)

        ax.set_title("Índice de Dependencia")
        ax.set_ylabel("Cantidad de Personas")
        plt.tight_layout()
        fig.savefig(self.OUTPUT_DIR / 'grafico_indice_dependencia.png', dpi=100)
        plt.close(fig)
        print("[✔] Gráfico Índice de Dependencia generado.")

    def graficar_edad_promedio(self):
        """Gráfico de edad promedio por especie y género"""
        print("\n[+] Generando gráfico de Edad Promedio...")
        csv_path = self.OUTPUT_DIR / 'C_edad_promedio.csv'
        if not csv_path.exists():
            print("[!] Archivo C_edad_promedio.csv no encontrado.")
            return

        df = pd.read_csv(csv_path)
        df.columns = df.columns.str.strip()
        pivot = df.pivot(index='especie', columns='genero', values='edad_promedio')

        fig, ax = plt.subplots(figsize=(10, 6))
        pivot.plot(kind='bar', ax=ax)
        for container in ax.containers:
            for bar in container:
                ax.text(bar.get_x() + bar.get_width()/2,
                        bar.get_height() + 0.1,
                        f"{bar.get_height():.2f}",
                        ha='center', fontsize=9)

        ax.set_title("Edad Promedio por Especie y Género")
        ax.set_ylabel("Edad Promedio")
        ax.set_xticklabels(pivot.index, rotation=45, ha='right')
        plt.tight_layout()
        fig.savefig(self.OUTPUT_DIR / 'grafico_edad_promedio.png', dpi=100)
        plt.close(fig)
        print("[✔] Gráfico Edad Promedio generado.")

    def graficar_distribucion_etaria_apilado(self):
        """Gráfico de barras apilado por especie y género"""
        print("\n[+] Generando gráfico de Distribución Etaria Apilada...")
        csv_path = self.OUTPUT_DIR / 'E_distribucion_etaria.csv'
        if not csv_path.exists():
            print("[!] Archivo E_distribucion_etaria.csv no encontrado.")
            return

        df = pd.read_csv(csv_path)
        pivot = df.groupby(['especie', 'genero'])['conteo'].sum().unstack(fill_value=0)
        totals = pivot.sum(axis=1)

        fig, ax = plt.subplots(figsize=(10, 6))
        pivot.plot(kind='bar', stacked=True, ax=ax)
        ax.set_title("Distribución Etaria por Especie y Género (Porcentaje)")
        ax.set_ylabel("Cantidad")
        ax.set_xticklabels(pivot.index, rotation=45, ha='right')

        for genero_idx, genero in enumerate(pivot.columns):
            container = ax.containers[genero_idx]
            for idx, bar in enumerate(container):
                height = bar.get_height()
                if height <= 0:
                    continue
                pct = height / totals.iloc[idx] * 100
                ax.text(bar.get_x() + bar.get_width()/2,
                        bar.get_y() + height/2,
                        f"{pct:.1f}%",
                        ha='center', va='center', fontsize=8, color='white')

        plt.tight_layout()
        fig.savefig(self.OUTPUT_DIR / 'grafico_distribucion_etaria_apilado.png', dpi=100)
        plt.close(fig)
        print("[✔] Gráfico Distribución Etaria Apilada generado con porcentaje.")

    def graficar_distribucion_etaria_agrupado(self):
        """Gráfico de barras agrupado por especie/género con colores"""
        print("\n[+] Generando gráfico de Distribución Etaria Agrupada con colores...")
        csv_path = self.OUTPUT_DIR / 'E_distribucion_etaria.csv'
        if not csv_path.exists():
            print("[!] Archivo E_distribucion_etaria.csv no encontrado.")
            return

        df = pd.read_csv(csv_path)
        pivot = df.groupby(['especie', 'genero'])['conteo'].sum().reset_index()
        pivot['etiqueta'] = pivot['especie'] + ' (' + pivot['genero'] + ')'

        especie_colores = {
            'elfica': '#2ca02c',
            'hombre bestia': '#d62728',
            'humana': '#1f77b4',
            'enana': '#ff7f0e'
        }
        colores = [especie_colores.get(e, '#7f7f7f') for e in pivot['especie']]

        fig, ax = plt.subplots(figsize=(12, 6))
        bars = ax.bar(pivot['etiqueta'], pivot['conteo'], color=colores)
        for bar in bars:
            ax.text(bar.get_x() + bar.get_width()/2,
                    bar.get_height() + max(pivot['conteo'])*0.01,
                    f"{int(bar.get_height()):,}",
                    ha='center', fontsize=8)

        ax.set_title("Distribución Etaria Agrupada por Especie y Género (Colores)")
        ax.set_ylabel("Cantidad")
        plt.xticks(rotation=45, ha='right')
        plt.tight_layout()
        fig.savefig(self.OUTPUT_DIR / 'grafico_distribucion_etaria_agrupado.png', dpi=100)
        plt.close(fig)
        print("[✔] Gráfico Distribución Etaria Agrupado generado con colores.")

    def graficar_top_20_rutas(self):
        """Top 20 rutas más usadas"""
        print("\n[+] Generando gráfico Top 20 Rutas...")
        csv_path = self.OUTPUT_DIR / 'top_10000_rutas_origen_destino.csv'
        if not csv_path.exists():
            print("[!] Archivo top_10000_rutas_origen_destino.csv no encontrado.")
            return

        df = pd.read_csv(csv_path)
        top20 = df.sort_values(by='cantidad_viajes', ascending=False).head(20)
        top20['ruta'] = top20['origen'].astype(str) + ' → ' + top20['destino'].astype(str)

        fig, ax = plt.subplots(figsize=(14, 8))
        bars = ax.barh(top20['ruta'][::-1], top20['cantidad_viajes'][::-1], color='#1f77b4')
        for bar in bars:
            ax.text(bar.get_width() + max(top20['cantidad_viajes'])*0.01,
                    bar.get_y() + bar.get_height()/2,
                    f"{int(bar.get_width()):,}",
                    va='center', fontsize=9)

        ax.set_title("Top 20 Rutas Más Usadas (Origen → Destino)")
        ax.set_xlabel("Cantidad de Viajes")
        plt.tight_layout()
        fig.savefig(self.OUTPUT_DIR / 'grafico_top20_rutas.png', dpi=100)
        plt.close(fig)
        print("[✔] Gráfico Top 20 Rutas generado.")

    def graficar_top_10_pueblos(self):
        """Top 10 pueblos más visitados"""
        print("\n[+] Generando gráfico Top 10 Pueblos Más Visitados...")
        csv_path = self.OUTPUT_DIR / 'top_pueblos_mas_visitados.csv'
        if not csv_path.exists():
            print("[!] Archivo top_pueblos_mas_visitados.csv no encontrado.")
            return

        df = pd.read_csv(csv_path)
        top10 = df.sort_values(by='frecuencia', ascending=False).head(10)

        fig, ax = plt.subplots(figsize=(10, 6))
        bars = ax.bar(top10['poblado'].astype(str), top10['frecuencia'], color='#9467bd')
        for bar in bars:
            ax.text(bar.get_x() + bar.get_width()/2,
                    bar.get_height() + max(top10['frecuencia'])*0.01,
                    f"{int(bar.get_height()):,}",
                    ha='center', fontsize=9)

        ax.set_title("Top 10 Pueblos Más Visitados")
        ax.set_xlabel("Código de Pueblo")
        ax.set_ylabel("Frecuencia de Visitas")
        plt.xticks(rotation=45, ha='right')
        plt.tight_layout()
        fig.savefig(self.OUTPUT_DIR / 'grafico_top10_pueblos.png', dpi=100)
        plt.close(fig)
        print("[✔] Gráfico Top 10 Pueblos generado.")

    def graficar_edad_mediana(self):
        """Genera gráfico de edad mediana por especie y género"""
        print("\n[+] Generando gráfico de Edad Mediana...")
        csv_path = self.OUTPUT_DIR / 'D_edad_mediana.csv'
        if not csv_path.exists():
            print("[!] Archivo D_edad_mediana.csv no encontrado.")
            return

        df = pd.read_csv(csv_path)
        df.columns = df.columns.str.strip()

        pivot = df.pivot(index='especie', columns='genero', values='edad_mediana_aprox')

        fig, ax = plt.subplots(figsize=(10, 6))
        pivot.plot(kind='bar', ax=ax)
        for container in ax.containers:
            for bar in container:
                ax.text(bar.get_x() + bar.get_width()/2,
                        bar.get_height() + 0.1,
                        f"{bar.get_height():.2f}",
                        ha='center', fontsize=9)

        ax.set_title("Edad Mediana por Especie y Género")
        ax.set_ylabel("Edad Mediana")
        ax.set_xticklabels(pivot.index, rotation=45, ha='right')
        plt.tight_layout()
        fig.savefig(self.OUTPUT_DIR / 'grafico_edad_mediana.png', dpi=100)
        plt.close(fig)
        print("[✔] Gráfico Edad Mediana generado.")

    def run_todos(self):
        self.graficar_conteo_estratos()
        self.graficar_porcentaje_estratos()
        self.graficar_indice_dependencia()
        self.graficar_edad_promedio()
        self.graficar_distribucion_etaria_apilado()
        self.graficar_distribucion_etaria_agrupado()
        self.graficar_top_20_rutas()
        self.graficar_top_10_pueblos()
        self.graficar_edad_mediana()

if __name__ == "__main__":
    EldoriaGraphGenerator().run_todos()
