#include <stdbool.h> // bool
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//* Organización de las tareas en el txt
//Nombre sin espacios    ultima fecha       siguiente fecha       gradp

char** tareas = NULL;
int numLineas = 0;
int capacidad = 20;
bool anadirLinea() {
    if (numLineas >= capacidad) {
        capacidad *= 2;  // Doblar la capacidad del array
        char** temp = realloc(tareas, capacidad * sizeof(char*));
        if (temp == NULL) {
            perror("Error al reasignar memoria, en la funcióon anadirLinea");
            for (int i = 0; i < numLineas; i++) {
                free(tareas[i]);  // Liberar la memoria previamente asignada
            }
            free(tareas);

            return true;
        }
        tareas = temp;
    }
    numLineas++;
}
bool pasarTareasAVariable() {
    FILE* tareasArchivo;
    // Abrir el archivo de tareas
    tareasArchivo = fopen("tareas.txt", "r");

    // Asignar memoria inicial para el array de punteros a char
    tareas = (char**)malloc(capacidad * sizeof(char*));

    if (tareas == NULL) {
        perror("Error al asignar memoria");
        fclose(tareasArchivo);
        return true;
    }

    if (tareasArchivo == NULL) {
        // Si el archivo no existe, crearlo con "w+"
        printf("El archivo tareas.txt no existe, creando uno vacío...\n");
        tareasArchivo = fopen("tareas.txt", "w+");
        if (tareasArchivo == NULL) {
            // Si por algún motivo la creación falla
            printf("No se pudo crear el archivo.\n");
            return true;
        }
        return false;
    }

    char buffer[128];
    // Leer cada línea del archivo
    while (fgets(buffer, 128, tareasArchivo) != NULL) {
        // Comprobar si necesitamos más espacio en el array de punteros
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
void extraerFecha(char fecha[], char linea[]) {
    int palabra_numero = 1; // Contador de palabras
    // Usar strtok para dividir la línea en palabras usando el espacio como delimitador
    fecha = strtok(linea, " \t\n"); // Eliminar espacios, tabulaciones y saltos de línea

    // Iterar por cada palabra en la línea
    while (fecha != NULL) {
        if (palabra_numero == 3) { // Si es la tercera palabra
            break;
        }
        // Pasar a la siguiente palabra
        fecha = strtok(NULL, " \t\n");
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
    char* palabra;
    int palabra_numero = 1; // Contador de palabras
    char buffer[128]; // Buffer para copiar la línea actual
    // Copiar la línea a un buffer para que strtok no modifique la cadena original
    strcpy(buffer, tareas[indice]);
    // Usar strtok para dividir la línea en palabras usando el espacio como delimitador
    palabra = strtok(buffer, " \t\n"); // Eliminar espacios, tabulaciones y saltos de línea

    // Iterar por cada palabra en la línea
    while (palabra != NULL) {
        if (palabra_numero == 3) { // Si es la tercera palabra
            if (strcmp(palabra, fecha) == 0) // Comprobar si es igual a la palabra buscada
                return true;
            break; // Salir del bucle ya que solo nos interesa la tercera palabra
        }
        // Pasar a la siguiente palabra
        palabra = strtok(NULL, " \t\n");
        palabra_numero++;
    }
    return false;
}
int buscarIndiceFecha(char fecha[]) {
    // Leer el archivo línea por línea
    for (int i = 0; i < numLineas;i++) {
        if (esTareaConFecha(i, fecha))
            return i;
    }
    return -1;
}

void calcularSiguienteFecha(struct tm fecha_original, int grado, char resultado[]) {
    actualizarTiempo();
    int dias;
    if (grado == 1)
        dias = 1;
    else if (grado == 2)
        dias = 3;
    else if (grado == 3)
        dias = 7;
    else if (grado == 4)
        dias = 14;
    else if (grado == 5)
        dias = 30;
    else {
        dias = 30;
        for (int contador = grado; contador > 5; contador--)
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
    return buscarIndiceFecha(fechaHoy);
}

bool escribirTareasHoy() {
    char fechaHoy[100]; // La palabra que queremos buscar

    sprintf(fechaHoy, "%02d/%02d/%d",
        info_tiempo->tm_mday,      // Día del mes (1-31)
        info_tiempo->tm_mon + 1,   // Mes (0-11, por eso se suma 1)
        info_tiempo->tm_year + 1900);

    int numerador = 1;
    int i = buscarIndiceTareasHoy();
    if (i == -1)
        return true;
    for (; i < numLineas;i++) {
        if (esTareaConFecha(i, fechaHoy)) {
            printf("%d. %s \n", numerador, tareas[i]);
            numerador++;
        }
        else
            return false;
    }
    return false;
}

char preguntarAccion() {
    char respuestaAccion;
    do {
        printf("¿Qué deseas hacer?\n");
        printf("1. Agregar un estudio\n");
        printf("2. Ver todos los estuidos\n");
        printf("3. Ver los estudios de hoy\n");
        printf("4. Marcar como estudiada \n");

        scanf(" %c", &respuestaAccion);

        if (respuestaAccion != '1' && respuestaAccion != '2' && respuestaAccion != '3') {
            printf("Respuesta no esperada, vuelve a intentarlo\n");
        }
    } while (respuestaAccion != '1' && respuestaAccion != '2' && respuestaAccion != '3');
    return respuestaAccion;
}

bool anadirTarea(char tareaAnadir[], char fecha[]) {
    int i = buscarIndiceFecha(fecha), j = numLineas;
    bool error = anadirLinea();
    if (error)
        return true;

    
    for (; j > i;j--) 
        tareas[j] = tareas[j - 1];
    tareas[i] = (char*)malloc((strlen(tareaAnadir) + 1) * sizeof(char));
    strcpy(tareas[i], tareaAnadir);
    return false;
}
int main() {
    bool error = pasarTareasAVariable();
    if (error) {
        printf("Error al pasar a la variable");
        return 1;
    }

    printf("Hola, soy la máquina de estudio intervalado.\n");
    char cerrarPrograma = 'a';
    do {
        char respuestaAccion = preguntarAccion();

        if (respuestaAccion = '1') {
            char respuestaBinaria;
            char nombre[50];
            do {
                printf("Cual es el nombre que le deseas dar al estudio?(menos de 50 caracteres y sin espacios)\n");
                scanf(" %s", nombre);
                do {
                    printf("El nombre eligido es %s?(Y,N)", nombre);
                    if (respuestaBinaria != 'Y' && respuestaBinaria != 'N')
                        printf("La respuesta dada no es valida repita");
                } while (respuestaBinaria != 'Y' && respuestaBinaria != 'N');

            } while (respuestaBinaria == 'N');

            char fechaAnterior[50];
            copiarFechaHoy(fechaAnterior);

            int grado;
            do {
                printf("Cual es el grado que le deseas dar al estudio?(0 estudiar manana, 1 dentro de tres días, 2 una semana, 3 dos semanas, 4 un mes ...)\n");
                scanf(" %s", grado);

                do {
                    printf("El grado eligido es %s?(Y,N)", grado);
                    if (respuestaBinaria != 'Y' && respuestaBinaria != 'N')
                        printf("La respuesta dada no es valida repita");
                } while (respuestaBinaria != 'Y' && respuestaBinaria != 'N');

            } while (respuestaBinaria == 'N');

            char fechaSiguiente[50];
            calcularSiguienteFecha(*info_tiempo, grado, fechaSiguiente);

            buscarIndiceFecha(fechaSiguiente);
            char tareaAnadir[126];
            sprintf(tareaAnadir, "%s %s %s %d\n", nombre, fechaAnterior, fechaSiguiente, grado);
            anadirTarea(tareaAnadir, fechaSiguiente);
        }





        else if (respuestaAccion = '2') {
            char linea[512]; // Buffer para almacenar cada línea

            // Leer el archivo línea por línea y mostrarlo en pantalla
            while (fgets(linea, sizeof(linea), tareasArchivo)) {
                printf("%s", linea); // Mostrar la línea leída
            }
            fseek(tareasArchivo, 0, SEEK_SET);
        }








        else if (respuestaAccion = '3') {
            escribirTareasHoy();
        }


        else if (respuestaAccion = '4') {
            buscarIndiceTareasHoy();
            escribirTareasHoy();
            bool dentroRango = true;
            long posicion = 0;

            posicion = ftell(tareasArchivo);
            do {
                int tareaEstudiada = 0;
                printf("\n Que tarea quieres marcar como estudiada?(Escribe el numero)");
                buscarIndiceTareasHoy();
                scanf("%d", tareaEstudiada);
                char linea[512];       // Buffer para almacenar cada línea
                char fechaBuscar[100]; // La palabra que queremos buscar

                sprintf(fechaBuscar, " %02d/%02d/%d",
                    info_tiempo->tm_mday,      // Día del mes (1-31)
                    info_tiempo->tm_mon + 1,   // Mes (0-11, por eso se suma 1)
                    info_tiempo->tm_year + 1900);

                int numerador = 1;


                while (fgets(linea, sizeof(linea), tareasArchivo) && dentroRango && (numerador != tareaEstudiada)) {
                    posicion = ftell(tareasArchivo);
                    char* palabra;
                    int palabra_numero = 1; // Contador de palabras

                    // Usar strtok para dividir la línea en palabras usando el espacio como delimitador
                    palabra = strtok(linea, " \t\n"); // Eliminar espacios, tabulaciones y saltos de línea

                    // Iterar por cada palabra en la línea
                    while (palabra != NULL) {
                        if (palabra_numero == 3) { // Si es la tercera palabra
                            if (strcmp(palabra, fechaBuscar) != 0) // Comprobar si es igual a la palabra buscada
                                dentroRango = false;
                            break; // Salir del bucle ya que solo nos interesa la tercera palabra
                        }
                        // Pasar a la siguiente palabra
                        palabra = strtok(NULL, " \t\n");
                        palabra_numero++;
                    }
                    numerador++;
                }
                if (dentroRango)
                    printf("Ese numero no está disponible diga otro");
            } while (!dentroRango);
            fseek(tareasArchivo, posicion, SEEK_SET);


            char linea[512];
            char nombre[50], palabra2[50], fechaAntigua[50], gradoPrevio[50];

            fgets(linea, sizeof(linea), tareasArchivo);
            // Dividir la línea en palabras
            sscanf(linea, "%s %s %s %s", nombre, palabra2, fechaAntigua, gradoPrevio);

            int grado;
            printf("Que grado quieres asignarle a la tarea el actual es: %d (0 estudiar manana, 1 dentro de tres días, 2 una semana, 3 dos semanas, 4 un mes ...)", gradoPrevio);
            scanf(" %d", grado);

            char fechaSiguiente[50];
            calcularSiguienteFecha(*info_tiempo, grado, fechaSiguiente);
            // Reconstruir la línea con las modificaciones
            sprintf(linea, "%s %s %s %d\n", nombre, fechaAntigua, fechaSiguiente, grado);

            // Limpiar el resto de la línea si la nueva es más corta que la anterior
            fflush(tareasArchivo); // Asegura que la salida se escriba completamente

            fseek(tareasArchivo, posicion, SEEK_SET);

            ordenarTareas();
        }




        do {
            printf("Tarea finalizada desea cerrar el programa?(Y, N)");
            if (cerrarPrograma != 'Y' && cerrarPrograma != 'N')
                printf("La respuesta dada no es valida repita");
        } while (cerrarPrograma != 'Y' && cerrarPrograma != 'N');
    } while (cerrarPrograma != 'Y');
    // Mostrar las líneas leídas
    printf("Liberando memoria:\n");
    for (int i = 0; i < numLineas; i++) {
        free(tareas[i]);  // Liberar la memoria de cada línea
    }

    free(tareas);  // Liberar el array dinámico
    return 0;
}
