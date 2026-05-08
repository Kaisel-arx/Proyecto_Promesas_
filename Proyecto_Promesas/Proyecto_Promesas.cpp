#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <ctime>
#include <conio.h>
#include <windows.h>

using namespace std;

#define TECLA_ARRIBA    72
#define TECLA_ABAJO     80
#define TECLA_ENTER     13
#define TECLA_ESPECIAL  224

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

NodoPromesa* listaPromesas = nullptr;
NodoVenta* listaVentas = nullptr;
NodoVenta* colaVentas = nullptr;
NodoUsuario* listaUsuarios = nullptr;
int contadorPromesas = 0;
int contadorVentas = 0;

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void ocultarCursor() {
    CONSOLE_CURSOR_INFO info = { 100, FALSE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void mostrarCursor() {
    CONSOLE_CURSOR_INFO info = { 100, TRUE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

int menuFlechas(string titulo, string opciones[], int numOpciones,
    int colorTitulo = 14, int colorNormal = 7, int colorSelec = 11) {
    int sel = 0;
    ocultarCursor();
    while (true) {
        system("cls");
        setColor(colorTitulo);
        cout << "==========================================" << endl;
        cout << "  " << titulo << endl;
        cout << "==========================================" << endl;
        for (int i = 0; i < numOpciones; i++) {
            if (i == sel) {
                setColor(colorSelec);
                cout << " >> " << opciones[i] << " <<" << endl;
            }
            else {
                setColor(colorNormal);
                cout << "    " << opciones[i] << endl;
            }
        }
        setColor(colorNormal);
        cout << "==========================================" << endl;
        cout << "  [Flechas] Navegar   [Enter] Seleccionar" << endl;

        int tecla = _getch();
        if (tecla == TECLA_ESPECIAL || tecla == 0) {
            tecla = _getch();
            if (tecla == TECLA_ARRIBA)
                sel = (sel - 1 + numOpciones) % numOpciones;
            else if (tecla == TECLA_ABAJO)
                sel = (sel + 1) % numOpciones;
        }
        else if (tecla == TECLA_ENTER) {
            mostrarCursor();
            return sel;
        }
    }
}

void insertarPromesa(int id, string nombre, float precio, float multa) {
    NodoPromesa* nuevo = new NodoPromesa;
    nuevo->id = id;
    nuevo->nombre = nombre;
    nuevo->precio = precio;
    nuevo->multa = multa;
    nuevo->siguiente = nullptr;
    if (listaPromesas == nullptr) {
        listaPromesas = nuevo;
    }
    else {
        NodoPromesa* temp = listaPromesas;
        while (temp->siguiente != nullptr) temp = temp->siguiente;
        temp->siguiente = nuevo;
    }
    contadorPromesas++;
}

NodoPromesa* buscarPromesaId(int id) {
    NodoPromesa* temp = listaPromesas;
    while (temp != nullptr) {
        if (temp->id == id) return temp;
        temp = temp->siguiente;
    }
    return nullptr;
}

NodoPromesa* buscarPromesaNombre(string nombre) {
    NodoPromesa* temp = listaPromesas;
    while (temp != nullptr) {
        if (temp->nombre == nombre) return temp;
        temp = temp->siguiente;
    }
    return nullptr;
}

bool eliminarPromesa(int id) {
    if (listaPromesas == nullptr) return false;
    if (listaPromesas->id == id) {
        NodoPromesa* b = listaPromesas;
        listaPromesas = listaPromesas->siguiente;
        delete b; contadorPromesas--; return true;
    }
    NodoPromesa* prev = listaPromesas;
    while (prev->siguiente != nullptr) {
        if (prev->siguiente->id == id) {
            NodoPromesa* b = prev->siguiente;
            prev->siguiente = b->siguiente;
            delete b; contadorPromesas--; return true;
        }
        prev = prev->siguiente;
    }
    return false;
}

void mostrarPromesas() {
    NodoPromesa* temp = listaPromesas;
    if (temp == nullptr) { cout << " No hay promesas en el catalogo." << endl; return; }
    setColor(14);
    cout << " ID  | Nombre                    | Precio    | Multa" << endl;
    cout << "--------------------------------------------------" << endl;
    setColor(7);
    while (temp != nullptr) {
        cout << " " << temp->id << "   | " << temp->nombre
            << "\t\t| Q" << temp->precio << "\t| Q" << temp->multa << endl;
        temp = temp->siguiente;
    }
}

void guardarPromesas() {
    ofstream f("promesas.txt");
    NodoPromesa* temp = listaPromesas;
    while (temp != nullptr) {
        f << temp->id << " " << temp->nombre << " "
            << temp->precio << " " << temp->multa << endl;
        temp = temp->siguiente;
    }
    f.close();
}

void cargarPromesas() {
    while (listaPromesas != nullptr) {
        NodoPromesa* b = listaPromesas;
        listaPromesas = listaPromesas->siguiente;
        delete b;
    }
    contadorPromesas = 0;
    ifstream f("promesas.txt");
    if (!f.is_open()) return;
    int id; string nombre; float precio, multa;
    while (f >> id >> nombre >> precio >> multa)
        insertarPromesa(id, nombre, precio, multa);
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

void insertarUsuario(string username, string password) {
    NodoUsuario* nuevo = new NodoUsuario;
    nuevo->username = username;
    nuevo->password = password;
    nuevo->siguiente = nullptr;
    if (listaUsuarios == nullptr) {
        nuevo->siguiente = nuevo;
        listaUsuarios = nuevo;
    }
    else {
        NodoUsuario* ultimo = listaUsuarios;
        while (ultimo->siguiente != listaUsuarios) ultimo = ultimo->siguiente;
        ultimo->siguiente = nuevo;
        nuevo->siguiente = listaUsuarios;
    }
}

NodoUsuario* buscarUsuario(string username) {
    if (listaUsuarios == nullptr) return nullptr;
    NodoUsuario* temp = listaUsuarios;
    do {
        if (temp->username == username) return temp;
        temp = temp->siguiente;
    } while (temp != listaUsuarios);
    return nullptr;
}

bool loginUsuario(string username, string password) {
    NodoUsuario* n = buscarUsuario(username);
    return (n != nullptr && n->password == password);
}

void mostrarUsuarios() {
    if (listaUsuarios == nullptr) { cout << " No hay usuarios registrados." << endl; return; }
    setColor(11);
    cout << " Usuario            | Contrasena" << endl;
    cout << "--------------------------------------------------" << endl;
    setColor(7);
    NodoUsuario* temp = listaUsuarios;
    do {
        cout << " " << temp->username << "\t\t| " << temp->password << endl;
        temp = temp->siguiente;
    } while (temp != listaUsuarios);
}

bool eliminarUsuario(string username) {
    if (listaUsuarios == nullptr) return false;
    if (listaUsuarios->siguiente == listaUsuarios &&
        listaUsuarios->username == username) {
        delete listaUsuarios; listaUsuarios = nullptr; return true;
    }
    NodoUsuario* prev = listaUsuarios;
    while (prev->siguiente != listaUsuarios) {
        if (prev->siguiente->username == username) {
            NodoUsuario* b = prev->siguiente;
            prev->siguiente = b->siguiente;
            if (b == listaUsuarios) listaUsuarios = prev->siguiente;
            delete b; return true;
        }
        prev = prev->siguiente;
    }
    if (listaUsuarios->username == username) {
        NodoUsuario* b = listaUsuarios;
        prev->siguiente = listaUsuarios->siguiente;
        listaUsuarios = listaUsuarios->siguiente;
        delete b; return true;
    }
    return false;
}

void guardarUsuarios() {
    if (listaUsuarios == nullptr) { ofstream f("usuarios.txt"); f.close(); return; }
    ofstream f("usuarios.txt");
    NodoUsuario* temp = listaUsuarios;
    do { f << temp->username << " " << temp->password << endl; temp = temp->siguiente; } while (temp != listaUsuarios);
    f.close();
}

void cargarUsuarios() {
    if (listaUsuarios != nullptr) {
        NodoUsuario* ultimo = listaUsuarios;
        while (ultimo->siguiente != listaUsuarios) ultimo = ultimo->siguiente;
        ultimo->siguiente = nullptr;
        NodoUsuario* temp = listaUsuarios;
        while (temp != nullptr) { NodoUsuario* b = temp; temp = temp->siguiente; delete b; }
        listaUsuarios = nullptr;
    }
    ifstream f("usuarios.txt");
    if (!f.is_open()) return;
    string u, p;
    while (f >> u >> p) insertarUsuario(u, p);
    f.close();
}

void insertarVenta(int id, string cliente, int promesaId,
    string nombrePromesa, float total,
    string estado, string fecha) {
    NodoVenta* nuevo = new NodoVenta;
    nuevo->id = id;
    nuevo->cliente = cliente;
    nuevo->promesaId = promesaId;
    nuevo->nombrePromesa = nombrePromesa;
    nuevo->total = total;
    nuevo->estado = estado;
    nuevo->fecha = fecha;
    nuevo->siguiente = nullptr;
    nuevo->anterior = nullptr;
    if (listaVentas == nullptr) {
        listaVentas = nuevo; colaVentas = nuevo;
    }
    else {
        nuevo->anterior = colaVentas;
        colaVentas->siguiente = nuevo;
        colaVentas = nuevo;
    }
    contadorVentas++;
}

NodoVenta* buscarVentaId(int id) {
    NodoVenta* temp = listaVentas;
    while (temp != nullptr) {
        if (temp->id == id) return temp;
        temp = temp->siguiente;
    }
    return nullptr;
}

bool eliminarVenta(int id) {
    NodoVenta* temp = listaVentas;
    while (temp != nullptr) {
        if (temp->id == id) {
            if (temp->anterior != nullptr) temp->anterior->siguiente = temp->siguiente;
            else listaVentas = temp->siguiente;
            if (temp->siguiente != nullptr) temp->siguiente->anterior = temp->anterior;
            else colaVentas = temp->anterior;
            delete temp; contadorVentas--; return true;
        }
        temp = temp->siguiente;
    }
    return false;
}

void guardarVentas() {
    ofstream f("ventas.txt");
    NodoVenta* temp = listaVentas;
    while (temp != nullptr) {
        f << temp->id << " " << temp->cliente << " "
            << temp->promesaId << " " << temp->nombrePromesa << " "
            << temp->total << " " << temp->estado << " " << temp->fecha << endl;
        temp = temp->siguiente;
    }
    f.close();
}

void cargarVentas() {
    while (listaVentas != nullptr) {
        NodoVenta* b = listaVentas; listaVentas = listaVentas->siguiente; delete b;
    }
    colaVentas = nullptr; contadorVentas = 0;
    ifstream f("ventas.txt");
    if (!f.is_open()) return;
    int id, promesaId; string cliente, nombrePromesa, estado, fecha; float total;
    while (f >> id >> cliente >> promesaId >> nombrePromesa >> total >> estado >> fecha)
        insertarVenta(id, cliente, promesaId, nombrePromesa, total, estado, fecha);
    f.close();
}

void pantallaInicio() {
    system("cls");
    setColor(6);
    cout << "==================================================" << endl;
    cout << "     PROYECTO: LA TABERNA DE LAS PROMESAS        " << endl;
    cout << "   Integrantes: Antonio, Waldemar, Luis, Oscar   " << endl;
    cout << "==================================================" << endl;
    cout << "                _.._..,_,_                       " << endl;
    cout << "               (          )                      " << endl;
    cout << "                ]..---..[                        " << endl;
    cout << "              .'          '.                     " << endl;
    cout << "             /   _.._       \\        PROMESA DE  " << endl;
    cout << "            /   (    )       \\        MENIQUE    " << endl;
    cout << "           (     ]..[         )         _        " << endl;
    cout << "            \\   '    '       /         | |       " << endl;
    cout << "             \\              /       _  | |       " << endl;
    cout << "              '.._      _..'       | |_| |       " << endl;
    cout << "                  |____|           |     |       " << endl;
    cout << "                                   |_____|       " << endl;
    cout << "==================================================" << endl;
    setColor(14);
    cout << "        PRESIONE ENTER PARA CONTINUAR...         " << endl;
    cout << "==================================================" << endl;
    setColor(7);
    cin.ignore();
    cin.get();
}

void imprimirTicket(string cliente, int idVenta, string nombrePromesa,
    float precio, float multa) {
    time_t t = time(0);
    char* fecha = ctime(&t);
    system("cls");
    setColor(15);
    cout << "**************************************************" << endl;
    cout << "       TICKET: LA TABERNA DE LAS PROMESAS        " << endl;
    cout << "**************************************************" << endl;
    cout << " No. Certificado : " << idVenta << endl;
    cout << " Fecha           : " << fecha;
    cout << " Cliente         : " << cliente << endl;
    cout << " Promesa         : " << nombrePromesa << endl;
    cout << "--------------------------------------------------" << endl;
    cout << " Subtotal        : Q" << precio << endl;
    cout << " IVA (12%)       : Q" << precio * 0.12f << endl;
    cout << " TOTAL A PAGAR   : Q" << precio * 1.12f << endl;
    cout << "--------------------------------------------------" << endl;
    cout << " Multa si rompe  : Q" << multa << endl;
    cout << "**************************************************" << endl;
    cout << " Antonio | Waldemar | Luis | Oscar               " << endl;
    cout << " Si la rompe... le cobramos. Prometido.          " << endl;
    cout << "**************************************************" << endl;
    system("pause");
}

void menuUsuario(string nombreU) {
    string opcs[] = {
        "Ver catalogo de promesas",
        "Certificar una promesa (genera ticket)",
        "Ver mis promesas certificadas",
        "Actualizar estado de mi promesa",
        "Cambiar mi contrasena",
        "Cerrar sesion"
    };
    int sel;
    do {
        sel = menuFlechas("BIENVENIDO/A: " + nombreU, opcs, 6, 14, 7, 11);

        if (sel == 0) {
            system("cls");
            setColor(14);
            cout << "==================================================" << endl;
            cout << "            CATALOGO DE PROMESAS                  " << endl;
            cout << "==================================================" << endl;
            mostrarPromesas();
            cout << "==================================================" << endl;
            system("pause");
        }
        else if (sel == 1) {
            system("cls");
            setColor(14);
            cout << "==================================================" << endl;
            cout << "           CERTIFICAR PROMESA                     " << endl;
            cout << "==================================================" << endl;
            mostrarPromesas();
            cout << "--------------------------------------------------" << endl;
            mostrarCursor();
            int idSel; cout << " Seleccione ID de promesa: "; cin >> idSel;
            NodoPromesa* prom = buscarPromesaId(idSel);
            if (prom == nullptr) {
                setColor(12); cout << " ID no encontrado." << endl; system("pause");
            }
            else {
                time_t t = time(0);
                char* fecha = ctime(&t);
                string fechaStr = string(fecha);
                if (!fechaStr.empty()) fechaStr.pop_back();
                for (char& c : fechaStr) if (c == ' ') c = '_';
                int nuevoId = contadorVentas + 1;
                insertarVenta(nuevoId, nombreU, prom->id, prom->nombre,
                    prom->precio * 1.12f, "Activa", fechaStr);
                guardarVentas();
                imprimirTicket(nombreU, nuevoId, prom->nombre, prom->precio, prom->multa);
            }
        }
        else if (sel == 2) {
            system("cls");
            setColor(14);
            cout << "==================================================" << endl;
            cout << "         MIS PROMESAS CERTIFICADAS               " << endl;
            cout << "==================================================" << endl;
            NodoVenta* temp = listaVentas;
            int encontradas = 0;
            while (temp != nullptr) {
                if (temp->cliente == nombreU) {
                    cout << " #" << temp->id << " | " << temp->nombrePromesa
                        << " | Q" << temp->total << " | Estado: " << temp->estado << endl;
                    encontradas++;
                }
                temp = temp->siguiente;
            }
            if (encontradas == 0) cout << " No tienes promesas certificadas aun." << endl;
            cout << "==================================================" << endl;
            system("pause");
        }
        else if (sel == 3) {
            system("cls");
            setColor(14);
            cout << "==================================================" << endl;
            cout << "         ACTUALIZAR ESTADO DE PROMESA            " << endl;
            cout << "==================================================" << endl;
            NodoVenta* temp = listaVentas;
            int encontradas = 0;
            while (temp != nullptr) {
                if (temp->cliente == nombreU) {
                    cout << " #" << temp->id << " | " << temp->nombrePromesa
                        << " | Estado: " << temp->estado << endl;
                    encontradas++;
                }
                temp = temp->siguiente;
            }
            if (encontradas == 0) {
                cout << " No tienes promesas certificadas." << endl; system("pause");
            }
            else {
                mostrarCursor();
                int idSel;
                cout << "--------------------------------------------------" << endl;
                cout << " Ingrese el # de la promesa a actualizar: "; cin >> idSel;
                NodoVenta* nodo = buscarVentaId(idSel);
                if (nodo == nullptr || nodo->cliente != nombreU) {
                    setColor(12); cout << " Promesa no encontrada." << endl;
                }
                else {
                    string opcsE[] = { "Activa", "Cumplida", "Rota", "Cancelar" };
                    int estSel = menuFlechas("NUEVO ESTADO", opcsE, 4, 14, 7, 10);
                    if (estSel == 0) nodo->estado = "Activa";
                    else if (estSel == 1) nodo->estado = "Cumplida";
                    else if (estSel == 2) nodo->estado = "Rota";
                    if (estSel != 3) {
                        guardarVentas();
                        setColor(10); cout << " Estado actualizado a: " << nodo->estado << endl;
                        system("pause");
                    }
                    return;
                }
                system("pause");
            }
        }
        else if (sel == 4) {
            mostrarCursor();
            string nueva; cout << " Ingrese nueva contrasena: "; cin >> nueva;
            NodoUsuario* nodo = buscarUsuario(nombreU);
            if (nodo != nullptr) {
                nodo->password = nueva;
                guardarUsuarios();
                setColor(10); cout << " Contrasena actualizada exitosamente!" << endl;
            }
            system("pause");
        }

    } while (sel != 5);
}

void menuAdministrador() {
    string opcs[] = {
        "Ver catalogo de promesas",
        "Agregar promesa al catalogo",
        "Editar promesa del catalogo",
        "Eliminar promesa del catalogo",
        "Ver usuarios registrados",
        "Agregar usuario",
        "Editar contrasena de usuario",
        "Eliminar usuario",
        "Ver reporte general de ventas",
        "Salir al menu principal"
    };
    int sel;
    do {
        sel = menuFlechas("PANEL DE CONTROL: ADMINISTRADOR", opcs, 10, 12, 7, 14);

        if (sel == 0) {
            system("cls"); setColor(12);
            cout << "==================================================" << endl;
            cout << "            CATALOGO DE PROMESAS                  " << endl;
            cout << "==================================================" << endl;
            mostrarPromesas();
            cout << "==================================================" << endl;
            system("pause");
        }
        else if (sel == 1) {
            system("cls"); setColor(12);
            cout << "--- AGREGAR PROMESA ---" << endl;
            setColor(7); mostrarCursor();
            string nombre; float precio, multa;
            cout << " Nombre (sin espacios, usa_guiones): "; cin >> nombre;
            cout << " Precio Q: "; cin >> precio;
            cout << " Multa Q si rompe la promesa: "; cin >> multa;
            insertarPromesa(contadorPromesas + 1, nombre, precio, multa);
            guardarPromesas();
            setColor(10); cout << " Promesa agregada correctamente!" << endl;
            system("pause");
        }
        else if (sel == 2) {
            system("cls"); setColor(12);
            cout << "--- EDITAR PROMESA ---" << endl;
            mostrarPromesas();
            setColor(7); mostrarCursor();
            int idSel; cout << " ID a editar: "; cin >> idSel;
            NodoPromesa* nodo = buscarPromesaId(idSel);
            if (nodo == nullptr) {
                setColor(12); cout << " ID no encontrado." << endl;
            }
            else {
                cout << " Nuevo nombre (sin espacios): "; cin >> nodo->nombre;
                cout << " Nuevo precio Q: ";             cin >> nodo->precio;
                cout << " Nueva multa Q: ";              cin >> nodo->multa;
                guardarPromesas();
                setColor(10); cout << " Promesa actualizada!" << endl;
            }
            system("pause");
        }
        else if (sel == 3) {
            system("cls"); setColor(12);
            cout << "--- ELIMINAR PROMESA ---" << endl;
            mostrarPromesas();
            setColor(7); mostrarCursor();
            int idSel; cout << " ID a eliminar: "; cin >> idSel;
            if (eliminarPromesa(idSel)) {
                guardarPromesas();
                setColor(10); cout << " Promesa eliminada correctamente!" << endl;
            }
            else {
                setColor(12); cout << " ID no encontrado." << endl;
            }
            system("pause");
        }
        else if (sel == 4) {
            system("cls"); setColor(12);
            cout << "--- USUARIOS REGISTRADOS ---" << endl;
            mostrarUsuarios();
            system("pause");
        }
        else if (sel == 5) {
            system("cls"); setColor(12);
            cout << "--- AGREGAR USUARIO ---" << endl;
            setColor(7); mostrarCursor();
            string usr, pwd;
            cout << " Nuevo username  : "; cin >> usr;
            if (buscarUsuario(usr) != nullptr) {
                setColor(12); cout << " Ese usuario ya existe." << endl;
            }
            else {
                cout << " Nueva contrasena: "; cin >> pwd;
                insertarUsuario(usr, pwd);
                guardarUsuarios();
                setColor(10); cout << " Usuario creado exitosamente!" << endl;
            }
            system("pause");
        }
        else if (sel == 6) {
            system("cls"); setColor(12);
            cout << "--- EDITAR CONTRASENA DE USUARIO ---" << endl;
            mostrarUsuarios();
            setColor(7); mostrarCursor();
            string usr; cout << " Username a editar: "; cin >> usr;
            NodoUsuario* nodo = buscarUsuario(usr);
            if (nodo == nullptr) {
                setColor(12); cout << " Usuario no encontrado." << endl;
            }
            else {
                cout << " Nueva contrasena: "; cin >> nodo->password;
                guardarUsuarios();
                setColor(10); cout << " Usuario actualizado!" << endl;
            }
            system("pause");
        }
        else if (sel == 7) {
            system("cls"); setColor(12);
            cout << "--- ELIMINAR USUARIO ---" << endl;
            mostrarUsuarios();
            setColor(7); mostrarCursor();
            string usr; cout << " Username a eliminar: "; cin >> usr;
            if (eliminarUsuario(usr)) {
                guardarUsuarios();
                setColor(10); cout << " Usuario eliminado correctamente!" << endl;
            }
            else {
                setColor(12); cout << " Usuario no encontrado." << endl;
            }
            system("pause");
        }
        else if (sel == 8) {
            system("cls"); setColor(12);
            float totalCaja = 0;
            int activas = 0, cumplidas = 0, rotas = 0;
            cout << "==================================================" << endl;
            cout << "          REPORTE GENERAL DE VENTAS               " << endl;
            cout << "==================================================" << endl;
            cout << " # | Cliente       | Promesa            | Total  | Estado" << endl;
            cout << "--------------------------------------------------" << endl;
            NodoVenta* temp = listaVentas;
            while (temp != nullptr) {
                cout << " " << temp->id << " | " << temp->cliente
                    << "\t| " << temp->nombrePromesa
                    << "\t| Q" << temp->total
                    << " | " << temp->estado << endl;
                totalCaja += temp->total;
                if (temp->estado == "Activa")        activas++;
                else if (temp->estado == "Cumplida") cumplidas++;
                else if (temp->estado == "Rota")     rotas++;
                temp = temp->siguiente;
            }
            cout << "==================================================" << endl;
            cout << " Total de certificaciones : " << contadorVentas << endl;
            cout << " Activas                  : " << activas << endl;
            cout << " Cumplidas                : " << cumplidas << endl;
            cout << " Rotas                    : " << rotas << endl;
            cout << "--------------------------------------------------" << endl;
            cout << " TOTAL RECAUDADO EN CAJA  : Q" << totalCaja << endl;
            cout << "==================================================" << endl;
            system("pause");
        }

    } while (sel != 9);
}

int main() {
    cargarUsuarios();
    inicializarPromesas();
    cargarVentas();

    if (listaUsuarios == nullptr) {
        insertarUsuario("admin", "admin123");
        guardarUsuarios();
    }

    pantallaInicio();

    string opcsMain[] = {
        "Iniciar Sesion",
        "Registrarse (Nuevo Usuario)",
        "Modo Administrador",
        "Salir"
    };

    int sel;
    do {
        sel = menuFlechas("SISTEMA DE PROMESAS DE BORRACHERA", opcsMain, 4, 11, 7, 14);

        if (sel == 0) {
            system("cls"); setColor(11);
            cout << "--- INICIAR SESION ---" << endl;
            setColor(7); mostrarCursor();
            string u, p;
            bool loginOk = false;
            int intentos = 3;
            while (intentos > 0 && !loginOk) {
                cout << " Usuario   : "; cin >> u;
                cout << " Contrasena: "; cin >> p;
                if (loginUsuario(u, p)) {
                    loginOk = true;
                }
                else {
                    intentos--;
                    setColor(12);
                    cout << " Credenciales incorrectas. Intentos restantes: " << intentos << endl;
                    setColor(7);
                    system("pause");
                }
            }
            if (loginOk) menuUsuario(u);
            else { setColor(12); cout << " Demasiados intentos fallidos. Acceso bloqueado." << endl; system("pause"); }
        }
        else if (sel == 1) {
            system("cls"); setColor(11);
            mostrarCursor();
            string u, p;
            cout << " Crea tu usuario   : "; cin >> u;
            if (buscarUsuario(u) != nullptr) {
                setColor(12); cout << " Ese usuario ya existe." << endl;
            }
            else {
                cout << " Crea tu contrasena: "; cin >> p;
                insertarUsuario(u, p);
                guardarUsuarios();
                setColor(10); cout << " Registro exitoso!" << endl;
            }
            system("pause");
        }
        else if (sel == 2) {
            system("cls"); setColor(12);
            mostrarCursor();
            string claveAdmin;
            cout << " Clave de administrador: "; cin >> claveAdmin;
            if (claveAdmin == "admin123") {
                menuAdministrador();
            }
            else {
                setColor(12); cout << " Clave incorrecta. Acceso denegado." << endl;
                system("pause");
            }
        }

    } while (sel != 3);

    while (listaPromesas != nullptr) {
        NodoPromesa* t = listaPromesas;
        listaPromesas = listaPromesas->siguiente;
        delete t;
    }
    while (listaVentas != nullptr) {
        NodoVenta* t = listaVentas;
        listaVentas = listaVentas->siguiente;
        delete t;
    }
    if (listaUsuarios != nullptr) {
        NodoUsuario* ultimo = listaUsuarios;
        while (ultimo->siguiente != listaUsuarios) ultimo = ultimo->siguiente;
        ultimo->siguiente = nullptr;
        NodoUsuario* temp = listaUsuarios;
        while (temp != nullptr) { NodoUsuario* t = temp; temp = temp->siguiente; delete t; }
    }

    setColor(7);
    cout << "\n Hasta luego! No olvide pagar sus promesas." << endl;
    return 0;
}
