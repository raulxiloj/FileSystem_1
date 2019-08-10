#include <QCoreApplication>
#include <iostream>

#include "scanner.h"
#include "parser.h"
#include "graficador.h"

extern int yyparse();
extern Nodo *raiz; // Raiz del arbol

using namespace std;

int main()
{
    //char string[256] = {0};
    char input[256];
    printf("Please write a command: ");

    fgets(input,sizeof (input),stdin);
    YY_BUFFER_STATE buffer = yy_scan_string(input);
    if(yyparse() == 0){
        Graficador *g = new Graficador(raiz);
        g->generarImagen();
        cout << "Cadena correcta" << endl;
    }else{
        cout << "Cadena incorrecta" << endl;
    }
    return 0;
}
