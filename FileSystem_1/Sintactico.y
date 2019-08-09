%{
    #include "scanner.h"
    #include "nodo.h"
    #include <iostream>

    extern int yylineno; //linea actual donde se encuentra el parser (analisis lexico) lo maneja BISON
    extern int columna; //columna actual donde se encuentra el parser (analisis lexico) lo maneja BISON
    extern char *yytext; //lexema actual donde esta el parser (analisis lexico) lo maneja BISON
    extern Nodo *raiz; // Raiz del arbol

    int yyerror(const char* mens){
        std::cout<<mens<<std::endl;
        return 0;
    }

%}

%error-verbose

%union
    {
        char text[256];
        class Nodo *nodito;
    }

/*--------------Terminals-------------*/
%token <text> mkdisk
%token <text> rmdisk
%token <text> fdisk
%token <text> mount
%token <text> unmount
%token <text> rep
%token <text> exec
%token <text> size
%token <text> unit
%token <text> path
%token <text> fit
%token <text> name
%token <text> type
%token <text> del
%token <text> add
%token <text> igual
%token <text> diagonal
%token <text> num
%token <text> caracter
%token <text> cadena
%token <text> id

/*----------Not terminals------------*/
%type <nodito> INIT
%type <nodito> COMANDO
%type <nodito> MKDISK
%type <nodito> ATRIBUTO_MK

%start INIT

%%

INIT:  COMANDO { raiz = $$; };

COMANDO: mkdisk MKDISK { $$ = new Nodo("MKDISK","");
                         $$->add(*$2);
                       };

MKDISK: MKDISK ATRIBUTO_MK {
                             $$ = $1;
                             $$->add(*$2);
                            }
        |ATRIBUTO_MK {
                       $$ = new Nodo("ATRIBUTO","");
                       $$->add(*$1);
                     };

ATRIBUTO_MK: size igual num {
                              Nodo *n = new Nodo("Size",$3);
                              $$ = n;
                            };
