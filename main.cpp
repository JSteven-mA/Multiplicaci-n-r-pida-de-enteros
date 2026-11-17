// Práctica 2 - Análisis y Diseño de Algoritmos
// Multiplicación rápida de enteros: Karatsuba vs. clásica

#include <bits/stdc++.h>
using namespace std;
using Num = vector<int>;

Num normalizar(Num a) {                 // elimina ceros a la izquierda (extremo alto)
    while (a.size() > 1 && a.back() == 0) a.pop_back();
    if (a.empty()) a.push_back(0);
    return a;
}
bool esCero(const Num& a) {
    for (int d : a) if (d) return false;
    return true;
}

// ---------- operaciones auxiliares ----------
// A + B; admite longitudes distintas, el resultado puede tener un dígito más
Num sumar(const Num& a, const Num& b) {
    size_t n = max(a.size(), b.size());
    Num r; r.reserve(n + 1);
    int c = 0;
    for (size_t i = 0; i < n; i++) {
        int s = c + (i < a.size() ? a[i] : 0) + (i < b.size() ? b[i] : 0);
        r.push_back(s % 10);
        c = s / 10;
    }
    if (c) r.push_back(c);
    return normalizar(r);
}

// A - B con precondición A >= B; normaliza el resultado
Num restar(const Num& a, const Num& b) {
    Num r; r.reserve(a.size());
    int borrow = 0;
    for (size_t i = 0; i < a.size(); i++) {
        int d = a[i] - borrow - (i < b.size() ? b[i] : 0);
        if (d < 0) { d += 10; borrow = 1; } else borrow = 0;
        r.push_back(d);
    }
    return normalizar(r);
}

// A * 10^k (añade k ceros a la derecha = k ceros al inicio en little-endian)
Num desplazar(const Num& a, size_t k) {
    if (esCero(a)) return {0};
    Num r(k, 0);
    r.insert(r.end(), a.begin(), a.end());
    return r;
}

// Separa A en mitad alta y baja respecto de la posición m
void partir(const Num& a, size_t m, Num& alta, Num& baja) {
    size_t c = min(m, a.size());
    baja = normalizar(Num(a.begin(), a.begin() + c));
    alta = (a.size() > m) ? normalizar(Num(a.begin() + m, a.end())) : Num{0};
}

// ---------- algoritmos ----------
// Multiplicación clásica Θ(n²)
Num multiplicacionClasica(const Num& a, const Num& b) {
    Num r(a.size() + b.size(), 0);
    for (size_t i = 0; i < a.size(); i++) {
        int c = 0;
        for (size_t j = 0; j < b.size(); j++) {
            int cur = r[i + j] + a[i] * b[j] + c;
            r[i + j] = cur % 10;
            c = cur / 10;
        }
        r[i + b.size()] += c;
    }
    return normalizar(r);
}

// Caso base: n <= UMBRAL se resuelve con multiplicación directa.
// UMBRAL = 1 -> Karatsuba
static size_t UMBRAL = 1;

Num karatsuba(const Num& x, const Num& y) {
    size_t n = max(x.size(), y.size());
    if (n <= UMBRAL) return multiplicacionClasica(x, y);
    if (esCero(x) || esCero(y)) return {0};

    size_t m = n / 2;
    Num x1, x0, y1, y0;
    partir(x, m, x1, x0);
    partir(y, m, y1, y0);

    Num P1 = karatsuba(x1, y1);
    Num P2 = karatsuba(x0, y0);
    Num P3 = karatsuba(sumar(x1, x0), sumar(y1, y0));

    Num mid = restar(restar(P3, P1), P2);           // orden: (P3 - P1) - P2, siempre >= 0
    return sumar(sumar(desplazar(P1, 2 * m), desplazar(mid, m)), P2);
}

// Relleno con ceros hasta una potencia de 2 
Num karatsubaPad(Num x, Num y) {
    size_t n = 1;
    while (n < max(x.size(), y.size())) n <<= 1;
    x.resize(n, 0);
    y.resize(n, 0);
    return karatsuba(x, y);
}

// ---------- conversión / generación ----------
Num desdeString(const string& s) {
    Num r;
    for (int i = (int)s.size() - 1; i >= 0; i--) r.push_back(s[i] - '0');
    return normalizar(r);
}
string aStr(const Num& a) {
    string s;
    for (int i = (int)a.size() - 1; i >= 0; i--) s += char('0' + a[i]);
    return s;
}
Num aleatorio(size_t n, mt19937& g) {
    Num r(n);
    for (auto& d : r) d = g() % 10;
    if (n > 1 && r.back() == 0) r.back() = 1;
    return r;
}

int main() {
    // ---------- VALIDACIÓN ----------
    mt19937 g(42);
    vector<pair<string, string>> casos = {
        {"7", "8"}, {"9", "9"}, {"0", "12345"},                      // 1 dígito / cero
        {"000000000000123", "0000000000456"},                         // ceros a la izquierda
        {"1000000007", "10203040506"}, {"90009", "100001"},           // ceros intercalados
        {"123", "45678"}, {"12345", "67"}, {"99999", "99999"},        // longitudes impares/distintas
    };
    for (auto& [a, b] : casos) {
        Num A = desdeString(a), B = desdeString(b);
        assert(karatsubaPad(A, B) == multiplicacionClasica(A, B));
    }
    for (size_t n : {300, 500, 777, 1000}) {                          // productos grandes
        Num A = aleatorio(n, g), B = aleatorio(n, g);
        assert(karatsubaPad(A, B) == multiplicacionClasica(A, B));
    }
    for (int t = 0; t < 2000; t++) {                                  // aleatorio masivo
        Num A = aleatorio(1 + g() % 60, g), B = aleatorio(1 + g() % 60, g);
        assert(karatsubaPad(A, B) == multiplicacionClasica(A, B));
    }
    fprintf(stderr, "Validacion OK\n");

    // ---------- BENCHMARK ----------
    puts("n,clasico_us,karatsuba_us");
    volatile int sink = 0;
    for (size_t n = 2; n <= 4096; n <<= 1) {
        Num A = aleatorio(n, g), B = aleatorio(n, g);
        auto medir = [&](auto f) {
            size_t reps = 1;
            double us;
            while (true) {                                            // auto-ajusta repeticiones
                auto t0 = chrono::steady_clock::now();
                for (size_t r = 0; r < reps; r++) sink = sink + f(A, B)[0];   // consume el resultado
                auto t1 = chrono::steady_clock::now();
                us = chrono::duration<double, micro>(t1 - t0).count();
                if (us > 50000) break;                                // >= 50 ms
                reps *= 2;
            }
            return us / reps;
        };
        double tc = medir(multiplicacionClasica);
        double tk = medir(karatsubaPad);
        printf("%zu,%.3f,%.3f\n", n, tc, tk);
    }
    return 0;
}
