#include <stdio.h>

unsigned int reverse_int(unsigned int in) {
    unsigned int out = 0;
    // Devolver el entero invertido bit a bit
    for (unsigned int i = 0; i < 32; i++) {
        out = out << 1;  
        out |= in & 1;   
        in = in >> 1;    
    }
    return out;
}

int main() {
    unsigned int num = 1234;  // Número a invertir
    unsigned int reversed_num;

    // Llamar a la función reverse_int y obtener el número invertido
    reversed_num = reverse_int(num);

    // Truncar a las 4 últimas cifras
    unsigned int truncated_original = num % 10000;
    unsigned int truncated_reversed = reversed_num % 10000;

    // Imprimir los resultados truncados
    printf("Últimas 4 cifras do número orixinal: %04u\n", truncated_original);
    printf("Últimas 4 cifras do número con bits invertidos: %04u\n", truncated_reversed);

    return 0;
}