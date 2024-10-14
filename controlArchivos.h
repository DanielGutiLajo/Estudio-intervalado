#ifndef CONTROL_ARCHIVOS_H
#define CONTROL_ARCHIVOS_H

#include <stdio.h>

char** copiarNombreArchivos(const char* ruta, int* numArchivos);
void crearCarpetaSiNoExiste(const char* ruta);
#endif // CONTROL_ARCHIVOS_H