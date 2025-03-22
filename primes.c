#include <stdio.h>    // Para printf
#include <stdlib.h>   // Para exit, malloc, free, etc.
#include <unistd.h>   // Para fork, pipe, read, write, close
#include <sys/types.h> // Para pid_t
#include <sys/wait.h>  // Para wait()

void funcion_hijo(int pipe_abuelo_padre[]){
    
    int primo;
    if(read(pipe_abuelo_padre[0], &primo, sizeof(primo)) <= 0){
        close(pipe_abuelo_padre[0]);
        exit(0);
    }

    printf("primo %i\n", primo);

    int pipe_padre_hijo[2];

    pipe(pipe_padre_hijo);
    if(pipe_padre_hijo == -1){
        printf("error en el pipe\n");
        exit(0);
    }

    pid_t hijo = fork();

    if(hijo < 0){
        printf("error al crear el fork hijo");
        exit(0);
    }
    else if (hijo == 0)
    {
        /* hijo */
        close(pipe_padre_hijo[1]);
        funcion_hijo(pipe_padre_hijo);
        close(pipe_padre_hijo[0]);
        exit(0);
    }
    else
    {
        /* padre */
        close(pipe_padre_hijo[0]);
        int numero;
        while (read(pipe_abuelo_padre[0], &numero, sizeof(numero)) > 0)
        {
            if (numero % primo != 0)
            {
                write(pipe_padre_hijo[1], &numero, sizeof(numero));
            }
        }
        close(pipe_padre_hijo[1]);
        close(pipe_abuelo_padre[0]);
        wait(NULL);
        
    }
}


int main(int argc, char *argv[]){
	
    if (argc != 2){
        printf("Error en la cantidad de argunmentos");
        return 0;
    }

    int n = atoi(argv[1]); 

    if (n < 2){
        printf("ingrese un numero mayor o igual a 2");
        return 0;
    }
    else if(n > 10000){
        printf("ingrese un n menor a 10000");
        return 0;
    }

    int pipe_abuelo_padre[2];

    pipe(pipe_abuelo_padre);
    if(pipe_abuelo_padre == -1){
        printf("error en el pipe\n");
        exit(0);
    }

    pid_t hijo = fork();

    if(hijo < 0){
        printf("error al crear el fork");
        exit(0);
    }
    else if (hijo == 0)
    {
        /* padre */
        close(pipe_abuelo_padre[1]);
        funcion_hijo(pipe_abuelo_padre);
        close(pipe_abuelo_padre[0]);
        exit(0);
    }
    else
    {
        /* zbuelo */
        for(int i = 2; i < n; i++){
            write(pipe_abuelo_padre[1], &i, sizeof(i));
        }
        close(pipe_abuelo_padre[1]);
        wait(NULL);
        
    }
	return 0;
}