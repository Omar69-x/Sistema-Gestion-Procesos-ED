#include <iostream>
#include <windows.h>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

void color(int c) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

// ==========================================
// ESTRUCTURA PRINCIPAL DE PROCESO
// ==========================================
struct Proceso {
    int id;
    char nombre[50];
    int prioridad; // 1 (Alta) a 5 (Baja)
    int memoria;   // MB
};

// ==========================================
// NODOS PARA LAS TRES ESTRUCTURAS
// ==========================================
struct NodoLista {
    Proceso dato;
    NodoLista *siguiente;
    NodoLista(Proceso p) { dato = p; siguiente = NULL; }
};

struct NodoCola {
    Proceso dato;
    NodoCola *siguiente;
    NodoCola(Proceso p) { dato = p; siguiente = NULL; }
};

struct NodoPila {
    Proceso dato;
    NodoPila *siguiente;
    NodoPila(Proceso p) { dato = p; siguiente = NULL; }
};

// Punteros globales
NodoLista *inicioLista = NULL;
NodoCola *frenteCPU = NULL, *finalCPU = NULL;
NodoPila *topeMemoria = NULL;

// ==========================================
// 1. GESTOR DE MEMORIA (PILA)
// ==========================================
void apilarMemoria(Proceso p) {
    NodoPila *nuevo = new NodoPila(p);
    nuevo->siguiente = topeMemoria;
    topeMemoria = nuevo;
}

void desapilarMemoria() {
    if (topeMemoria != NULL) {
        NodoPila *aux = topeMemoria;
        topeMemoria = topeMemoria->siguiente;
        delete aux;
    }
}

void mostrarMemoria() {
    NodoPila *aux = topeMemoria;
    color(11); cout << "\n--- ESTADO DE LA MEMORIA (PILA) ---\n"; color(7);
    if (topeMemoria == NULL) {
        color(12); cout << "Memoria vacia.\n"; color(7);
        return;
    }
    int totalUsado = 0;
    while (aux != NULL) {
        color(10);
        cout << "| Proceso ID: " << aux->dato.id << " -> " << aux->dato.memoria << " MB |\n";
        cout << "---------------------------------\n";
        totalUsado += aux->dato.memoria;
        aux = aux->siguiente;
    }
    color(14); cout << "Total Memoria en uso: " << totalUsado << " MB\n"; color(7);
}

// ==========================================
// 2. PLANIFICADOR DE CPU (COLA DE PRIORIDAD)
// ==========================================
void encolarCPU(Proceso p) {
    NodoCola *nuevo = new NodoCola(p);
    
    // Insercion por prioridad (menor numero = mayor prioridad)
    if (frenteCPU == NULL || p.prioridad < frenteCPU->dato.prioridad) {
        nuevo->siguiente = frenteCPU;
        frenteCPU = nuevo;
        if (finalCPU == NULL) finalCPU = nuevo;
    } else {
        NodoCola *aux = frenteCPU;
        while (aux->siguiente != NULL && aux->siguiente->dato.prioridad <= p.prioridad) {
            aux = aux->siguiente;
        }
        nuevo->siguiente = aux->siguiente;
        aux->siguiente = nuevo;
        if (nuevo->siguiente == NULL) finalCPU = nuevo;
    }
    apilarMemoria(p); // Consume memoria al entrar a CPU
}

void ejecutarProcesoCPU() {
    if (frenteCPU == NULL) {
        color(12); cout << "\nNo hay procesos en la Cola de CPU.\n"; color(7);
        return;
    }
    NodoCola *aux = frenteCPU;
    color(14);
    cout << "\nEJECUTANDO PROCESO...\n";
    cout << "ID: " << aux->dato.id << " | Nombre: " << aux->dato.nombre << " | Prioridad: " << aux->dato.prioridad << "\n";
    
    frenteCPU = frenteCPU->siguiente;
    if (frenteCPU == NULL) finalCPU = NULL;
    
    delete aux;
    desapilarMemoria(); // Libera RAM
    color(10); cout << "Proceso finalizado y memoria liberada.\n"; color(7);
}

void mostrarColaCPU() {
    NodoCola *aux = frenteCPU;
    color(11); cout << "\n--- COLA DE CPU (ORDEN DE PRIORIDAD) ---\n"; color(7);
    if (frenteCPU == NULL) {
        color(12); cout << "CPU Inactiva (Cola vacia).\n"; color(7);
        return;
    }
    cout << left << setw(10) << "ID" << setw(20) << "NOMBRE" << setw(15) << "PRIORIDAD" << "MEMORIA\n";
    cout << "------------------------------------------------------\n";
    while (aux != NULL) {
        color(14);
        cout << setw(10) << aux->dato.id << setw(20) << aux->dato.nombre 
             << setw(15) << aux->dato.prioridad << aux->dato.memoria << " MB\n";
        aux = aux->siguiente;
    }
    color(7);
}

// ==========================================
// 3. GESTOR DE PROCESOS (LISTA ENLAZADA)
// ==========================================
void registrarProceso() {
    Proceso p;
    cout << "\n--- REGISTRAR NUEVO PROCESO ---\n";
    cout << "ID del Proceso: "; cin >> p.id;
    cin.ignore();
    cout << "Nombre del Proceso: "; cin.getline(p.nombre, 50);
    cout << "Prioridad (1-Alta, 5-Baja): "; cin >> p.prioridad;
    cout << "Memoria requerida (MB): "; cin >> p.memoria;

    NodoLista *nuevo = new NodoLista(p);
    if (inicioLista == NULL) {
        inicioLista = nuevo;
    } else {
        NodoLista *aux = inicioLista;
        while (aux->siguiente != NULL) {
            aux = aux->siguiente;
        }
        aux->siguiente = nuevo;
    }
    color(10); cout << "\nProceso registrado en la Lista General.\n"; color(7);
}

void mostrarListaProcesos() {
    NodoLista *aux = inicioLista;
    color(11); cout << "\n--- LISTA GENERAL DE PROCESOS REGISTRADOS ---\n"; color(7);
    if (inicioLista == NULL) {
        color(12); cout << "No hay procesos registrados.\n"; color(7);
        return;
    }
    cout << left << setw(10) << "ID" << setw(20) << "NOMBRE" << setw(15) << "PRIORIDAD" << "MEMORIA\n";
    cout << "------------------------------------------------------\n";
    while (aux != NULL) {
        color(15);
        cout << setw(10) << aux->dato.id << setw(20) << aux->dato.nombre 
             << setw(15) << aux->dato.prioridad << aux->dato.memoria << " MB\n";
        aux = aux->siguiente;
    }
    color(7);
}

void enviarProcesoACPU() {
    int idBuscar;
    cout << "\nIngrese el ID del proceso a enviar a CPU: ";
    cin >> idBuscar;

    NodoLista *aux = inicioLista;
    while (aux != NULL) {
        if (aux->dato.id == idBuscar) {
            encolarCPU(aux->dato);
            color(10); cout << "\nProceso enviado a la Cola de CPU correctamente.\n"; color(7);
            return;
        }
        aux = aux->siguiente;
    }
    color(12); cout << "\nError: Proceso no encontrado en la Lista General.\n"; color(7);
}

// ==========================================
// 4. PERSISTENCIA DE DATOS (TXT)
// ==========================================
void guardarDatos() {
    ofstream archivo("procesos.txt");
    if(!archivo) {
        color(12); cout << "Error al abrir el archivo.\n"; color(7);
        return;
    }
    NodoLista *aux = inicioLista;
    while (aux != NULL) {
        archivo << aux->dato.id << " " << aux->dato.nombre << " " 
                << aux->dato.prioridad << " " << aux->dato.memoria << "\n";
        aux = aux->siguiente;
    }
    archivo.close();
    color(10); cout << "\nDatos guardados correctamente en 'procesos.txt'.\n"; color(7);
}

void cargarDatos() {
    ifstream archivo("procesos.txt");
    if(!archivo) {
        return; // Falla silenciosa si es la primera vez que se ejecuta
    }
    Proceso p;
    while (archivo >> p.id >> p.nombre >> p.prioridad >> p.memoria) {
        NodoLista *nuevo = new NodoLista(p);
        if (inicioLista == NULL) {
            inicioLista = nuevo;
        } else {
            NodoLista *aux = inicioLista;
            while (aux->siguiente != NULL) {
                aux = aux->siguiente;
            }
            aux->siguiente = nuevo;
        }
    }
    archivo.close();
    color(10); cout << "\nDatos cargados correctamente desde 'procesos.txt'.\n"; color(7);
}

// ==========================================
// MENU PRINCIPAL
// ==========================================
void menu() {
    int opcion;
    do {
        color(11);
        cout << "\n+==========================================+\n";
        cout << "|       SISTEMA DE GESTION DE PROCESOS     |\n";
        cout << "+==========================================+\n";
        color(14);
        cout << "| 1. Registrar Proceso (Lista General)     |\n";
        cout << "| 2. Enviar Proceso a CPU (Cola Prioridad) |\n";
        cout << "| 3. Ejecutar Proceso en CPU (Desencolar)  |\n";
        cout << "| 4. Mostrar Procesos Registrados (Lista)  |\n";
        cout << "| 5. Mostrar Cola de CPU (Cola)            |\n";
        cout << "| 6. Mostrar Estado de Memoria (Pila)      |\n";
        cout << "| 7. Guardar Sistema (Archivo TXT)         |\n";
        cout << "| 8. Salir                                 |\n";
        color(11);
        cout << "+==========================================+\n";
        color(15);
        cout << "Seleccione una opcion: ";
        
        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(10000, '\n');
            opcion = 0;
        }

        switch (opcion) {
            case 1: registrarProceso(); break;
            case 2: enviarProcesoACPU(); break;
            case 3: ejecutarProcesoCPU(); break;
            case 4: mostrarListaProcesos(); break;
            case 5: mostrarColaCPU(); break;
            case 6: mostrarMemoria(); break;
            case 7: guardarDatos(); break;
            case 8: color(10); cout << "Saliendo...\n"; color(7); break;
            default: color(12); cout << "Opcion invalida.\n"; color(7);
        }
        
        if (opcion != 8) {
            color(7); cout << "\nPresione ENTER para continuar...";
            cin.ignore(10000, '\n');
            cin.get();
            system("cls");
        }
    } while (opcion != 8);
}

int main() {
    cargarDatos();
    system("cls");
    menu();
    return 0;
}