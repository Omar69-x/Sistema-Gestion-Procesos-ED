#include <iostream>
#include <windows.h>
#include <iomanip>
#include <cstring>
#include <fstream>
using namespace std;

// COLORES (igual que en clase)
void color(int c) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

// ESTRUCTURA PRINCIPAL
struct Proceso {
    int id;
    char nombre[50];
    int prioridad; // 1 = Alta, 5 = Baja
    int memoria;   // En MB
};

// NODOS PARA CADA ESTRUCTURA
struct NodoLista {
    Proceso dato;
    NodoLista *siguiente;

    NodoLista(Proceso p) {
        dato = p;
        siguiente = NULL;
    }
};

struct NodoCola {
    Proceso dato;
    NodoCola *siguiente;

    NodoCola(Proceso p) {
        dato = p;
        siguiente = NULL;
    }
};

struct NodoPila {
    Proceso dato;
    NodoPila *siguiente;

    NodoPila(Proceso p) {
        dato = p;
        siguiente = NULL;
    }
};

// PUNTEROS GLOBALES
NodoLista *inicio = NULL;
NodoCola *frente = NULL;
NodoCola *finalCola = NULL;
NodoPila *tope = NULL;

// ==========================================
// 1. GESTOR DE PROCESOS (LISTA ENLAZADA)
// ==========================================
void insertarProceso() {
    Proceso p;

    cout << "\n  ID del proceso: ";
    cin >> p.id;

    // Validar que no se repita el ID
    NodoLista *aux = inicio;
    while (aux != NULL) {
        if (aux->dato.id == p.id) {
            color(12);
            cout << "\n  ERROR: Ese ID ya existe.\n";
            color(7);
            return;
        }
        aux = aux->siguiente;
    }

    cout << "  Nombre (sin espacios, ej. Chrome): ";
    cin >> p.nombre;

    cout << "  Prioridad (1-Alta a 5-Baja): ";
    cin >> p.prioridad;

    cout << "  Memoria (MB): ";
    cin >> p.memoria;

    NodoLista *nuevo = new NodoLista(p);

    if (inicio == NULL) {
        inicio = nuevo;
    } else {
        aux = inicio;
        while (aux->siguiente != NULL) {
            aux = aux->siguiente;
        }
        aux->siguiente = nuevo;
    }

    color(10);
    cout << "\n  PROCESO REGISTRADO CORRECTAMENTE\n";
    color(7);
}

void eliminarProceso() {
    if (inicio == NULL) {
        color(12);
        cout << "\n  LA LISTA ESTA VACIA\n";
        color(7);
        return;
    }

    int id;
    cout << "\n  Ingrese ID a eliminar: ";
    cin >> id;

    NodoLista *aux = inicio;
    NodoLista *anterior = NULL;

    while (aux != NULL && aux->dato.id != id) {
        anterior = aux;
        aux = aux->siguiente;
    }

    if (aux == NULL) {
        color(12);
        cout << "\n  PROCESO NO ENCONTRADO\n";
        color(7);
        return;
    }

    if (anterior == NULL) {
        inicio = aux->siguiente;
    } else {
        anterior->siguiente = aux->siguiente;
    }

    delete aux;
    color(10);
    cout << "\n  PROCESO ELIMINADO CORRECTAMENTE\n";
    color(7);
}

void buscarProceso() {
    if (inicio == NULL) {
        color(12);
        cout << "\n  LA LISTA ESTA VACIA\n";
        color(7);
        return;
    }

    int op;
    cout << "\n  1. Buscar por ID\n";
    cout << "  2. Buscar por Nombre\n";
    cout << "  Opcion: ";
    cin >> op;

    NodoLista *aux = inicio;
    bool encontrado = false;

    if (op == 1) {
        int id;
        cout << "  Ingrese ID: ";
        cin >> id;

        while (aux != NULL) {
            if (aux->dato.id == id) {
                color(10);
                cout << "\n  ENCONTRADO -> ID: " << aux->dato.id
                     << " | Nombre: " << aux->dato.nombre
                     << " | Prioridad: " << aux->dato.prioridad
                     << " | Memoria: " << aux->dato.memoria << " MB\n";
                color(7);
                encontrado = true;
                break;
            }
            aux = aux->siguiente;
        }
    } else if (op == 2) {
        char nombreBuscar[50];
        cout << "  Ingrese Nombre: ";
        cin >> nombreBuscar;

        while (aux != NULL) {
            if (strcmp(aux->dato.nombre, nombreBuscar) == 0) {
                color(10);
                cout << "\n  ENCONTRADO -> ID: " << aux->dato.id
                     << " | Nombre: " << aux->dato.nombre
                     << " | Prioridad: " << aux->dato.prioridad
                     << " | Memoria: " << aux->dato.memoria << " MB\n";
                color(7);
                encontrado = true;
                break;
            }
            aux = aux->siguiente;
        }
    }

    if (!encontrado) {
        color(12);
        cout << "\n  PROCESO NO ENCONTRADO\n";
        color(7);
    }
}

void modificarPrioridad() {
    if (inicio == NULL) {
        color(12);
        cout << "\n  LA LISTA ESTA VACIA\n";
        color(7);
        return;
    }

    int id;
    cout << "\n  Ingrese ID del proceso: ";
    cin >> id;

    NodoLista *aux = inicio;
    while (aux != NULL) {
        if (aux->dato.id == id) {
            cout << "  Prioridad actual: " << aux->dato.prioridad << "\n";
            cout << "  Nueva prioridad (1-5): ";
            cin >> aux->dato.prioridad;
            color(10);
            cout << "\n  PRIORIDAD ACTUALIZADA\n";
            color(7);
            return;
        }
        aux = aux->siguiente;
    }

    color(12);
    cout << "\n  PROCESO NO ENCONTRADO\n";
    color(7);
}

void mostrarLista() {
    NodoLista *aux = inicio;

    color(11);
    cout << "\n  ================================================\n";
    cout << "             LISTA GENERAL DE PROCESOS\n";
    cout << "  ================================================\n";

    if (inicio == NULL) {
        color(12);
        cout << "\n  LISTA VACIA\n";
        color(7);
        return;
    }

    cout << left;
    cout << "  " << setw(8) << "ID" << setw(18) << "NOMBRE"
         << setw(12) << "PRIORIDAD" << "MEMORIA\n";
    cout << "  ------------------------------------------------\n";

    while (aux != NULL) {
        color(14);
        cout << "  " << setw(8) << aux->dato.id
             << setw(18) << aux->dato.nombre
             << setw(12) << aux->dato.prioridad
             << aux->dato.memoria << " MB\n";
        aux = aux->siguiente;
    }
    color(7);
}

// ==========================================
// 2. PLANIFICADOR DE CPU (COLA DE PRIORIDAD)
// ==========================================
void enqueueCPU() {
    if (inicio == NULL) {
        color(12);
        cout << "\n  PRIMERO REGISTRE PROCESOS EN LA LISTA\n";
        color(7);
        return;
    }

    int id;
    cout << "\n  Ingrese ID del proceso a enviar a CPU: ";
    cin >> id;

    // Buscar el proceso en la lista general
    NodoLista *auxLista = inicio;
    while (auxLista != NULL && auxLista->dato.id != id) {
        auxLista = auxLista->siguiente;
    }

    if (auxLista == NULL) {
        color(12);
        cout << "\n  ERROR: ESE PROCESO NO EXISTE EN LA LISTA\n";
        color(7);
        return;
    }

    NodoCola *nuevo = new NodoCola(auxLista->dato);

    // Encolar ordenado por prioridad (menor numero = mas prioridad)
    if (frente == NULL || nuevo->dato.prioridad < frente->dato.prioridad) {
        nuevo->siguiente = frente;
        frente = nuevo;
        if (finalCola == NULL) {
            finalCola = nuevo;
        }
    } else {
        NodoCola *aux = frente;
        while (aux->siguiente != NULL && aux->siguiente->dato.prioridad <= nuevo->dato.prioridad) {
            aux = aux->siguiente;
        }
        nuevo->siguiente = aux->siguiente;
        aux->siguiente = nuevo;
        if (nuevo->siguiente == NULL) {
            finalCola = nuevo;
        }
    }

    color(10);
    cout << "\n  PROCESO ENCOLADO EN CPU\n";
    color(7);
}

void dequeueCPU() {
    if (frente == NULL) {
        color(12);
        cout << "\n  COLA DE CPU VACIA\n";
        color(7);
        return;
    }

    NodoCola *aux = frente;
    color(14);
    cout << "\n  EJECUTANDO PROCESO...\n";
    cout << "  ID: " << aux->dato.id << "\n";
    cout << "  Nombre: " << aux->dato.nombre << "\n";
    cout << "  Prioridad: " << aux->dato.prioridad << "\n";

    frente = frente->siguiente;
    if (frente == NULL) {
        finalCola = NULL;
    }

    delete aux;
    color(10);
    cout << "\n  EJECUCION FINALIZADA\n";
    color(7);
}

void mostrarColaCPU() {
    NodoCola *aux = frente;

    color(11);
    cout << "\n  ================================================\n";
    cout << "              COLA DE PLANIFICACION CPU\n";
    cout << "  ================================================\n";

    if (frente == NULL) {
        color(12);
        cout << "\n  COLA VACIA\n";
        color(7);
        return;
    }

    cout << left;
    cout << "  " << setw(8) << "ID" << setw(18) << "NOMBRE"
         << setw(12) << "PRIORIDAD" << "MEMORIA\n";
    cout << "  ------------------------------------------------\n";

    while (aux != NULL) {
        color(14);
        cout << "  " << setw(8) << aux->dato.id
             << setw(18) << aux->dato.nombre
             << setw(12) << aux->dato.prioridad
             << aux->dato.memoria << " MB\n";
        aux = aux->siguiente;
    }
    color(7);
}

// ==========================================
// 3. GESTOR DE MEMORIA (PILA)
// ==========================================
void pushMemoria() {
    if (inicio == NULL) {
        color(12);
        cout << "\n  PRIMERO REGISTRE PROCESOS EN LA LISTA\n";
        color(7);
        return;
    }

    int id;
    cout << "\n  Ingrese ID del proceso para asignar memoria: ";
    cin >> id;

    NodoLista *auxLista = inicio;
    while (auxLista != NULL && auxLista->dato.id != id) {
        auxLista = auxLista->siguiente;
    }

    if (auxLista == NULL) {
        color(12);
        cout << "\n  ERROR: PROCESO NO ENCONTRADO\n";
        color(7);
        return;
    }

    NodoPila *nuevo = new NodoPila(auxLista->dato);
    nuevo->siguiente = tope;
    tope = nuevo;

    color(10);
    cout << "\n  MEMORIA ASIGNADA CORRECTAMENTE (PUSH)\n";
    color(7);
}

void popMemoria() {
    if (tope == NULL) {
        color(12);
        cout << "\n  PILA DE MEMORIA VACIA\n";
        color(7);
        return;
    }

    NodoPila *aux = tope;
    color(14);
    cout << "\n  MEMORIA LIBERADA (POP)\n";
    cout << "  Proceso: " << aux->dato.nombre << " (" << aux->dato.memoria << " MB)\n";

    tope = tope->siguiente;
    delete aux;
    color(7);
}

void mostrarMemoria() {
    NodoPila *aux = tope;

    color(11);
    cout << "\n  ===== PILA DE MEMORIA =====\n";

    if (tope == NULL) {
        color(12);
        cout << "  MEMORIA LIBRE (VACIA)\n";
        color(7);
        return;
    }

    int total = 0;
    while (aux != NULL) {
        color(10);
        cout << "  | ID: " << aux->dato.id << " - " << aux->dato.nombre
             << " (" << aux->dato.memoria << " MB) |\n";
        cout << "  -------------------------------\n";
        total = total + aux->dato.memoria;
        aux = aux->siguiente;
    }

    color(14);
    cout << "  NULL\n";
    cout << "  Total usado: " << total << " MB\n";
    color(7);
}

// ==========================================
// 4. GUARDAR Y CARGAR (PERSISTENCIA TXT)
// ==========================================
void guardarArchivo() {
    ofstream archivo("procesos.txt");
    NodoLista *aux = inicio;

    while (aux != NULL) {
        archivo << aux->dato.id << " "
                << aux->dato.nombre << " "
                << aux->dato.prioridad << " "
                << aux->dato.memoria << "\n";
        aux = aux->siguiente;
    }

    archivo.close();
    color(10);
    cout << "\n  DATOS GUARDADOS EN procesos.txt\n";
    color(7);
}

void cargarArchivo() {
    ifstream archivo("procesos.txt");
    if (!archivo) {
        color(12);
        cout << "\n  NO EXISTE ARCHIVO procesos.txt TODAVIA\n";
        color(7);
        return;
    }

    Proceso p;
    while (archivo >> p.id >> p.nombre >> p.prioridad >> p.memoria) {
        NodoLista *nuevo = new NodoLista(p);
        if (inicio == NULL) {
            inicio = nuevo;
        } else {
            NodoLista *aux = inicio;
            while (aux->siguiente != NULL) {
                aux = aux->siguiente;
            }
            aux->siguiente = nuevo;
        }
    }

    archivo.close();
    color(10);
    cout << "\n  DATOS CARGADOS DESDE procesos.txt\n";
    color(7);
}

// ==========================================
// LIBERAR MEMORIA AL SALIR
// ==========================================
void liberarTodo() {
    while (inicio != NULL) {
        NodoLista *aux = inicio;
        inicio = inicio->siguiente;
        delete aux;
    }
    while (frente != NULL) {
        NodoCola *aux = frente;
        frente = frente->siguiente;
        delete aux;
    }
    while (tope != NULL) {
        NodoPila *aux = tope;
        tope = tope->siguiente;
        delete aux;
    }
}

// ==========================================
// MENU PRINCIPAL
// ==========================================
void menu() {
    color(11);
    cout << "\n";
    cout << "  +===========================================+\n";
    cout << "  |      SISTEMA DE GESTION DE PROCESOS       |\n";
    cout << "  +===========================================+\n";
    color(14);
    cout << "  |  --- GESTOR DE PROCESOS (LISTA) ---       |\n";
    cout << "  |  1. Insertar proceso                      |\n";
    cout << "  |  2. Eliminar proceso                      |\n";
    cout << "  |  3. Buscar proceso (ID o Nombre)          |\n";
    cout << "  |  4. Modificar prioridad                   |\n";
    cout << "  |  5. Mostrar lista de procesos             |\n";
    cout << "  |  --- PLANIFICADOR CPU (COLA) ---          |\n";
    cout << "  |  6. Encolar proceso (ENQUEUE)             |\n";
    cout << "  |  7. Ejecutar proceso (DEQUEUE)            |\n";
    cout << "  |  8. Mostrar cola de CPU                   |\n";
    cout << "  |  --- GESTOR DE MEMORIA (PILA) ---         |\n";
    cout << "  |  9. Asignar memoria (PUSH)                |\n";
    cout << "  | 10. Liberar memoria (POP)                 |\n";
    cout << "  | 11. Mostrar pila de memoria               |\n";
    cout << "  |  --- ARCHIVOS ---                         |\n";
    cout << "  | 12. Guardar datos (.txt)                  |\n";
    cout << "  | 13. Cargar datos (.txt)                   |\n";
    cout << "  | 14. Salir                                 |\n";
    color(11);
    cout << "  +===========================================+\n";
    color(15);
    cout << "\n  Seleccione una opcion: ";
}

int main() {
    int opcion;

    do {
        system("cls");
        menu();

        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(10000, '\n');
            opcion = 0;
        }

        switch (opcion) {
            case 1: insertarProceso(); break;
            case 2: eliminarProceso(); break;
            case 3: buscarProceso(); break;
            case 4: modificarPrioridad(); break;
            case 5: mostrarLista(); break;
            case 6: enqueueCPU(); break;
            case 7: dequeueCPU(); break;
            case 8: mostrarColaCPU(); break;
            case 9: pushMemoria(); break;
            case 10: popMemoria(); break;
            case 11: mostrarMemoria(); break;
            case 12: guardarArchivo(); break;
            case 13: cargarArchivo(); break;
            case 14:
                liberarTodo();
                color(10);
                cout << "\n  MEMORIA LIBERADA. HASTA PRONTO!\n";
                break;
            default:
                color(12);
                cout << "\n  OPCION INCORRECTA\n";
        }

        color(7);
        if (opcion != 14) {
            cout << "\n  Presione ENTER para continuar...";
            cin.ignore(10000, '\n');
            cin.get();
        }

    } while (opcion != 14);

    return 0;
}