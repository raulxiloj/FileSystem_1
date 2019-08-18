#include "listamount.h"

ListaMount::ListaMount(){
    primero = nullptr;
}

/* Metodo para insertar al final de la lista
 * @param NodoMount nuevo: El nodo a insertar
*/
void ListaMount::insertarNodo(NodoMount *nuevo){
    NodoMount *aux = primero;
    while(aux->siguiente!=nullptr){
        aux = aux->siguiente;
    }
    aux->siguiente = nuevo;
}

/* Funcion para verificar que letra asignarle al id de un nodo
 * @param QString direccion: Ruta del disco
 * @param QString nombre: nombre de la particion
 * @return -1 = ya esta montada | # = letra
*/
int ListaMount::buscarLetra(QString direccion, QString nombre){
    NodoMount *aux = primero;
    int retorno = 'a';
    while(aux!=nullptr){
        if((direccion == aux->direccion) && (nombre == aux->nombre)){
            return -1;
        }else{
            if(direccion == aux->direccion){
                return aux->letra;
            }else if(retorno <= aux->letra){
                retorno++;
            }
        }
        aux = aux->siguiente;
    }
    return retorno;
}

/* Funcion para verificar que numero asignarle al id de un nodo
 * @param QString direccion: Ruta del disco
 * @param QString nombre: nombre de la particion
 * @return retorno = numero a asignar
*/
int ListaMount::buscarNumero(QString direccion, QString nombre){
    int retorno = 1;
    NodoMount *aux = primero;
    while(aux!=nullptr){
        if((direccion == aux->direccion) && (retorno == aux->num)){
            retorno++;
        }
        aux = aux->siguiente;
    }
    return retorno;
}
