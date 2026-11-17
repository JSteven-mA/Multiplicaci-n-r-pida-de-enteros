Práctica 2 — Multiplicación rápida de enteros: Karatsuba
Análisis y Diseño de Algoritmos · Divide y Vencerás
Implementación en C++ de la multiplicación clásica (Θ(n²)) y del algoritmo de Karatsuba (Θ(n^log₂3) ≈ Θ(n^1,585)) sobre números de muchos dígitos, con validación de correctitud y medición de tiempos.
Autor: Jaisson Steven Merchán Ardila
Contenido del repositorio
```
.
main.cpp            # código fuente 
tiempos.csv         # tiempos medidos                            
informe.pdf         # informe de la práctica
README.md
```
Requisitos
Compilador `g++` con soporte para C++17 (por ejemplo, MinGW en Windows o GCC en Linux/macOS).
No se usan bibliotecas externas.
Compilar
```
g++ -O2 -std=c++17 src/main.cpp -o ada2
```
`-O2` activa las optimizaciones del compilador; los tiempos del informe se midieron con esta opción.
Ejecutar
Linux / macOS / Git Bash:
```
./ada2 > tiempos.csv
```
PowerShell (Windows):
```
./ada2 | Out-File -Encoding ascii tiempos.csv
```
El programa tarda unos segundos, porque cada medición se repite hasta superar 50 ms.
Qué hace el programa
Validación (por pantalla): compara Karatsuba contra la multiplicación clásica en casos de borde (1 dígito, ceros a la izquierda, ceros intercalados, longitudes impares), productos grandes (300 a 1000 dígitos) y 2000 pares aleatorios. Si todo coincide, imprime `Validacion OK`; si algo falla, se detiene con un error de `assert`.
Benchmark (al archivo CSV): mide el tiempo medio por multiplicación, en microsegundos, para n = 2, 4, 8, …, 4096 dígitos, con `std::chrono::steady_clock`. El CSV tiene las columnas `n,clasico_us,karatsuba_us`.
Decisiones de diseño
Los números son `vector<int>` en base 10 y little-endian (`a[0]` son las unidades).
Los operandos se rellenan con ceros hasta una potencia de 2 antes de aplicar Karatsuba.
Las funciones auxiliares (`sumar`, `restar`, `desplazar`, `partir`) admiten longitudes variables y normalizan los ceros a la izquierda.
El término central se calcula como `(P3 - P1) - P2`, para no tener restas negativas.
Caso base: números de 1 dígito (`UMBRAL = 1` en el código).
Los operandos aleatorios usan una semilla fija (`mt19937` con semilla 42), así que los resultados son reproducibles.
Nota sobre el CSV en Excel
Excel en español usa el punto y coma como separador, por lo que el CSV puede abrirse en una sola columna. Para separarlo: Datos → Texto en columnas → Delimitados → Coma, y en Avanzadas indicar el punto como separador decimal.
Uso de inteligencia artificial
Se usó Claude (Anthropic) y ChatGPT como apoyo al aprendizaje. El detalle está en la última sección del informe.
