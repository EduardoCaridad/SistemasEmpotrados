# Práctica 4 - SE 24/25

# ENUNCIADO:

Nesta práctica veremos como empregar algo de código ensamblador nos nosos proxectos. En xeral, isto faise cando é preciso optimizar algunha parte do código respecto ao código máquina que o compilador proporciona, pero no noso caso imos facelo para un caso concreto e sinxelo.

Proporciona unha implementación en ensamblador para unha función que rote todos os bits dun número enteiro que recibe como parámetro: reverse_int(). Ofrece unha implementación que *ti* penses que pode ser máis eficiente que a ofrecida polo compilador cando traduce este código C co nivel de optimización -Ofast:

unsigned int reverse_int(unsigned int in)
{
  unsigned int out = 0;
  // Devolve o enteiro invertido bit a bit

  for (unsigned int i=0; i<32; i++) {
    out = out << 1;
    out |= in & 1;
    in = in >> 1;
  }

  return out;
}

Fai dúas implementacións desa función:

1. Función C con código ensamblador embebido no código C (inline ASM).

2. Función completa en ensamblador nun arquivo .s propio que haberá que ensamblar para obter o código obxecto que logo enlazar co resto.

Inclúe na entrega os arquivos .c e .s correspondentes, xunto a un arquivo de texto explicando a túa mellora sobre a versión ofrecida polo compilador. Non pasa nada se a túa mellora, ao final, non é máis eficiente, pero xustifica as túas decisións.


#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Tal y como se indica en el enunciado de la práctica, este proyecto contiene dos versiones del código: una versión no optimizada y otra optimizada (de código ensamblador).

## Estructura del proyecto

 **Makefile** permite realizar dos acciones principales:

    1. **`make flash_rev`**: Esta opción compila y flashea la versión **no optimizada** del código, correspondente a la implementación propuesta en la  práctica.

    2. **`make flash_opt`**: Esta opción compila y flashea la versión **optimizada** del código, que mejora la función de inversión de bits.


### Detalles de los programas

#### `reverse_int_main.c`

- Este programa se incluye en el Makefile para permitir que el usuario vea en la pantalla que el número invertido es procesado correctamente. Podemos editar el número a invertir (por defecto el 1234) y observar el valor resultante. Para eso editamos el fichero **reverse_int_main.c**.

- Muestra los últimos 4 dígitos del número tras ser invertido (aunque el código invierte todos los bits).

- Esta función utiliza el código proporcionado en la práctica para asegurar que la inversión de bits se realiza correctamente.

#### `main.c`

- Este fichero se encarga de **comprobar** que el código ensamblador, sea optimizado o no, funciona correctamente.

- La función que se llama mediante `extern` se ejecuta para mostrar el número invertido en el LCD de la placa, utilizando la librería da práctica `Prac3` del repositorio.

- A través de este programa se puede verificar que la optimización realizada funciona de manera efectiva, representando correctamente el número invertido en el LCD.

- Aquí también se cambiaría el valor del número que se desea invertir en caso de haberlo hecho en el fichero **reverse_int_main.c**, haciendo la edición correspondente en el fichero **main.c**.
  
### Justificación de la optimización

La optimización realizada en el código ensamblador busca mejorar la eficiencia, reduciendo el número de instruciones y haciendo un mejor uso de los registros. Esto minimiza la sobrecarga en las operaciones repetitivas del bucle, especialmente durante la inversión de bits. 

A continuación, detallamos las diferencias entre ambas versiones explicando la optimización del código.

---

#### **Versión no optimizada (`reverse_int.s`)**:

```assembly
	.cpu cortex-m0plus
	.arch armv6s-m
	.fpu softvfp
	.eabi_attribute 23, 1
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"reverse_int.c"
	.text
	.align	1
	.p2align 2,,3
	.global reverse_int
	.syntax unified
	.code 16
	.thumb_func
	.type reverse_int, %function

reverse_int:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, r0                 @ Mueve el valor de entrada (r0) al rexistro r3
	push	{r4, lr}               @ Guarda r4 y el link register (LR) en la pila
	movs	r2, #32                @ Establece un contador de 32 bits en r2
	movs	r0, #0                 @ Inicializa el valor de saída (r0) en 0
	movs	r4, #1                 @ Establece el valor 1 en r4 para la operación AND
.L2:
	movs	r1, r4                @ Carga 1 en r1 para la operación AND
	lsls	r0, r0, #1            @ Desplazamiento del valor de salída a la izquerda (multiplica por 2)
	ands	r1, r3                @ Aplica AND entre r3 (bit menos significativo de entrada) y r1
	subs	r2, r2, #1            @ Decrementa el contador de bits en r2
	orrs	r0, r1                @ OR lógico entre r0 y el resultado de la operación AND
	lsrs	r3, r3, #1            @ Desplazamiento de r3 (entrada) a la derecha para procesar el seguinte bit
	cmp	r2, #0                 @ Compara el contador con 0
	bne	.L2                    @ Se el contador no es 0, repite el bucle
	pop	{r4, pc}               @ Restaura r4 y salta a la dirección en el rexistro PC (termina la función)
	.size reverse_int, .-reverse_int
	.ident	"GCC: (Arm GNU Toolchain 13.3.Rel1 (Build arm-13.24)) 13.3.1 20240614
```
### Explicación do código non optimizado:

1. **Preparación**: 
   - Se inicialízan varios registros (r2 con el valor de 32, r0 con 0, r4 con 1) para las operaciones del bucle.
   
   - Se guarda el valor de r4 y el **Link Register** en la pila para su restauración posterior.

2. **Bucle**:
   - El bucle principal invierte los bits de `r3` (el valor de entrada), uno por uno, movéndolos a `r0` (valor de salída).
   
   - Cada iteración del bucle usa varias instruciones para operaciones básicas como la comparación (`cmp`), el desplazamiento (`lsls`, `lsrs`) y la manipulación de bits (`ands`, `orrs`).
   
   - En el bucle, el compilador con -Ofast utiliza la técnica de reordenamiento de instruciones, moviendo la instrucción `subs r2, r2, #1` entre la instrucción `ands r1, r3` e `orrs r0, r1` seguramente buscando evitar una dependencia entre instruciones.
   
   - Al final de cada iteración, el contador (`r2`) se decrementa, y el bucle se repíte hasta que `r2` llega a 0.

3. **Lógica adicional**:
   - Se usa un registro temporal (`r1`) para realizar la operación AND, lo que introduce una instrucción adicional innecesaria.

---
#### **Versión optimizada (`reverse_int_opt.s`)**:

```assembly
	.cpu cortex-m0plus
	.arch armv6s-m
	.fpu softvfp
	.eabi_attribute 23, 1
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"reverse_int.c"
	.text
	.align	1
	.p2align 2,,3
	.global reverse_int
	.syntax unified
	.code 16
	.thumb_func
	.type reverse_int, %function

reverse_int:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, r0                 @ Mueve el valor de entrada (r0) al rexistro r3
	push	{r4, lr}               @ Guarda r4 y el link register (LR) en la pila
	movs	r2, #32                @ Establece un contador de 32 bits en r2
	movs	r0, #0                 @ Inicializa el valor de saída (r0) en 0
	movs	r4, #1                 @ Establece el valor 1 en r4 para la operación AND
.L2:
	lsls	r0, r0, #1            @ Desplazamiento del valor de salída a la izquierda (multiplica por 2)
	movs	r1, r3                @ Mueve el valor de r3 a r1 para la operación AND
	ands	r1, r4                @ Aplica AND entre r3 (bit menos significativo de entrada) y r4
	orrs	r0, r1                @ OR lóxico entre r0 y el resultado de la operación AND
	lsrs	r3, r3, #1            @ Desplazamiento de r3 (entrada) a la derecha para procesar el seguinte bit
	subs	r2, r2, #1            @ Decrementa el contador de bits en r2
	bne	.L2                    @ Si el contador no es 0, repite el bucle
	pop	{r4, pc}               @ Restaura r4 y salta a la dirección en el registro PC (termina la función)
	.size reverse_int, .-reverse_int
	.ident	"GCC: (Arm GNU Toolchain 13.3.Rel1 (Build arm-13.24)) 13.3.1 20240614
```
### Explicación do código optimizado:

1. **Reducción de instruciones**:
   - La operación que antes precisaba de un registro temporal (`movs r1, r4`) se optimizó al eliminar el registro adicional y realizar la operación directamente sobre el valor necesario (`movs r1, r3`).
   
   - Esto reduce el uso innecesario de operaciones adicionales y hace que el bucle sea más compacto y eficiente.

2. **Manejo más directo de los registros**:
   - Se evitó el uso de una instrución de más para cargar un valor constante en `r1`. En lugar de hacerlo en cada iteración del bucle, simplemente se carga el valor directamente desde `r3`, y que reduce la sobrecarga.

3. **Operaciones más directas**:
   - Las operaciones AND e OR se aplícan de forma más directa, optimizando la manipulación de bits.
   - El resto del bucle se mantiene igual, pero el código más limpio reduce el número de instruciones y el uso innecesario de registros.


