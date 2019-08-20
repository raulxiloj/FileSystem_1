#ifndef LISTAMOUNT_H
#define LISTAMOUNT_H

#include "nodomount.h"

class ListaMount
{
public:
    NodoMount *primero;
    ListaMount();
    int buscarLetra(QString direccion, QString nombre);
    int buscarNumero(QString direccion, QString nombre);
    void insertarNodo(NodoMount*);
    int eliminarNodo(QString);
    bool buscarNodo(QString, QString);
    QString getDireccion(QString);
};

#endif // LISTAMOUNT_H
