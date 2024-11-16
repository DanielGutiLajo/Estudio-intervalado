#include <stdbool.h> // bool
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> //cosas de strings

#include "controlArchivos.h"
//* Organización de las tareas en el txt
//Nombre sin espacios    ultima fecha       siguiente fecha       gradp

char nombreArchivo[100];
char** tareas = NULL;
short numLineas = 0;
short capacidad = 20;

void saltoDobleLinea() {
    puts("\n");
}
void saltoUnaLinea() {
    puts("");
}
bool comprobar_NoY(const char respuesta) {
    return respuesta != 'Y' && respuesta != 'N' && respuesta != 'y' && respuesta != 'n';
}
bool preguntar(char pregunta[]) {
    char respuesta;
    do {
        printf("%s (Y,N) \n", pregunta);
        scanf(" %c", &respuesta);
        if (comprobar_NoY(respuesta))
            puts("Respuesta incorrecta repita");
    } while (comprobar_NoY(respuesta));
    return (respuesta == 'Y' || respuesta == 'y');
}
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
void extraerFecha(char fecha[], int indice) {
    int palabra_numero = 1;
    char tareaCopia[128];
    strcpy(tareaCopia, tareas[indice]);
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
    extraerFecha(fechaIndice, indice);
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
    extraerFecha(fechaIndice, indice);
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

char preguntarAccion() {
    char respuestaAccion;
    do {
        puts("Elige una tarea");
        puts("1. Agregar un estudio");
        puts("2. Ver todos los estudios");
        puts("3. Ver los estudios de hoy");
        puts("4. Marcar como estudiada ");
        puts("5. Ver copias de seguridad ");
        puts("6. Cerrar el programa ");
        saltoUnaLinea();
        scanf(" %c", &respuestaAccion);

        if (respuestaAccion != '1' && respuestaAccion != '2' && respuestaAccion != '3' && respuestaAccion != '4' && respuestaAccion != '5' && respuestaAccion != '6')
            puts("Respuesta no esperada, vuelve a intentarlo");
    } while (respuestaAccion != '1' && respuestaAccion != '2' && respuestaAccion != '3' && respuestaAccion != '4' && respuestaAccion != '5' && respuestaAccion != '6');
    saltoUnaLinea();
    return respuestaAccion;
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
    crearCarpetaSiNoExiste("Archivos");
    crearCarpetaSiNoExiste("Archivos\\Estudios");
    int numArchivos;
    char** tareasNombres = copiarNombreArchivos("Archivos\\Estudios\\*", &numArchivos);
    for (int i = 0; i < numArchivos;i++)
        printf("%d. %s\n", i + 1, tareasNombres[i]);
    fputs("Escribe el numero del archivo que quieres o 0 para uno nuevo: ", stdout);
    int decision = 0;
    scanf("%d", &decision);
    if (decision == 0) {
        printf("Escribe el nombre del nuevo archivo: ");
        scanf("%s", nombreArchivo);
        // Añadir la extensión al nombre
        strcat(nombreArchivo, ".txt");
        anadirRutaAlInicio(nombreArchivo);
        FILE* archivo = fopen(nombreArchivo, "w");
        if (archivo != NULL)
            fclose(archivo);

        else
            printf("Error al abrir el archivo\n"); 
    }
    else if (decision > 0 && decision <= numArchivos) {
        strcpy(nombreArchivo, tareasNombres[decision - 1]);
        anadirRutaAlInicio(nombreArchivo);
    }
    else {
        printf("Opción no válida.\n");
        // Libera la memoria y termina el programa si la decisión no es válida
        for (int i = 0; i < numArchivos; i++)
            free(tareasNombres[i]);
        free(tareasNombres);
        return 1;
    }
    
    for (int i = 0; i < numArchivos; i++)
        free(tareasNombres[i]);
    free(tareasNombres);

    bool error = pasarTareasAVariable();
    if (error) {
        puts("Error al pasar a la variable");
        liberarMemoria();
        return 1;
    }

    saltoDobleLinea();
    puts("Hola, soy el programa de estudio intervalado.");

    saltoUnaLinea();
    escribirTareasHoy();
    saltoDobleLinea();

    bool programaFuncionando = true;
    do {
        saltoDobleLinea();
        char respuestaAccion = preguntarAccion();

        if (respuestaAccion == '1') {
            char nombre[50];
            puts("Cual es el nombre que le deseas dar al estudio?(menos de 50 caracteres y sin espacios)");
            scanf(" %s", nombre);

            char fechaAnterior[32];
            copiarFechaHoy(fechaAnterior);

            char grado[3];
            preguntarGrado(grado);


            char fechaSiguiente[32];
            calcularSiguienteFecha(*info_tiempo, atoi(grado), fechaSiguiente);

            char tareaAnadir[126];
            snprintf(tareaAnadir, sizeof(tareaAnadir), "%s %s %s %s", nombre, fechaAnterior, fechaSiguiente, grado);
            anadirTarea(tareaAnadir, fechaSiguiente);
            saltoUnaLinea();
            error = copiarCambiosArchivo();
            if (error) {
                puts("Error al copiar los cambios al archivo");
                liberarMemoria();
                return 1;
            }
            saltoUnaLinea();
        }




        else if (respuestaAccion == '2') {
            if (numLineas == 0)
                puts("No hay estudios guardados");
            else
                puts("Nombre   est ant   est post   grado");
            for (int i = 0; i < numLineas;i++)
                printf("%s\n", tareas[i]);
        }




        else if (respuestaAccion == '3') {
            escribirTareasHoy();
        }


        else if (respuestaAccion == '4') {
            bool hayTareas = escribirTareasHoy();
            if (hayTareas) {


                int indiceRelativo = 0;
                char fechaHoy[100]; // La palabra que queremos buscar
                copiarFechaHoy(fechaHoy);

                int indiceTareasHoy = buscarIndiceTareasHoy();
                char respuestaBinaria;
                do {
                    puts("\nQue tarea quieres marcar como estudiada?(Escribe el numero)");
                    scanf(" %d", &indiceRelativo);

                    do {
                        printf("El nombre eligido es:\n%s.\nEs correcto?(Y,N)\n  ", tareas[indiceRelativo + indiceTareasHoy - 1]);
                        scanf(" %c", &respuestaBinaria);
                        if (respuestaBinaria != 'Y' && respuestaBinaria != 'N')
                            puts("La respuesta dada no es valida repita");
                    } while (respuestaBinaria != 'Y' && respuestaBinaria != 'N');
                    if (!fechaTareaAnteriorOIgual(indiceRelativo + indiceTareasHoy - 1, fechaHoy))
                        puts("Numero incorrecto diga otro");
                } while (!fechaTareaAnteriorOIgual(indiceRelativo + indiceTareasHoy - 1, fechaHoy) || respuestaBinaria == 'N');
                saltoUnaLinea();
                int indiceTareaEstudiada = indiceRelativo + indiceTareasHoy - 1;

                char nombre[50], palabra2[50], fechaAntigua[50], gradoPrevio[50];
                // Dividir la línea en palabras
                sscanf(tareas[indiceTareaEstudiada], "%s %s %s %s", nombre, palabra2, fechaAntigua, gradoPrevio);

                char grado[3];
                printf("El grado actual es: %s\n", gradoPrevio);
                preguntarGrado(grado);

                char fechaSiguiente[50];
                calcularSiguienteFecha(*info_tiempo, atoi(grado), fechaSiguiente);
                // Reconstruir la línea con las modificaciones
                snprintf(tareas[indiceTareaEstudiada], 126, "%s %s %s %s", nombre, fechaAntigua, fechaSiguiente, grado);


                moverTarea(indiceTareaEstudiada, buscarPosicionFecha(fechaSiguiente));

                saltoUnaLinea();
                error = copiarCambiosArchivo();

                if (error) {
                    printf("Error al copiar los cambios al archivo\n");
                    liberarMemoria();
                    return 1;
                }
            }
        }
        else if (respuestaAccion == '5') {
            /*bool acabarCopia;
            do {
                int numeroCopiaRevisar = 0;
                for (int i = 1; i < numCopias;i++)
                    printf("%d. %s", i, copiaNombre[i]);
                 saltoUnaLinea();
                printf("Que copia quieres revisar?");
                scanf(" %d", numeroCopiaRevisar);
                copiaRevisar = fopen(copiaNombre[numeroCopiaRevisar], "r");
                char buffer[128];
                while (fgets(buffer, 128, copiaRevisar) != NULL)
                    printf("%s", buffer);
                fclose(copiaRevisar);
                if (preguntar("Deseas sustituir el actual por la copia?"));
                else
                    acabarCopia = preguntar("Deseas terminar el proceso de revisar copias?");
            } while (acabarCopia == "N" || acabarCopia == "n");*/

        }

        else if (respuestaAccion == '6')
            programaFuncionando = false;

    } while (programaFuncionando);
    liberarMemoria();
    return 0;
}

