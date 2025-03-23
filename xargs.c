#define _GNU_SOURCE
#include <stdio.h>      // Para getline, printf
#include <stdlib.h>     // Para exit, malloc, free, etc.
#include <unistd.h>     // Para fork, execvp
#include <sys/types.h>  // Para pid_t
#include <sys/wait.h>   // Para wait
#include <string.h>     // Para strdup

#ifndef NARGS
#define NARGS 4
#endif

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("Error en la cantidad de argumentos");
        exit(EXIT_FAILURE);
    }

    char *comando = argv[1];

    char *linea = NULL;
    size_t len = 0;
    ssize_t leido;

    char *argumentos[NARGS];
    int contador = 0;

    while ((leido = getline(&linea, &len, stdin)) != -1) {
        // Eliminar el salto de línea final
        if (linea[leido - 1] == '\n') {
            linea[leido - 1] = '\0';
        }

        // Guardar una copia de la línea en el array de argumentos
        argumentos[contador] = strdup(linea);
        contador++;

        // Si juntamos NARGS argumentos, ejecutamos el comando
        if (contador == NARGS) {
            char *args[NARGS + 2]; // +1 para comando, +1 para NULL
            args[0] = comando;
            for (int i = 0; i < NARGS; i++) {
                args[i + 1] = argumentos[i];
            }
            args[NARGS + 1] = NULL;

            pid_t pid = fork();
            if (pid == 0) {
                // Proceso hijo
                execvp(comando, args);
                perror("Error al ejecutar comando");
                exit(EXIT_FAILURE);
            } else if (pid > 0) {
                // Proceso padre
                wait(NULL);
                for (int i = 0; i < NARGS; i++) {
                    free(argumentos[i]);
                    argumentos[i] = NULL;
                }
                contador = 0;
            } else {
                perror("fork falló");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Ejecutar lo que quede si no llegó a NARGS
    if (contador > 0) {
		char **args = malloc((contador + 2) * sizeof(char *));
        args[0] = comando;
        for (int i = 0; i < contador; i++) {
            args[i + 1] = argumentos[i];
        }
        args[contador + 1] = NULL;

        pid_t pid = fork();
        if (pid == 0) {
            execvp(comando, args);
            perror("Error al ejecutar comando final");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            wait(NULL);
            for (int i = 0; i < contador; i++) {
                free(argumentos[i]);
            }
        } else {
            perror("fork final falló");
            exit(EXIT_FAILURE);
        }
		free(args);
    }

    free(linea);
    return 0;
}
