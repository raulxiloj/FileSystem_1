#include "listamount.h"

ListaMount::ListaMount(){
    raiz = nullptr;
}

void ListaMount::insertarNodo(NodoMount *nuevo){
    raiz = nuevo;
}
