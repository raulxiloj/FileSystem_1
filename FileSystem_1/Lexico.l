%{
    #include <iostream>
    #include "parser.h"
    Nodo *raiz;
%}

%option case-insensitive
%option noyywrap

/*Regular expresions*/
letra [a-zA-Z]
entero [0-9]+
caracter ({letra}|{entero})
id {letra}({letra}|{entero}|"_")*
cadena "\""[^"\""]*"\""
extension "."{id}

%%
"mkdisk"    { return mkdisk; }
"rmdisk"    { return rmdisk; }
"fdisk"     { return fdisk; }
"mount"     { return mount; }
"unmount"   { return unmount; }
"rep"       { return rep; }
"exec"      { return exec; }
"-size"     { return size; }
"-unit"     { return unit; }
"-path"     { return path; }
"-fit"      { return fit; }
"-name"     { return name; }
"-type"     { return type; }
"-delete"   { return del; }
"-add"      { return add; }
"-id"       { return id; }
"bf"        { return bf; }
"ff"        { return ff; }
"wf"        { return wf; }
"Fast"      { return fast; }
"Full"      { return full; }
"mbr"       { return mbr; }
"disk"      { return disk; }

"="         { return igual; }
"/"         { return diagonal; }

{entero}    {strcpy(yylval.text, yytext); return num;}
{caracter}  {strcpy(yylval.text, yytext); return caracter;}
{cadena}    {strcpy(yylval.text, yytext); return cadena;}
{id}        {strcpy(yylval.text, yytext); return identificador;}
{extension} {strcpy(yylval.text, yytext); return extension;}

[[:blank:]] {}

.           {std::cout <<yytext <<" Error Lexico" << std::endl;}


%%
