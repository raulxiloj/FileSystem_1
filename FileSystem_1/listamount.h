#ifndef LISTAMOUNT_H
#define LISTAMOUNT_H

#include "nodomount.h"

class ListaMount
{
public:
    NodoMount *raiz;
    ListaMount();
    void insertarNodo(NodoMount*);
};

#endif // LISTAMOUNT_H
