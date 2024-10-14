#include "controlArchivos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>//Para copiar texto de un char[] a otro
#include <windows.h> //Manejar directorios en windows

char** aumentarMemoria(char** listaNombres, int* capacidad) {
    *capacidad += 20;
    char** temp = realloc(listaNombres, (*capacidad) * sizeof(char*));
    if (temp == NULL) {
        printf("Error al aumentar la memoria");
        exit(EXIT_FAILURE);
    }
    return temp;
}

void crearCarpetaSiNoExiste(const char* ruta) {
    // Intenta crear la carpeta
    if (!(CreateDirectory(ruta, NULL) || GetLastError() == ERROR_ALREADY_EXISTS)) 
        printf("Error al crear la carpeta '%s'. Código de error: %d\n", ruta, GetLastError());
}

// Pasar rutas con \\ y \\* al final
char** copiarNombreArchivos(const char* ruta, int* numArchivos) {
    if (ruta == NULL) {
        printf("Ruta nula.\n");
        return NULL;
    }

    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    // Intenta encontrar los archivos en el directorio
    hFind = FindFirstFile(ruta, &findFileData);


    if (hFind == INVALID_HANDLE_VALUE) {
        printf("No se pudo abrir el directorio. Error: %d\n", GetLastError());
        return NULL;
    }
    int capacidad = 20;
    char **listaNombres = malloc(20 * sizeof(char*));
    if (listaNombres == NULL) {
        printf("Error al asignar memoria a la lista de Nombres");
        FindClose(hFind); // Cerrar el manejador
        return NULL;
    }
    int i = 0;
    // Lee las entradas del directorio
    do {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (i >= capacidad)
                listaNombres = aumentarMemoria(listaNombres, &capacidad);
            listaNombres[i] = malloc(strlen(findFileData.cFileName) + 1);
            if (listaNombres[i] != NULL) {
                strcpy(listaNombres[i], findFileData.cFileName); // Copiar el nombre del archivo
                i++;
            }
            else
                printf("Error al asignar memoria para el archivo %s\n", findFileData.cFileName);
        }
    } while (FindNextFile(hFind, &findFileData) != 0 && i < 1000);

    if (GetLastError() != ERROR_NO_MORE_FILES)
        printf("Error al leer el siguiente archivo. Error: %d\n", GetLastError());

    *numArchivos = i;

    // Cierra el manejador
    FindClose(hFind);
    puts("");
    return listaNombres;
}
