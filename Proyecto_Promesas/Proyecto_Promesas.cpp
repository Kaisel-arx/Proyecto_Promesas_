#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <iomanip>
#include <sstream>

using namespace std;

#define TECLA_ARRIBA   72
#define TECLA_ABAJO    80
#define TECLA_ENTER    13
#define TECLA_ESPECIAL 224
#define ANCHO_MARCO    56

// ============================================================
//  ESTRUCTURAS CON PUNTEROS
// ============================================================
struct NodoPromesa {
    int    id;
    string nombre;
    float  precio;
    float  multa;
    NodoPromesa* siguiente;
};

struct NodoVenta {
    int    id;
    string cliente;
    int    promesaId;
    string nombrePromesa;
    float  total;
    string estado;
    string fecha;
    NodoVenta* siguiente;
    NodoVenta* anterior;
};

struct NodoUsuario {
    string username;
    string password;
    NodoUsuario* siguiente;
};

// ============================================================
//  PUNTEROS GLOBALES
// ============================================================
NodoPromesa* listaPromesas = nullptr;
NodoVenta* listaVentas = nullptr;
NodoVenta* colaVentas = nullptr;
NodoUsuario* listaUsuarios = nullptr;
int contadorPromesas = 0;
int contadorVentas = 0;

// ============================================================
//  UTILIDADES DE CONSOLA
// ============================================================
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void ocultarCursor() {
    CONSOLE_CURSOR_INFO i = { 100, FALSE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &i);
}
void mostrarCursor() {
    CONSOLE_CURSOR_INFO i = { 100, TRUE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &i);
}

void gotoxy(int x, int y) {
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y = y;
    SetConsoleCursorPosition(hCon, dwPos);
}
int obtenerAnchoConsola() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

int obtenerAltoConsola() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

string M() {

    int ancho = obtenerAnchoConsola();

    int margen = (ancho - ANCHO_MARCO) / 2;

    if (margen < 0)
        margen = 0;

    return string(margen, ' ');
}
string dirMem(void* p) {
    if (!p) return "nullptr";
    ostringstream ss;
    ss << "0x" << hex << uppercase << (uintptr_t)p;
    return ss.str();
}

// ============================================================
//  MARCO CENTRADO  (solo con cout, sin gotoxy)
// ============================================================
void marcoTope(int c = 11) {
    setColor(c);
    cout << M() << char(201);
    for (int i = 0; i < ANCHO_MARCO - 2; i++) cout << char(205);
    cout << char(187) << "\n";
}
void marcoBase(int c = 11) {
    setColor(c);
    cout << M() << char(200);
    for (int i = 0; i < ANCHO_MARCO - 2; i++) cout << char(205);
    cout << char(188) << "\n";
}
void marcoSep(int c = 11) {
    setColor(c);
    cout << M() << char(204);
    for (int i = 0; i < ANCHO_MARCO - 2; i++) cout << char(205);
    cout << char(185) << "\n";
}
void marcoLinea(string texto, int colMarco, int colTexto) {
    int anchoUtil = ANCHO_MARCO - 2; // Espacio real disponible dentro del marco
    int tamTexto = (int)texto.size();

    // Si el texto es más largo que el marco, lo recortamos para evitar romper el dibujo
    if (tamTexto > anchoUtil) {
        texto = texto.substr(0, anchoUtil);
        tamTexto = anchoUtil;
    }

    // Calculamos cuántos espacios en total sobran
    int espaciosTotales = anchoUtil - tamTexto;
    // Los dividimos a la mitad para el lado izquierdo y el derecho
    int espaciosIzquierda = espaciosTotales / 2;
    int espaciosDerecha = espaciosTotales - espaciosIzquierda;

    // 1. Imprimir margen inicial y el borde izquierdo
    cout << M();
    setColor(colMarco);
    cout << (char)186; // Borde║

    // 2. Imprimir los espacios de la izquierda para centrar
    cout << string(espaciosIzquierda, ' ');

    // 3. Imprimir el texto con su respectivo color
    setColor(colTexto);
    cout << texto;

    // 4. Imprimir los espacios de la derecha para completar el ancho
    cout << string(espaciosDerecha, ' ');

    // 5. Imprimir el borde derecho
    setColor(colMarco);
    cout << (char)186 << endl; // Borde ║
}

void marcoIzq(string txt, int cb = 11, int ct = 7) {
    int interior = ANCHO_MARCO - 4;
    if ((int)txt.size() > interior) txt = txt.substr(0, interior);
    setColor(cb); cout << M() << char(186);
    setColor(ct); cout << " " << txt << string(interior - (int)txt.size(), ' ') << " ";
    setColor(cb); cout << char(186) << "\n";
}
void lineas(int n) { for (int i = 0; i < n; i++) cout << "\n"; }

void encabezado(string titulo, int c = 14) {
    system("cls");
    lineas(1);
    marcoTope(c);
    marcoLinea(titulo, c, 15);
    marcoBase(c);
    lineas(1);
    setColor(7);
}

// ============================================================
//  MENU CON FLECHAS CENTRADO
// ============================================================
// ============================================================
//  MENU CON FLECHAS CENTRADO CON GOTOXY — CORREGIDO
// ============================================================
int menuFlechas(string titulo, string ops[], int n,
    int colorMarco = 11,
    int colorSel = 14) {

    int sel = 0;
    ocultarCursor();

    while (true) {
        system("cls");

        // Altura REAL del menú
        int totalFilas = 6 + n;

        // Centro vertical REAL
        int y_inicio = (obtenerAltoConsola() - totalFilas) / 2;

        if (y_inicio < 0)
            y_inicio = 0;

        // Empujar hacia abajo
        for (int i = 0; i < y_inicio; i++)
            cout << "\n";

        // =========================
        // DIBUJO
        // =========================
        marcoTope(colorMarco);
        marcoLinea(titulo, colorMarco, 15);
        marcoSep(colorMarco);

        for (int i = 0; i < n; i++) {

            if (i == sel)
                marcoLinea(">> " + ops[i] + " <<",
                    colorMarco,
                    colorSel);
            else
                marcoIzq("   " + ops[i],
                    colorMarco,
                    7);
        }

        marcoSep(colorMarco);

        marcoLinea(
            "[Flechas] Navegar    [Enter] Seleccionar",
            colorMarco,
            8);

        marcoBase(colorMarco);

        // =========================
        // CONTROLES
        // =========================
        int t = _getch();

        if (t == TECLA_ESPECIAL || t == 0) {

            t = _getch();

            if (t == TECLA_ARRIBA) {
                sel--;

                if (sel < 0)
                    sel = n - 1;
            }
            else if (t == TECLA_ABAJO) {
                sel++;

                if (sel >= n)
                    sel = 0;
            }
        }
        else if (t == TECLA_ENTER) {

            mostrarCursor();
            return sel;
        }
    }
}

// ============================================================
//  MENSAJES DE ERROR / EXITO ESTANDARIZADOS
// ============================================================
void msgError(string msg) {
    setColor(12);
    cout << "\n" << M() << " [!] ERROR: " << msg << "\n";
    setColor(7);
    system("pause");
}
void msgExito(string msg) {
    setColor(10);
    cout << "\n" << M() << " [OK] " << msg << "\n";
    setColor(7);
    system("pause");
}
void msgInfo(string msg) {
    setColor(14);
    cout << M() << " [i] " << msg << "\n";
    setColor(7);
}

// ============================================================
//  DIBUJO DE NODOS EN HORIZONTAL (Lista Simple y Doble)
//  Cada nodo se muestra como una celda: [ dato | dir -> ]
//  Las celdas se encadenan con flechas horizontales
// ============================================================
void dibujarListaSimple(NodoPromesa* cabeza, NodoPromesa* resaltar = nullptr) {
    if (cabeza == nullptr) {
        setColor(8); cout << M() << "  (lista vacia -- nullptr)\n"; setColor(7); return;
    }
    setColor(13);
    cout << M() << "[LISTA SIMPLE - Promesas]  Cabeza -> " << dirMem(cabeza) << "\n\n";

    // Fila 1: topes de celdas
    // Fila 2: contenido
    // Fila 3: direccion
    // Fila 4: puntero siguiente
    // Fila 5: bases

    string fTope = "", fNombre = "", fDir = "", fSig = "", fBase = "", fFlecha = "";
    NodoPromesa* t = cabeza;
    while (t != nullptr) {
        bool esResaltado = (t == resaltar);
        string celda = "| ID:" + to_string(t->id) + " " + t->nombre.substr(0, 8) + " Q" + to_string((int)t->precio) + " |";
        string dirS = "| " + dirMem(t).substr(0, 14) + " |";
        string sigS = "| ->sig:" + dirMem(t->siguiente).substr(0, 8) + " |";

        int w = (int)celda.size(); if ((int)dirS.size() > w) w = (int)dirS.size(); if ((int)sigS.size() > w) w = (int)sigS.size();
        // normalizar ancho
        while ((int)celda.size() < w) celda.insert(celda.size() - 1, " ");
        while ((int)dirS.size() < w) dirS.insert(dirS.size() - 1, " ");
        while ((int)sigS.size() < w) sigS.insert(sigS.size() - 1, " ");

        string tope = "+" + string(w - 2, '-') + "+";
        string base = "+" + string(w - 2, '-') + "+";

        fTope += tope;
        fNombre += celda;
        fDir += dirS;
        fSig += sigS;
        fBase += base;

        if (t->siguiente != nullptr) {
            fTope += "   ";
            fNombre += "-->";
            fDir += "   ";
            fSig += "   ";
            fBase += "   ";
        }
        t = t->siguiente;
    }
    fNombre += "  nullptr";

    setColor(14); cout << M() << fTope << "\n";
    if (resaltar) setColor(11); else setColor(7);
    cout << M() << fNombre << "\n";
    setColor(13); cout << M() << fDir << "\n";
    setColor(8);  cout << M() << fSig << "\n";
    setColor(14); cout << M() << fBase << "\n";
    setColor(7);
}

// ============================================================
// DIBUJAR LISTA DOBLE CON FILTRADO POR USUARIO (Optimizado)
// ============================================================
void dibujarListaDoble(NodoVenta* cabeza, NodoVenta* destacado = nullptr, string usuarioFiltrado = "") {
    if (!cabeza) {
        msgError("La lista de certificados (listaVentas) esta vacia.");
        return;
    }

    NodoVenta* t = cabeza;
    bool tieneRegistros = false;

    // Primero verificamos si el usuario tiene al menos un registro para no dejar la tabla rota
    if (!usuarioFiltrado.empty()) {
        while (t) {
            if (t->cliente == usuarioFiltrado) {
                tieneRegistros = true;
                break;
            }
            t = t->siguiente;
        }
        if (!tieneRegistros) {
            msgError("No se encontraron promesas certificadas para tu usuario.");
            return;
        }
    }

    // Reiniciamos el recorrido para dibujar
    t = cabeza;

    // Imprimir cabecera de la tabla de certificados
    setColor(14); // Amarillo
    cout << M() << "====================================================================\n";
    cout << M() << "  ID  |    CLIENTE    |    PROMESA    |   TOTAL   |   ESTADO   \n";
    cout << M() << "====================================================================\n";

    while (t) {
        // APLICAMOS EL FILTRO LOGÍSTICO:
        // Si 'usuarioFiltrado' está vacío, se muestra todo (Modo Admin).
        // Si no está vacío, solo entra si coincide exactamente con el cliente del nodo.
        if (usuarioFiltrado.empty() || t->cliente == usuarioFiltrado) {

            // Efecto visual si el nodo es el recién interactuado
            if (t == destacado) setColor(240); // Fondo blanco con letras negras
            else setColor(7); // Texto gris estándar

            // Formateo de columnas alineadas usando left e setw para que no se descuadre
            cout << M() << " " << left
                << setw(4) << t->id << " | "
                << setw(13) << t->cliente << " | "
                << setw(13) << t->nombrePromesa << " | "
                << "$ " << setw(7) << fixed << setprecision(2) << t->total << " | "
                << "[" << t->estado << "]\n";
        }
        t = t->siguiente;
    }

    setColor(14);
    cout << M() << "====================================================================\n";
    setColor(7);
}

void dibujarListaCircular(NodoUsuario* cabeza, NodoUsuario* resaltar = nullptr) {
    if (cabeza == nullptr) {
        setColor(8); cout << M() << "  (lista vacia -- nullptr)\n"; setColor(7); return;
    }
    setColor(13);
    cout << M() << "[LISTA CIRCULAR - Usuarios]  Cabeza -> " << dirMem(cabeza) << "\n\n";

    NodoUsuario* t = cabeza;
    string fTope = "", fDato = "", fDir = "", fSig = "", fBase = "", fFlecha = "";
    do {
        bool esRes = (t == resaltar);
        string celda = "| " + t->username.substr(0, 10) + " |";
        string dirS = "| " + dirMem(t).substr(0, 12) + " |";
        string sigS = "| ->" + dirMem(t->siguiente).substr(0, 10) + " |";
        int w = (int)celda.size(); if ((int)dirS.size() > w) w = (int)dirS.size(); if ((int)sigS.size() > w) w = (int)sigS.size();
        while ((int)celda.size() < w) celda.insert(celda.size() - 1, " ");
        while ((int)dirS.size() < w) dirS.insert(dirS.size() - 1, " ");
        while ((int)sigS.size() < w) sigS.insert(sigS.size() - 1, " ");
        string tope = "+" + string(w - 2, '-') + "+";
        fTope += tope;   fDato += celda; fDir += dirS;
        fSig += sigS;   fBase += tope;
        if (t->siguiente != cabeza) {
            fTope += "   "; fDato += "-->"; fDir += "   ";
            fSig += "   "; fBase += "   ";
        }
        t = t->siguiente;
    } while (t != cabeza);

    setColor(14); cout << M() << fTope << "\n";
    setColor(7);  cout << M() << fDato << "\n";
    setColor(13); cout << M() << fDir << "\n";
    setColor(8);  cout << M() << fSig << "\n";
    setColor(14); cout << M() << fBase << "\n";
    setColor(12); cout << M() << "  ^ ultimo ->siguiente apunta de regreso a la cabeza ^\n";
    setColor(7);
}

// ============================================================
//  FICHA DE NODO  (al buscar)
// ============================================================
void fichaNodoPromesa(NodoPromesa* n) {
    if (!n) return;
    lineas(1);
    marcoTope(11);
    marcoLinea("FICHA DEL NODO ENCONTRADO", 11, 15);
    marcoSep(11);
    marcoIzq("Direccion en memoria : " + dirMem(n), 11, 13);
    marcoIzq("ID                   : " + to_string(n->id), 11, 7);
    marcoIzq("Nombre               : " + n->nombre, 11, 7);
    marcoIzq("Precio               : Q" + to_string(n->precio), 11, 7);
    marcoIzq("Multa                : Q" + to_string(n->multa), 11, 7);
    marcoIzq("Puntero ->siguiente  : " + dirMem(n->siguiente), 11, 8);
    marcoBase(11);
    lineas(1);
}

void fichaNodoVenta(NodoVenta* n) {
    if (!n) return;
    lineas(1);
    marcoTope(6);
    marcoLinea("FICHA DEL NODO ENCONTRADO", 6, 15);
    marcoSep(6);
    marcoIzq("Direccion en memoria : " + dirMem(n), 6, 13);
    marcoIzq("ID                   : " + to_string(n->id), 6, 7);
    marcoIzq("Cliente              : " + n->cliente, 6, 7);
    marcoIzq("Promesa              : " + n->nombrePromesa, 6, 7);
    marcoIzq("Total                : Q" + to_string(n->total), 6, 7);
    marcoIzq("Estado               : " + n->estado, 6, 7);
    marcoIzq("Puntero ->anterior   : " + dirMem(n->anterior), 6, 8);
    marcoIzq("Puntero ->siguiente  : " + dirMem(n->siguiente), 6, 8);
    marcoBase(6);
    lineas(1);
}

void fichaNodoUsuario(NodoUsuario* n) {
    if (!n) return;
    lineas(1);
    marcoTope(14);
    marcoLinea("FICHA DEL NODO ENCONTRADO", 14, 15);
    marcoSep(14);
    marcoIzq("Direccion en memoria : " + dirMem(n), 14, 13);
    marcoIzq("Username             : " + n->username, 14, 7);
    marcoIzq("Puntero ->siguiente  : " + dirMem(n->siguiente), 14, 8);
    marcoBase(14);
    lineas(1);
}

// ============================================================
//  LISTA SIMPLE — PROMESAS
// ============================================================
void insertarPromesa(int id, string nombre, float precio, float multa) {
    NodoPromesa* nuevo = new NodoPromesa;
    nuevo->id = id; nuevo->nombre = nombre; nuevo->precio = precio;
    nuevo->multa = multa; nuevo->siguiente = nullptr;
    if (!listaPromesas) { listaPromesas = nuevo; }
    else {
        NodoPromesa* t = listaPromesas;
        while (t->siguiente) t = t->siguiente;
        t->siguiente = nuevo;
    }
    contadorPromesas++;
}

NodoPromesa* buscarPromesaId(int id) {
    NodoPromesa* t = listaPromesas;
    while (t) { if (t->id == id) return t; t = t->siguiente; }
    return nullptr;
}

NodoPromesa* buscarPromesaNombre(string nombre) {
    NodoPromesa* t = listaPromesas;
    while (t) { if (t->nombre == nombre) return t; t = t->siguiente; }
    return nullptr;
}

bool eliminarPromesa(int id) {
    if (!listaPromesas) return false;
    if (listaPromesas->id == id) {
        NodoPromesa* b = listaPromesas;
        listaPromesas = listaPromesas->siguiente;
        delete b; contadorPromesas--; return true;
    }
    NodoPromesa* prev = listaPromesas;
    while (prev->siguiente) {
        if (prev->siguiente->id == id) {
            NodoPromesa* b = prev->siguiente;
            prev->siguiente = b->siguiente;
            delete b; contadorPromesas--; return true;
        }
        prev = prev->siguiente;
    }
    return false;
}

void mostrarPromesasTabla() {
    NodoPromesa* t = listaPromesas;
    if (!t) { setColor(8); cout << M() << "  (catalogo vacio)\n"; setColor(7); return; }
    setColor(14);
    cout << M() << " ID | Nombre              | Precio  | Multa\n";
    cout << M() << "----+---------------------+---------+--------\n";
    setColor(7);
    while (t) {
        cout << M() << " " << left << setw(3) << t->id
            << "| " << setw(20) << t->nombre
            << "| Q" << setw(7) << t->precio
            << "| Q" << t->multa << "\n";
        t = t->siguiente;
    }
    cout << M() << "----+---------------------+---------+--------\n";
}

void guardarPromesas() {
    ofstream f("promesas.txt");
    NodoPromesa* t = listaPromesas;
    while (t) { f << t->id << " " << t->nombre << " " << t->precio << " " << t->multa << "\n"; t = t->siguiente; }
    f.close();
}
void cargarPromesas() {
    while (listaPromesas) { NodoPromesa* b = listaPromesas; listaPromesas = listaPromesas->siguiente; delete b; }
    contadorPromesas = 0;
    ifstream f("promesas.txt"); if (!f.is_open()) return;
    int id; string nombre; float precio, multa;
    while (f >> id >> nombre >> precio >> multa) insertarPromesa(id, nombre, precio, multa);
    f.close();
}
void inicializarPromesas() {
    cargarPromesas();
    if (contadorPromesas == 0) {
        insertarPromesa(1, "El_Arrepentido", 50.0f, 100.0f);
        insertarPromesa(2, "La_Amnesia_Total", 100.0f, 200.0f);
        insertarPromesa(3, "El_Busca_Perdon", 150.0f, 300.0f);
        insertarPromesa(4, "El_Inmortal", 250.0f, 500.0f);
        guardarPromesas();
    }
}

// ============================================================
//  LISTA CIRCULAR — USUARIOS
// ============================================================
void insertarUsuario(string username, string password) {
    NodoUsuario* nuevo = new NodoUsuario;
    nuevo->username = username; nuevo->password = password; nuevo->siguiente = nullptr;
    if (!listaUsuarios) { nuevo->siguiente = nuevo; listaUsuarios = nuevo; }
    else {
        NodoUsuario* u = listaUsuarios;
        while (u->siguiente != listaUsuarios) u = u->siguiente;
        u->siguiente = nuevo; nuevo->siguiente = listaUsuarios;
    }
}
NodoUsuario* buscarUsuario(string username) {
    if (!listaUsuarios) return nullptr;
    NodoUsuario* t = listaUsuarios;
    do { if (t->username == username) return t; t = t->siguiente; } while (t != listaUsuarios);
    return nullptr;
}
bool loginUsuario(string u, string p) {
    NodoUsuario* n = buscarUsuario(u);
    return n && n->password == p;
}
bool eliminarUsuario(string username) {
    if (!listaUsuarios) return false;

    // Caso 1: Solo existe un usuario en la lista
    if (listaUsuarios->siguiente == listaUsuarios && listaUsuarios->username == username) {
        delete listaUsuarios;
        listaUsuarios = nullptr;
        return true;
    }

    // Buscamos el nodo anterior al que queremos eliminar
    NodoUsuario* prev = listaUsuarios;
    while (prev->siguiente != listaUsuarios && prev->siguiente->username != username) {
        prev = prev->siguiente;
    }

    // Caso 2: El usuario fue encontrado
    if (prev->siguiente->username == username) {
        NodoUsuario* b = prev->siguiente;

        // Si el nodo a eliminar es la cabeza actual, movemos la cabeza antes de borrar
        if (b == listaUsuarios) {
            listaUsuarios = listaUsuarios->siguiente;
        }

        prev->siguiente = b->siguiente; // Reenlazamos el puente circular
        delete b;
        return true;
    }
    return false;
}
void guardarUsuarios() {
    if (!listaUsuarios) { ofstream f("usuarios.txt"); f.close(); return; }
    ofstream f("usuarios.txt");
    NodoUsuario* t = listaUsuarios;
    do { f << t->username << " " << t->password << "\n"; t = t->siguiente; } while (t != listaUsuarios);
    f.close();
}
void cargarUsuarios() {
    if (listaUsuarios) {
        // Rompemos la circularidad de forma segura convirtiéndola temporalmente en lista simple
        NodoUsuario* u = listaUsuarios;
        while (u->siguiente != listaUsuarios) {
            u = u->siguiente;
        }
        u->siguiente = nullptr;

        // Ahora liberamos nodo por nodo sin peligro
        NodoUsuario* t = listaUsuarios;
        while (t) {
            NodoUsuario* b = t;
            t = t->siguiente;
            delete b;
        }
        listaUsuarios = nullptr;
    }

    ifstream f("usuarios.txt");
    if (!f.is_open()) return;
    string u, p;
    while (f >> u >> p) {
        insertarUsuario(u, p);
    }
    f.close();
}

// ============================================================
//  LISTA DOBLE — VENTAS
// ============================================================
void insertarVenta(int id, string cliente, int promesaId, string nombrePromesa,
    float total, string estado, string fecha) {
    NodoVenta* n = new NodoVenta;
    n->id = id; n->cliente = cliente; n->promesaId = promesaId;
    n->nombrePromesa = nombrePromesa; n->total = total;
    n->estado = estado; n->fecha = fecha; n->siguiente = nullptr; n->anterior = nullptr;
    if (!listaVentas) { listaVentas = n; colaVentas = n; }
    else { n->anterior = colaVentas; colaVentas->siguiente = n; colaVentas = n; }
    contadorVentas++;
}
NodoVenta* buscarVentaId(int id) {
    NodoVenta* t = listaVentas;
    while (t) { if (t->id == id) return t; t = t->siguiente; }
    return nullptr;
}
bool eliminarVenta(int id) {
    NodoVenta* t = listaVentas;
    while (t) {
        if (t->id == id) {
            if (t->anterior) t->anterior->siguiente = t->siguiente; else listaVentas = t->siguiente;
            if (t->siguiente) t->siguiente->anterior = t->anterior; else colaVentas = t->anterior;
            delete t; contadorVentas--; return true;
        }
        t = t->siguiente;
    }
    return false;
}
void guardarVentas() {
    ofstream f("ventas.txt");
    NodoVenta* t = listaVentas;
    while (t) {
        f << t->id << " " << t->cliente << " " << t->promesaId << " " << t->nombrePromesa
            << " " << t->total << " " << t->estado << " " << t->fecha << "\n"; t = t->siguiente;
    }
    f.close();
}
void cargarVentas() {
    while (listaVentas) { NodoVenta* b = listaVentas; listaVentas = listaVentas->siguiente; delete b; }
    colaVentas = nullptr; contadorVentas = 0;
    ifstream f("ventas.txt"); if (!f.is_open()) return;
    int id, promesaId; string cliente, nombrePromesa, estado, fecha; float total;
    while (f >> id >> cliente >> promesaId >> nombrePromesa >> total >> estado >> fecha)
        insertarVenta(id, cliente, promesaId, nombrePromesa, total, estado, fecha);
    f.close();
}

// ============================================================
//  PANTALLA DE INICIO
// ============================================================
// ============================================================
//  PANTALLA DE INICIO - CORREGIDA
// ============================================================
void pantallaInicio() {
    system("cls");

    int alto = 18;
    int y = (obtenerAltoConsola() - alto) / 2;

    for (int i = 0; i < y; i++)
        cout << "\n";

    marcoTope(6);
    marcoLinea("LA TABERNA DE LAS PROMESAS", 6, 14);
    marcoSep(6);
    marcoLinea("Antonio | Waldemar | Luis | Oscar", 6, 11);
    marcoSep(6);

    // Espacio superior más pequeño
    marcoLinea("", 6, 6);

    string art[] = {
        "        _.._..,_,_        ",
        "       (          )       ",
        "        ]~,\"-.-~~[        ",
        "      .=])' (;  ([='      ",
        "      | ]:: '    [ |      ",
        "      '=]): .)  ([='      ",
        "        |:: '    |        ",
        "         ~~----~~         ",
        "",
        "    PROMESA DE MENIQUE    "
    };

    int nArt = 8;

    // Mostrar dibujo centrado
    for (int i = 0; i < nArt; i++) {
        marcoLinea(art[i], 6, 14);
    }

    // Espacio pequeño
    marcoLinea("", 6, 6);

    // Texto centrado
    marcoLinea("PARA QUE NO OLVIDES", 6, 14);

    // Espacio inferior pequeño
    marcoLinea("", 6, 6);

    marcoSep(6);
    marcoLinea("PRESIONE ENTER PARA CONTINUAR...", 6, 14);
    marcoBase(6);

    mostrarCursor();
    cin.get();
}
// ============================================================
//  TICKET
// ============================================================
void imprimirTicket(string cliente, int idVenta, string nombrePromesa, float precio, float multa) {
    system("cls"); lineas(2);
    ostringstream ss;
    marcoTope(15);
    marcoLinea("TICKET: LA TABERNA DE LAS PROMESAS", 15, 15);
    marcoSep(15);
    marcoIzq("No. Certificado : " + to_string(idVenta), 15, 7);
    marcoIzq("Cliente         : " + cliente, 15, 7);
    marcoIzq("Promesa         : " + nombrePromesa, 15, 7);
    marcoSep(15);
    ss.str(""); ss << fixed << setprecision(2) << "Subtotal        : Q" << precio;
    marcoIzq(ss.str(), 15, 7);
    ss.str(""); ss << fixed << setprecision(2) << "IVA (12%)       : Q" << precio * 0.12f;
    marcoIzq(ss.str(), 15, 7);
    ss.str(""); ss << fixed << setprecision(2) << "TOTAL A PAGAR   : Q" << precio * 1.12f;
    marcoIzq(ss.str(), 15, 14);
    marcoSep(15);
    ss.str(""); ss << fixed << setprecision(2) << "Multa si rompe  : Q" << multa;
    marcoIzq(ss.str(), 15, 12);
    marcoSep(15);
    marcoLinea("Antonio | Waldemar | Luis | Oscar", 15, 8);
    marcoLinea("Si la rompe... le cobramos. Prometido.", 15, 8);
    marcoBase(15);
    lineas(1); setColor(7); system("pause");
}

// ============================================================
//  Autor: Waldemar Sicajau — MENU USUARIO
// ============================================================
void menuUsuario(string nombreU) {
    string opcs[] = {
        "Ver catalogo de promesas",
        "Certificar una promesa",
        "Ver mis promesas certificadas",
        "Actualizar estado de mi promesa",
        "Cambiar mi contrasena",
        "Cerrar sesion"
    };
    int sel;
    do {
        sel = menuFlechas("BIENVENIDO/A: " + nombreU, opcs, 6, 11, 14);

        // --- VER CATALOGO ---
        if (sel == 0) {
            encabezado("CATALOGO DE PROMESAS  [Lista Simple]", 14);
            mostrarPromesasTabla();
            lineas(1);
            dibujarListaSimple(listaPromesas);
            lineas(1); system("pause");
        }
        else if (sel == 2) {
            encabezado("MIS PROMESAS  [Lista Doble]", 14);

            // Pasamos 'nombreU' al final para activar el filtro automático por usuario
            dibujarListaDoble(listaVentas, nullptr, nombreU);

            lineas(1);
            system("pause");
        }

        // --- CERTIFICAR PROMESA ---
        else if (sel == 1) {
            encabezado("CERTIFICAR PROMESA  [Lista Simple -> Lista Doble]", 14);
            mostrarPromesasTabla();
            lineas(1);
            dibujarListaSimple(listaPromesas);
            lineas(1);
            mostrarCursor();
            int idSel; cout << M() << " Seleccione ID de promesa: "; cin >> idSel;
            NodoPromesa* prom = buscarPromesaId(idSel);
            if (!prom) {
                msgError("El ID " + to_string(idSel) + " no existe en el catalogo.");
            }
            else {
                // Mostrar ficha del nodo encontrado
                fichaNodoPromesa(prom);

                time_t t = time(0); char* fecha = ctime(&t);
                string fs = string(fecha);
                if (!fs.empty()) fs.pop_back();
                for (char& c : fs) if (c == ' ') c = '_';
                int nid = contadorVentas + 1;

                // Estructura ANTES de insertar
                msgInfo("Estructura de ventas ANTES de insertar:");
                lineas(1);
                dibujarListaDoble(listaVentas);
                lineas(1);

                insertarVenta(nid, nombreU, prom->id, prom->nombre,
                    prom->precio * 1.12f, "Activa", fs);
                guardarVentas();

                // Estructura DESPUES de insertar
                msgInfo("Estructura de ventas DESPUES de insertar:");
                lineas(1);
                dibujarListaDoble(listaVentas, buscarVentaId(nid));
                lineas(1);
                system("pause");
                imprimirTicket(nombreU, nid, prom->nombre, prom->precio, prom->multa);
            }
        }

        // --- VER MIS PROMESAS ---
        else if (sel == 2) {
            encabezado("MIS PROMESAS  [Lista Doble]", 14);

            NodoVenta* t = listaVentas;
            int n = 0;

            // Primero contamos si este usuario específico tiene promesas
            while (t) {
                if (t->cliente == nombreU) n++;
                t = t->siguiente;
            }

            if (n == 0) {
                msgError("No tienes ninguna promesa certificada todavia.");
            }
            else {
                // Reiniciamos el puntero al inicio de la lista doble
                t = listaVentas;

                msgInfo("Tus certificados activos en el sistema:");
                lineas(1);

                // Recorremos la lista y SOLO imprimimos las que pertenecen al usuario
                while (t) {
                    if (t->cliente == nombreU) {
                        // Formato de ficha simplificada para cada una de sus promesas
                        setColor(11); // Color azul claro
                        cout << M() << "==========================================\n";
                        cout << M() << " CERTIFICADO #" << t->id << "\n";
                        cout << M() << "==========================================\n";
                        setColor(7); // Texto normal
                        cout << M() << " Promesa  : " << t->nombrePromesa << "\n";
                        cout << M() << " Total    : $" << t->total << "\n";
                        cout << M() << " Estado   : [" << t->estado << "]\n";
                        cout << M() << " Fecha    : " << t->fecha << "\n\n";
                    }
                    t = t->siguiente;
                }

                // Si tienes una función como dibujarListaDoble Filtrada puedes usarla,
                // pero este bucle directo asegura que solo se impriman las suyas.
                lineas(1);
                system("pause");
            }
        }

        // --- ACTUALIZAR ESTADO ---
        else if (sel == 3) {
            encabezado("ACTUALIZAR ESTADO  [Lista Doble]", 14);
            NodoVenta* t = listaVentas; int n = 0;
            while (t) {
                if (t->cliente == nombreU) {
                    setColor(11); cout << M() << " #" << t->id << " | " << t->nombrePromesa << " | " << t->estado << "\n";
                    n++;
                }
                t = t->siguiente;
            }
            if (n == 0) {
                msgError("No tienes promesas certificadas para actualizar.");
            }
            else {
                mostrarCursor();
                int idSel; cout << "\n" << M() << " # a actualizar: "; cin >> idSel;
                NodoVenta* nodo = buscarVentaId(idSel);
                if (!nodo || nodo->cliente != nombreU) {
                    msgError("El certificado #" + to_string(idSel) + " no existe o no te pertenece.");
                }
                else {
                    // Mostrar ficha
                    fichaNodoVenta(nodo);
                    string opcsE[] = { "Activa","Cumplida","Rota","Cancelar" };
                    int estSel = menuFlechas("NUEVO ESTADO", opcsE, 4, 14, 10);
                    if (estSel == 0) nodo->estado = "Activa";
                    else if (estSel == 1) nodo->estado = "Cumplida";
                    else if (estSel == 2) nodo->estado = "Rota";
                    if (estSel != 3) {
                        guardarVentas();
                        encabezado("ESTADO ACTUALIZADO", 10);
                        msgInfo("El nodo ha sido modificado en memoria:");
                        fichaNodoVenta(nodo);
                        dibujarListaDoble(listaVentas, nodo, nombreU);
                        lineas(1); system("pause");
                    }
                }
            }
        }

        // --- CAMBIAR CONTRASENA ---
        else if (sel == 4) {
            encabezado("CAMBIAR CONTRASENA  [Lista Circular]", 14);
            mostrarCursor();
            string nueva; cout << M() << " Nueva contrasena: "; cin >> nueva;
            NodoUsuario* nodo = buscarUsuario(nombreU);
            if (nodo) {
                nodo->password = nueva; guardarUsuarios();
                msgExito("Contrasena actualizada correctamente.");
            }
            else {
                msgError("No se encontro el usuario en la lista circular.");
            }
        }

    } while (sel != 5);
}

// ============================================================
//  Autor: Luis Samines — MENU ADMINISTRADOR
// ============================================================
// ============================================================
//  MENU ADMINISTRADOR — REQUERIMIENTOS AL 100%
// ============================================================
void menuAdministrador() {
    string opcs[] = {
        "Ver catalogo de promesas (L. Simple)",
        "Agregar promesa",
        "Editar promesa",
        "Eliminar promesa",
        "Vaciar catalogo de promesas",          // <- NUEVO (Requerimiento Vaciado)
        "Ver usuarios (L. Circular)",
        "Agregar usuario",
        "Editar contrasena de usuario",
        "Eliminar usuario",
        "Vaciar lista de usuarios",             // <- NUEVO (Requerimiento Vaciado)
        "Reporte general de ventas (L. Doble)",
        "Eliminar registro de venta",           // <- NUEVO (Uso de eliminarVenta)
        "Vaciar registro de ventas",            // <- NUEVO (Requerimiento Vaciado)
        "Salir al menu principal"
    };
    int sel;
    do {
        // Pasamos 14 como el número total de opciones
        sel = menuFlechas("PANEL DE CONTROL: ADMINISTRADOR", opcs, 14, 12, 14);

        // --- 0. VER CATALOGO ---
        if (sel == 0) {
            encabezado("CATALOGO  [Lista Simple]", 12);
            mostrarPromesasTabla();
            lineas(1);
            dibujarListaSimple(listaPromesas);
            lineas(1); system("pause");
        }

        // --- 1. AGREGAR PROMESA ---
        else if (sel == 1) {
            encabezado("AGREGAR PROMESA  [Lista Simple]", 12);
            msgInfo("Estructura ANTES de insertar:");
            lineas(1);
            dibujarListaSimple(listaPromesas);
            lineas(1);
            mostrarCursor();
            string nombre; float precio, multa;
            cout << M() << " Nombre (sin espacios): "; cin >> nombre;
            if (buscarPromesaNombre(nombre)) {
                msgError("Ya existe una promesa con el nombre '" + nombre + "'.");
            }
            else {
                cout << M() << " Precio Q: "; cin >> precio;
                cout << M() << " Multa  Q: "; cin >> multa;
                insertarPromesa(contadorPromesas + 1, nombre, precio, multa);
                guardarPromesas();
                NodoPromesa* nuevo = buscarPromesaNombre(nombre);
                encabezado("PROMESA AGREGADA", 10);
                msgInfo("Estructura DESPUES de insertar:");
                lineas(1);
                dibujarListaSimple(listaPromesas, nuevo);
                lineas(1);
                fichaNodoPromesa(nuevo);
                msgExito("Promesa '" + nombre + "' agregada correctamente.");
            }
        }

        // --- 2. EDITAR PROMESA ---
        else if (sel == 2) {
            encabezado("EDITAR PROMESA  [Lista Simple]", 12);
            if (!listaPromesas) {
                msgError("Estructura vacia. No hay promesas para modificar.");
            }
            else {
                mostrarPromesasTabla();
                mostrarCursor();
                int idSel; cout << "\n" << M() << " ID a editar: "; cin >> idSel;
                NodoPromesa* nodo = buscarPromesaId(idSel);
                if (!nodo) {
                    msgError("Nodo no encontrado. No existe promesa con ID " + to_string(idSel) + ".");
                }
                else {
                    fichaNodoPromesa(nodo);
                    cout << M() << " Nuevo nombre: ";  cin >> nodo->nombre;
                    cout << M() << " Nuevo precio: ";  cin >> nodo->precio;
                    cout << M() << " Nueva multa:  ";  cin >> nodo->multa;
                    guardarPromesas();
                    encabezado("PROMESA EDITADA", 10);
                    msgInfo("Nodo modificado en memoria:");
                    fichaNodoPromesa(nodo);
                    dibujarListaSimple(listaPromesas, nodo);
                    lineas(1);
                    msgExito("Promesa actualizada correctamente.");
                }
            }
        }

        // --- 3. ELIMINAR PROMESA ---
        else if (sel == 3) {
            encabezado("ELIMINAR PROMESA  [Lista Simple]", 12);
            if (!listaPromesas) {
                msgError("Estructura vacia. No hay nodos que eliminar.");
            }
            else {
                mostrarPromesasTabla();
                mostrarCursor();
                int idSel; cout << "\n" << M() << " ID a eliminar: "; cin >> idSel;
                NodoPromesa* nodo = buscarPromesaId(idSel);
                if (!nodo) {
                    msgError("Nodo no encontrado. No existe promesa con ID " + to_string(idSel) + ".");
                }
                else {
                    encabezado("ELIMINANDO PROMESA", 12);
                    msgInfo("Estructura ANTES de eliminar:");
                    lineas(1);
                    dibujarListaSimple(listaPromesas, nodo);
                    lineas(1);
                    fichaNodoPromesa(nodo);
                    eliminarPromesa(idSel);
                    guardarPromesas();
                    msgInfo("Estructura DESPUES de eliminar:");
                    lineas(1);
                    dibujarListaSimple(listaPromesas);
                    lineas(1);
                    msgExito("Nodo eliminado y memoria liberada correctamente.");
                }
            }
        }

        // --- 4. VACIAR CATALOGO DE PROMESAS ---
        else if (sel == 4) {
            encabezado("VACIAR CATALOGO  [Lista Simple]", 12);
            if (!listaPromesas) {
                msgError("Estructura vacia. No hay nada que vaciar.");
            }
            else {
                mostrarCursor();
                cout << M() << " ¿Esta seguro de vaciar TODA la estructura? (S/N): ";
                char op; cin >> op;
                if (op == 'S' || op == 's') {
                    while (listaPromesas) {
                        NodoPromesa* b = listaPromesas;
                        listaPromesas = listaPromesas->siguiente;
                        delete b;
                    }
                    contadorPromesas = 0;
                    guardarPromesas();
                    msgExito("Estructura vaciada por completo. Todos los nodos fueron liberados.");
                }
                else {
                    msgInfo("Operacion cancelada.");
                    system("pause");
                }
            }
        }

        // --- 5. VER USUARIOS ---
        else if (sel == 5) {
            encabezado("USUARIOS  [Lista Circular]", 12);
            dibujarListaCircular(listaUsuarios);
            lineas(1); system("pause");
        }

        // --- 6. AGREGAR USUARIO ---
        else if (sel == 6) {
            encabezado("AGREGAR USUARIO  [Lista Circular]", 12);
            msgInfo("Estructura ANTES de insertar:");
            lineas(1);
            dibujarListaCircular(listaUsuarios);
            lineas(1);
            mostrarCursor();
            string usr, pwd;
            cout << M() << " Nuevo username  : "; cin >> usr;
            if (buscarUsuario(usr)) {
                msgError("El usuario '" + usr + "' ya existe en la lista circular.");
            }
            else {
                cout << M() << " Nueva contrasena: "; cin >> pwd;
                insertarUsuario(usr, pwd);
                guardarUsuarios();
                NodoUsuario* nuevo = buscarUsuario(usr);
                encabezado("USUARIO CREADO", 10);
                msgInfo("Estructura DESPUES de insertar:");
                lineas(1);
                dibujarListaCircular(listaUsuarios, nuevo);
                lineas(1);
                fichaNodoUsuario(nuevo);
                msgExito("Usuario '" + usr + "' creado correctamente.");
            }
        }

        // --- 7. EDITAR USUARIO ---
        else if (sel == 7) {
            encabezado("EDITAR USUARIO  [Lista Circular]", 12);
            if (!listaUsuarios) {
                msgError("Estructura vacia. No hay usuarios registrados.");
            }
            else {
                dibujarListaCircular(listaUsuarios);
                mostrarCursor();
                string usr; cout << "\n" << M() << " Username a editar: "; cin >> usr;
                NodoUsuario* nodo = buscarUsuario(usr);
                if (!nodo) {
                    msgError("Nodo no encontrado. El usuario '" + usr + "' no existe.");
                }
                else {
                    fichaNodoUsuario(nodo);
                    cout << M() << " Nueva contrasena: "; cin >> nodo->password;
                    guardarUsuarios();
                    msgExito("Contrasena del usuario '" + usr + "' actualizada correctamente.");
                }
            }
        }

        // --- 8. ELIMINAR USUARIO ---
        else if (sel == 8) {
            encabezado("ELIMINAR USUARIO  [Lista Circular]", 12);
            if (!listaUsuarios) {
                msgError("Estructura vacia. No hay usuarios que eliminar.");
            }
            else {
                msgInfo("Estructura ANTES de eliminar:");
                lineas(1);
                dibujarListaCircular(listaUsuarios);
                lineas(1);
                mostrarCursor();
                string usr; cout << M() << " Username a eliminar: "; cin >> usr;
                NodoUsuario* nodo = buscarUsuario(usr);
                if (!nodo) {
                    msgError("Nodo no encontrado. El usuario '" + usr + "' no existe.");
                }
                else {
                    fichaNodoUsuario(nodo);
                    eliminarUsuario(usr);
                    guardarUsuarios();
                    msgInfo("Estructura DESPUES de eliminar:");
                    lineas(1);
                    dibujarListaCircular(listaUsuarios);
                    lineas(1);
                    msgExito("Nodo eliminado y memoria liberada correctamente.");
                }
            }
        }

        // --- 9. VACIAR LISTA DE USUARIOS ---
        else if (sel == 9) {
            encabezado("VACIAR USUARIOS  [Lista Circular]", 12);
            if (!listaUsuarios || (listaUsuarios->siguiente == listaUsuarios && listaUsuarios->username == "admin")) {
                msgError("La estructura esta vacia o solo contiene la cuenta de respaldo 'admin'.");
            }
            else {
                mostrarCursor();
                cout << M() << " ¿Seguro de vaciar todos los usuarios? (S/N): ";
                char op; cin >> op;
                if (op == 'S' || op == 's') {
                    NodoUsuario* u = listaUsuarios;
                    while (u->siguiente != listaUsuarios) u = u->siguiente;
                    u->siguiente = nullptr;
                    NodoUsuario* t = listaUsuarios;
                    while (t) {
                        NodoUsuario* b = t;
                        t = t->siguiente;
                        delete b;
                    }
                    listaUsuarios = nullptr;
                    insertarUsuario("admin", "admin123");
                    guardarUsuarios();
                    msgExito("Estructura vaciada. Se conservo la credencial 'admin' de fabrica por respaldo.");
                }
                else {
                    msgInfo("Operacion cancelada.");
                    system("pause");
                }
            }
        }

        // --- 10. REPORTE GENERAL DE VENTAS ---
        else if (sel == 10) {
            encabezado("REPORTE GENERAL  [Lista Doble]", 12);
            dibujarListaDoble(listaVentas);
            lineas(1);
            if (!listaVentas) {
                msgError("Estructura vacia. No se han registrado certificaciones.");
            }
            else {
                setColor(14);
                cout << M() << " #  | Cliente      | Promesa         | Total  | Estado\n";
                cout << M() << "----+--------------+-----------------+--------+---------\n";
                setColor(7);
                NodoVenta* t = listaVentas;
                float totalCaja = 0; int activas = 0, cumplidas = 0, rotas = 0;
                while (t) {
                    cout << M() << " " << left << setw(3) << t->id
                        << "| " << setw(13) << t->cliente
                        << "| " << setw(16) << t->nombrePromesa
                        << "| Q" << setw(6) << t->total
                        << "| " << t->estado << "\n";
                    totalCaja += t->total;
                    if (t->estado == "Activa") activas++;
                    else if (t->estado == "Cumplida") cumplidas++;
                    else if (t->estado == "Rota") rotas++;
                    t = t->siguiente;
                }
                cout << M() << "----+--------------+-----------------+--------+---------\n";
                setColor(14);
                cout << M() << " Total:" << contadorVentas << " Activas:" << activas << " Cumplidas:" << cumplidas << " Rotas:" << rotas << "\n";
                setColor(10);
                cout << M() << " TOTAL RECAUDADO: Q" << totalCaja << "\n";
                lineas(1); system("pause");
            }
        }

        // --- 11. ELIMINAR REGISTRO DE VENTA ---
        else if (sel == 11) {
            encabezado("ELIMINAR CERTIFICADO  [Lista Doble]", 12);
            if (!listaVentas) {
                msgError("Estructura vacia. No hay ventas/certificaciones registradas.");
            }
            else {
                dibujarListaDoble(listaVentas);
                mostrarCursor();
                int idSel; cout << "\n" << M() << " ID de certificado a eliminar: "; cin >> idSel;
                NodoVenta* nodo = buscarVentaId(idSel);
                if (!nodo) {
                    msgError("Nodo no encontrado. El certificado #" + to_string(idSel) + " no existe.");
                }
                else {
                    encabezado("ELIMINANDO DE LISTA DOBLE", 12);
                    msgInfo("Estructura ANTES de eliminar:");
                    lineas(1);
                    dibujarListaDoble(listaVentas, nodo);
                    lineas(1);
                    fichaNodoVenta(nodo);

                    eliminarVenta(idSel);
                    guardarVentas();

                    msgInfo("Estructura DESPUES de eliminar:");
                    lineas(1);
                    dibujarListaDoble(listaVentas);
                    lineas(1);
                    msgExito("Nodo de lista doble desvinculado de memoria correctamente.");
                }
            }
        }

        // --- 12. VACIAR REGISTRO DE VENTAS ---
        else if (sel == 12) {
            encabezado("VACIAR REGISTRO VENTAS  [Lista Doble]", 12);
            if (!listaVentas) {
                msgError("Estructura vacia. No hay nada que vaciar.");
            }
            else {
                mostrarCursor();
                cout << M() << " ¿Seguro de borrar por completo el historico de ventas? (S/N): ";
                char op; cin >> op;
                if (op == 'S' || op == 's') {
                    while (listaVentas) {
                        NodoVenta* b = listaVentas;
                        listaVentas = listaVentas->siguiente;
                        delete b;
                    }
                    colaVentas = nullptr;
                    contadorVentas = 0;
                    guardarVentas();
                    msgExito("Estructura vaciada por completo. Historial de ventas formateado.");
                }
                else {
                    msgInfo("Operacion cancelada.");
                    system("pause");
                }
            }
        }

    } while (sel != 13);
}
// ============================================================
//  Autor: Oscar Ben — MAIN
// ============================================================
int main() {
    system("mode con: cols=120 lines=35");
    cargarUsuarios(); inicializarPromesas(); cargarVentas();
    if (!listaUsuarios) { insertarUsuario("admin", "admin123"); guardarUsuarios(); }

    pantallaInicio();

    string opcsMain[] = { "Iniciar Sesion","Registrarse (Nuevo Usuario)","Modo Administrador","Salir" };
    int sel;
    do {
        sel = menuFlechas("SISTEMA DE PROMESAS DE BORRACHERA", opcsMain, 4, 11, 14);

        // --- LOGIN ---
        if (sel == 0) {
            encabezado("INICIAR SESION  [Lista Circular]", 11);
            mostrarCursor();
            string u, p; bool ok = false; int intentos = 3;
            while (intentos > 0 && !ok) {
                setColor(7);
                cout << M() << " Usuario   : "; cin >> u;
                cout << M() << " Contrasena: "; cin >> p;
                NodoUsuario* nodo = buscarUsuario(u);
                if (loginUsuario(u, p)) {
                    ok = true;
                }
                else {
                    intentos--;
                    if (intentos > 0)
                        msgError("Credenciales incorrectas. Te quedan " + to_string(intentos) + " intento(s).");
                    else
                        msgError("Demasiados intentos fallidos. Acceso bloqueado por seguridad.");
                }
            }
            if (ok) menuUsuario(u);
        }

        // --- REGISTRO ---
        else if (sel == 1) {
            encabezado("REGISTRARSE  [Lista Circular]", 11);
            msgInfo("Estructura actual de usuarios:");
            lineas(1);
            dibujarListaCircular(listaUsuarios);
            lineas(1);
            mostrarCursor();
            string u, p;
            cout << M() << " Nuevo usuario   : "; cin >> u;
            if (buscarUsuario(u)) {
                msgError("El usuario '" + u + "' ya esta registrado. Elige otro nombre.");
            }
            else {
                cout << M() << " Nueva contrasena: "; cin >> p;
                insertarUsuario(u, p);
                guardarUsuarios();
                NodoUsuario* nuevo = buscarUsuario(u);
                encabezado("REGISTRO EXITOSO", 10);
                msgInfo("Estructura DESPUES del registro:");
                lineas(1);
                dibujarListaCircular(listaUsuarios, nuevo);
                lineas(1);
                fichaNodoUsuario(nuevo);
                msgExito("Usuario '" + u + "' registrado correctamente. Ya puedes iniciar sesion.");
            }
        }

        // --- ADMINISTRADOR ---
        else if (sel == 2) {
            encabezado("MODO ADMINISTRADOR", 12);
            mostrarCursor();
            string clave; cout << M() << " Clave: "; cin >> clave;
            if (clave == "admin123") menuAdministrador();
            else msgError("Clave incorrecta. Acceso denegado al panel de administrador.");
        }

    } while (sel != 3);

    // Liberar lista simple
    while (listaPromesas) { NodoPromesa* t = listaPromesas; listaPromesas = listaPromesas->siguiente; delete t; }
    // Liberar lista doble
    while (listaVentas) { NodoVenta* t = listaVentas; listaVentas = listaVentas->siguiente; delete t; }
    // Liberar lista circular
    if (listaUsuarios) {
        NodoUsuario* u = listaUsuarios;
        while (u->siguiente != listaUsuarios) u = u->siguiente;
        u->siguiente = nullptr;
        NodoUsuario* t = listaUsuarios;
        while (t) { NodoUsuario* b = t; t = t->siguiente; delete b; }
    }
    setColor(7);
    cout << "\n" << M() << " Hasta luego! No olvide pagar sus promesas.\n";
    return 0;
}

