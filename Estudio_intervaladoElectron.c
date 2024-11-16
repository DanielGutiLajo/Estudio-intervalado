#include <stdbool.h> // bool
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> //cosas de strings
#include <node_api.h>

#include "controlArchivos.h"
//* Organización de las tareas en el txt
//Nombre sin espacios    ultima fecha       siguiente fecha       gradp

char nombreArchivo[100];
char** tareas = NULL;
short numLineas = 0;
short capacidad = 20;


void liberarMemoria() {
    puts("Liberando memoria:");
    for (int i = 0; i < numLineas; i++) {
        free(tareas[i]);  // Liberar la memoria de cada línea
    }

    free(tareas);  // Liberar el array dinámico
}
void anadirRutaAlInicio(char* nombreArchivo) {
    // La ruta que queremos añadir al principio
    const char* ruta = "Archivos\\Estudios\\";

    // Tamaño necesario para la nueva cadena
    size_t longitudRuta = strlen(ruta);
    size_t longitudNombre = strlen(nombreArchivo);

    // Asegúrate de que el tamaño del buffer es suficiente
    if (longitudRuta + longitudNombre >= 100) { // Suponiendo que el tamaño máximo de nombreArchivo es 100
        printf("Error: El nombre del archivo es demasiado largo.\n");
        return;
    }

    // Mueve el contenido de nombreArchivo hacia adelante para hacer espacio para la ruta
    memmove(nombreArchivo + longitudRuta, nombreArchivo, longitudNombre + 1);

    // Copia la ruta al inicio
    memcpy(nombreArchivo, ruta, longitudRuta);
}
bool anadirLinea() {
    numLineas++;
    if (numLineas >= capacidad) {
        capacidad *= 2;  // Doblar la capacidad del array
        char** temp = realloc(tareas, capacidad * sizeof(char*));
        if (temp == NULL) {
            perror("Error al reasignar memoria, en la funcióon anadirLinea");
            for (int i = 0; i < numLineas; i++)
                free(tareas[i]);  // Liberar la memoria previamente asignada
            free(tareas);
            return true;
        }
        tareas = temp;
    }

    return false;
}
bool pasarTareasAVariable() {
    numLineas = 0;
    FILE* tareasArchivo;
    // Abrir el archivo de tareas
    tareasArchivo = fopen(nombreArchivo, "r");

    if (tareasArchivo == NULL) {
        perror("Error al abrir el archivo");
        fclose(tareasArchivo);
        return false;
    }
    // Asignar memoria inicial para el array de punteros a char
    tareas = (char**)malloc(capacidad * sizeof(char*));

    if (tareas == NULL) {
        perror("Error al asignar memoria");
        fclose(tareasArchivo);
        return true;
    }



    char buffer[128];
    // Leer cada línea del archivo
    while (fgets(buffer, 128, tareasArchivo) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        bool error = anadirLinea();
        if (error) {
            fclose(tareasArchivo);
            return true;
        }

        // Asignar memoria para la línea actual y copiarla al array
        tareas[numLineas - 1] = (char*)malloc((strlen(buffer) + 1) * sizeof(char));
        if (tareas[numLineas - 1] == NULL) {
            perror("Error al asignar memoria para la línea");
            for (int i = 0; i < numLineas - 1; i++)
                free(tareas[i]);  // Liberar la memoria previamente asignada
            free(tareas);
            fclose(tareasArchivo);
            return true;
        }

        strcpy(tareas[numLineas - 1], buffer);  // Copiar la línea al array
    }
    // Cerrar el archivo
    fclose(tareasArchivo);
    return false;
}
void preguntarGrado(char grado[]) {
    bool gradoValido = false;
    do {
        puts("Cual es el grado que le deseas dar al estudio?(0 estudiar manana, 1 dentro de tres días, 2 una semana, 3 dos semanas, 4 28 dias ...)");
        scanf(" %s", grado);

        char* endptr;
        int gradoInt = strtol(grado, &endptr, 10);
        if (atoi(grado) < 0 || atoi(grado) > 99 && *endptr != '\0') {
            // Verificar si la entrada es numérica
            if (*endptr != '\0')
                puts("Entrada inválida. Por favor, ingresa un número.");
            else
                puts("Grado inválido. Por favor, ingresa un número entre 0 y 99");
            gradoValido = false;
        }
        else
            gradoValido = true;
    } while (!gradoValido);
}
void extraerFecha(char fecha[], char tarea[]) {
    int palabra_numero = 1;
    char tareaCopia[128];
    strcpy(tareaCopia, tarea);
    char* token = strtok(tareaCopia, " \t\n");
    while (token != NULL) {
        if (palabra_numero == 3) {
            strcpy(fecha, token);
            break;
        }
        token = strtok(NULL, " \t\n");
        palabra_numero++;
    }
}

//*Tiempo
time_t tiempo_actual;
struct tm* info_tiempo;
void actualizarTiempo() {
    // Obtener el tiempo actual del sistema
    time(&tiempo_actual);

    // Convertir el tiempo a una estructura de tiempo local
    info_tiempo = localtime(&tiempo_actual);
}
void copiarFechaHoy(char fechaHoy[]) {
    actualizarTiempo();
    sprintf(fechaHoy, "%02d/%02d/%d",
        info_tiempo->tm_mday,      // Día del mes (1-31)
        info_tiempo->tm_mon + 1,   // Mes (0-11, por eso se suma 1)
        info_tiempo->tm_year + 1900);
}
bool esTareaConFecha(int indice, char fecha[]) {
    char fechaIndice[128];
    extraerFecha(fechaIndice, tareas[indice]);
    if (strcmp(fechaIndice, fecha) == 0)
        return true;
    else
        return false;
}
bool fechaTareaAnteriorOIgual(int indice, char fecha[]) {
    struct tm fechaTarea = { 0 }, fechaComparar = { 0 };

    sscanf(fecha, "%2d/%2d/%4d", &fechaComparar.tm_mday, &fechaComparar.tm_mon, &fechaComparar.tm_year);
    fechaComparar.tm_year -= 1900; // Ajustar año
    fechaComparar.tm_mon -= 1;     // Ajustar mes

    char fechaIndice[128];
    extraerFecha(fechaIndice, tareas[indice]);
    sscanf(fechaIndice, "%2d/%2d/%4d", &fechaTarea.tm_mday, &fechaTarea.tm_mon, &fechaTarea.tm_year);
    fechaTarea.tm_year -= 1900; // Ajustar año
    fechaTarea.tm_mon -= 1;     // Ajustar mes

    time_t tiempoTarea = mktime(&fechaTarea);
    time_t tiempoComparar = mktime(&fechaComparar);

    return difftime(tiempoTarea, tiempoComparar) <= 0;
}
int buscarPosicionFecha(char fecha[]) {
    for (int i = 0; i < numLineas;i++) {
        if (fechaTareaAnteriorOIgual(i, fecha))
            return i;
    }
    return numLineas;
}

void calcularSiguienteFecha(struct tm fecha_original, int grado, char resultado[]) {
    int dias;
    if (grado == 0)
        dias = 1;
    else if (grado == 1)
        dias = 3;
    else if (grado == 2)
        dias = 7;
    else {
        dias = 7;
        for (int contador = grado; contador > 2; contador--)
            dias *= 2;
    }
    // Copia la fecha original para trabajar sobre la copia
    struct tm fecha_modificada = fecha_original;

    // Convertir la estructura tm a time_t
    time_t tiempo_modificado = mktime(&fecha_modificada);

    // Sumar los días (1 día = 86400 segundos)
    tiempo_modificado += dias * 86400;

    // Convertir de vuelta a la estructura tm
    struct tm* nueva_fecha = localtime(&tiempo_modificado);

    sprintf(resultado, "%02d/%02d/%d",
        nueva_fecha->tm_mday,
        nueva_fecha->tm_mon + 1,   // Mes (se suma 1 porque empieza en 0)
        nueva_fecha->tm_year + 1900); // Año (se suma 1900)
}
int buscarIndiceTareasHoy() {
    char fechaHoy[100]; // La palabra que queremos buscar
    copiarFechaHoy(fechaHoy);
    return buscarPosicionFecha(fechaHoy);
}

bool escribirTareasHoy() {
    puts("Estudios por hacer:");
    int numerador = 1;
    char fechaHoy[100];
    copiarFechaHoy(fechaHoy);

    bool hayTareas = false;
    int i = 0;
    //Busca la primera tarea que es de hoy o anterior
    while (i < numLineas && !fechaTareaAnteriorOIgual(i, fechaHoy))
        i++;
    if (i >= numLineas || !esTareaConFecha(i, fechaHoy))
        puts("No hay estudios programados para hoy");
    else
        puts("Nombre   est ant   est post   grado");

    for (; i < numLineas && esTareaConFecha(i, fechaHoy); i++) {
        printf("%d. %s\n", numerador, tareas[i]);
        numerador++;
        hayTareas = true;
    }

    //Escribir tareas anteriores si las hay
    if (i < numLineas && fechaTareaAnteriorOIgual(i, fechaHoy)) {
        if (!hayTareas)
            puts("Nombre   est ant   est post   grado");
        puts("\nEstudios anteriores");
        for (; i < numLineas; i++) {
            printf("%d. %s\n", numerador, tareas[i]);
            numerador++;
            hayTareas = true;
        }
    }
    return hayTareas;
}

bool anadirTarea(char tareaAnadir[], char fecha[]) {
    int i = buscarPosicionFecha(fecha), j = numLineas;

    bool error = anadirLinea();
    if (error)
        return true;


    for (; j > i;j--)
        tareas[j] = tareas[j - 1];
    tareas[i] = (char*)malloc((strlen(tareaAnadir) + 1) * sizeof(char));
    if (tareas[i] == NULL) {
        puts("Error al asignar memoria anadiendo tarea");
        return true;
    }

    strcpy(tareas[i], tareaAnadir);
    return false;
}
void moverTarea(int indiceOrigen, int indiceDestino) {
    char* temp = tareas[indiceOrigen];
    if (indiceOrigen > indiceDestino)
        for (int i = indiceOrigen - 1; i >= indiceDestino; i--)
            tareas[i + 1] = tareas[i];

    else if (indiceOrigen < indiceDestino)
        for (int i = indiceOrigen + 1; i <= indiceDestino; i++)
            tareas[i - 1] = tareas[i];

    tareas[indiceDestino] = temp;
}
bool copiarCambiosArchivo() {
    // Abrir el archivo en modo escritura
    FILE* tareasArchivo = fopen(nombreArchivo, "w");
    if (tareasArchivo == NULL) {
        perror("Error al abrir el archivo");
        return true;
    }

    // Escribir cada línea en el archivo
    for (int i = 0; i < numLineas; i++)
        fprintf(tareasArchivo, "%s\n", tareas[i]);  // Escribir la tarea actual en el archivo


    // Cerrar el achivo
    fclose(tareasArchivo);
    printf("Cambios guardados\n");
    return false;
}
int main() {
    return 0;
}

napi_value archivosValidos(napi_env env, napi_callback_info info) {
    crearCarpetaSiNoExiste("Archivos");
    crearCarpetaSiNoExiste("Archivos\\Estudios");
    int numArchivos;
    char** tareasNombres = copiarNombreArchivos("Archivos\\Estudios\\*", &numArchivos);
    napi_value jsArray;
    napi_create_array_with_length(env, numArchivos, &jsArray);

    // Convertir cada string en un `napi_value` y agregarlo al array
    for (int i = 0; i < numArchivos; i++) {
        napi_value jsString;
        napi_create_string_utf8(env, tareasNombres[i], strlen(tareasNombres[i]), &jsString);
        free(tareasNombres[i]);
        napi_set_element(env, jsArray, i, jsString);
    }

    free(tareasNombres);
    return jsArray;
}

napi_value archivoAbrir(napi_env env, napi_callback_info info) {
    FILE* archivoConfiguracion = fopen("Archivos\\Configuracion.txt", "r");
    napi_value result;
    if (archivoConfiguracion == NULL) {
        archivoConfiguracion = fopen("Archivos\\Configuracion.txt", "w");
        if (archivoConfiguracion == NULL) {
            napi_throw_error(env, NULL, "Error al crear el archivo de configuración");
            return NULL;
        }
        fprintf(archivoConfiguracion, "0");
        fclose(archivoConfiguracion);


        napi_create_string_utf8(env, "0", NAPI_AUTO_LENGTH, &result);
    }
    else {
        char buffer[50];
        fgets(buffer, sizeof(buffer), archivoConfiguracion);
        // Devuelve el contenido del buffer como un napi_value
        napi_create_string_utf8(env, buffer, NAPI_AUTO_LENGTH, &result);
        //napi_create_string_utf8(env, "1", NAPI_AUTO_LENGTH, &result);
        fclose(archivoConfiguracion);
    }
    return result;
}

napi_value pasarTareas(napi_env env, napi_callback_info info) {
    int numeroArchivo;
    FILE* archivoConfiguracion = fopen("Archivos\\Configuracion.txt", "r");
    if (archivoConfiguracion == NULL) {
        puts("Error leyeno el archivo de configuracion en pasarTareas");
        napi_throw_error(env, NULL, "Error al abrir Configuracion.txt");
        return NULL;
    }
    fscanf(archivoConfiguracion, "%d", &numeroArchivo);
    fclose(archivoConfiguracion);

    int numArchivos;
    char** tareasNombres = copiarNombreArchivos("Archivos\\Estudios\\*", &numArchivos);

    sprintf(nombreArchivo, "Archivos\\Estudios\\%s", tareasNombres[numeroArchivo]);
    printf("%s", nombreArchivo);

    bool error = pasarTareasAVariable();
    if (error) {
        puts("Error leyendo variables");
        return NULL;
    }
    napi_value jsArray;
    napi_create_array_with_length(env, numLineas, &jsArray);
    puts("\n");
    napi_status status;

    // Verifica la creación del array
    status = napi_create_array_with_length(env, numLineas, &jsArray);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Error al crear el array de JavaScript");
        return NULL;
    }

    // Dentro del bucle
    for (int i = 0; i < numLineas; i++) {
        if (!tareas[i]) {
            napi_throw_error(env, NULL, "Cadena nula encontrada en tareas");
            return NULL;
        }

        printf("%s\n", tareas[i]);

        napi_value jsString;
        status = napi_create_string_utf8(env, tareas[i], NAPI_AUTO_LENGTH, &jsString);
        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Error al crear cadena de JavaScript");
            return NULL;
        }

        status = napi_set_element(env, jsArray, i, jsString);
        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Error al agregar elemento al array de JavaScript");
            return NULL;
        }
    }

    return jsArray;
    puts("Esto funciona");
}
napi_value nuevaTarea(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

    // Verificar que hay un argumento
    if (argc < 1) {
        puts("No se puso un argumento a la funcion nuevaTarea");
        napi_throw_error(env, NULL, "Se requiere un argumento");
        return NULL;
    }

    // Asegurarte de que el argumento es una cadena
    char tareaAnadir[100];
    size_t tareaAnadir_length;
    napi_get_value_string_utf8(env, argv[0], tareaAnadir, 100, &tareaAnadir_length);

    char fechaSiguiente[50];
    extraerFecha(fechaSiguiente, tareaAnadir);
    anadirTarea(tareaAnadir, fechaSiguiente);

    bool error = copiarCambiosArchivo();
    if (error) {
        puts("Error al copiar los cambios al archivo");
        napi_throw_error(env, NULL, "Error al copiar los cambios al archivo");
        liberarMemoria();
        return NULL;
    }

    // Retornar undefined
    napi_value resultado;
    napi_get_undefined(env, &resultado);
    return resultado;
}
napi_value borrarTarea(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

    // Verificar que hay un argumento
    if (argc < 1) {
        puts("No se puso un argumento a la funcion nuevaTarea");
        napi_throw_error(env, NULL, "Se requiere un argumento");
        return NULL;
    }

    // Asegurarte de que el argumento es una cadena
    int32_t tareaIndex;
    napi_get_value_int32(env, argv[0], &tareaIndex);


    numLineas--;
    for (int i = tareaIndex;i < numLineas;i++)
        tareas[i] = tareas[i + 1];
    bool error = copiarCambiosArchivo();
    if (error) {
        puts("Error al copiar los cambios al archivo");
        napi_throw_error(env, NULL, "Error al copiar los cambios al archivo");
        liberarMemoria();
        return NULL;
    }
    // Retornar undefined
    napi_value resultado;
    napi_get_undefined(env, &resultado);
    return resultado;
}
napi_value editarTarea(napi_env env, napi_callback_info info) {
    size_t argc = 2; // Esperamos 2 argumentos
    napi_value argv[2];
    napi_status status;

    // Obtener la información de la función llamada desde JavaScript
    status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Error al obtener los argumentos");
        return NULL;
    }

    // Verificar que se han pasado ambos argumentos
    if (argc < 2) {
        puts("No se pusieron suficientes argumentos a la función editarTarea");
        napi_throw_error(env, NULL, "Se requieren dos argumentos");
        return NULL;
    }

    // Obtener el primer argumento como un número entero
    int32_t tareaIndex;
    status = napi_get_value_int32(env, argv[0], &tareaIndex);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "El primer argumento debe ser un número entero");
        return NULL;
    }

    // Paso 1: Obtener la longitud de la cadena primero
    size_t tarea_length;
    status = napi_get_value_string_utf8(env, argv[1], NULL, 0, &tarea_length);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Error al obtener la longitud de la cadena");
        return NULL;
    }

    // Paso 2: Asignar memoria dinámicamente, sumando 1 para el terminador nulo '\0'
    tareas[tareaIndex] = (char*)malloc((tarea_length + 1) * sizeof(char));
    if (tareas[tareaIndex] == NULL) {
        napi_throw_error(env, NULL, "Error de asignación de memoria");
        return NULL;
    }

    // Paso 3: Obtener la cadena real
    status = napi_get_value_string_utf8(env, argv[1], tareas[tareaIndex], tarea_length + 1, &tarea_length);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "El segundo argumento debe ser una cadena");
        return NULL;
    }
    // Guardar los cambios en el archivo
    bool error = copiarCambiosArchivo();
    if (error) {
        puts("Error al copiar los cambios al archivo");
        napi_throw_error(env, NULL, "Error al copiar los cambios al archivo");
        liberarMemoria();
        return NULL;
    }

    // Retornar undefined
    napi_value resultado;
    status = napi_get_undefined(env, &resultado);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Error al obtener 'undefined'");
        return NULL;
    }

    return resultado;
}
napi_value Init(napi_env env, napi_value exports) {
    napi_value fn;

    napi_create_function(env, NULL, 0, archivosValidos, NULL, &fn);
    napi_set_named_property(env, exports, "archivosValidos", fn);

    napi_create_function(env, NULL, 0, archivoAbrir, NULL, &fn);
    napi_set_named_property(env, exports, "archivoAbrir", fn);

    napi_create_function(env, NULL, 0, pasarTareas, NULL, &fn);
    napi_set_named_property(env, exports, "pasarTareas", fn);

    napi_create_function(env, NULL, 0, nuevaTarea, NULL, &fn);
    napi_set_named_property(env, exports, "nuevaTarea", fn);

    napi_create_function(env, NULL, 0, borrarTarea, NULL, &fn);
    napi_set_named_property(env, exports, "borrarTarea", fn);

    napi_create_function(env, NULL, 0, editarTarea, NULL, &fn);
    napi_set_named_property(env, exports, "editarTarea", fn);
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init);
