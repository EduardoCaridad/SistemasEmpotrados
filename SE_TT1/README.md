Eduardo M. Caridad Cordeiro.



GEI-SE 614G010602425 |  Traballos Tutelados / Supervised Projects |  TT1. Conta de tempo / Timer


# Enunciado

Supervised Project 1. Timer

Traballo individual

Data de entrega: 22 de abril de 2025

Neste traballo tutelado tedes que mostrar como empregades algún dos temporizadores (timers) da placa para medir o paso do tempo en segundos.

Implementaredes un cronómetro que permita fixar un número de segundos de partida (dous díxitos, ss), e lanzar a conta atrás, empregando o dous díxitos menos significativos do LCD para mostrar en todo momento a conta, que para ao chegar a cero (queda pestanexando o 0 unha vez rematada a conta). O cronómetro ten que incorporar:

    Alarma: o voso cronómetro permite tamén introducir, antes de arrancar, un segundo número de dous díxitos, que logo se mostrará durante o conteo nos dous díxitos máis significativos do cronómetro (aa:ss). Cando a conta atrás co cronómetro alcanza ese valor (aa == ss), os dous LEDS da placa comezan a acender e apagar, simultaneamente, e xa non se deteñen ata o momento no que remata a conta atrás.
    Pausar/Reanudar: a posibilidade de pausar e reanudar a conta con algún dos botóns da placa.

Teredes que enviarme un correo para que vos asigne a cada un de vós o temporizador a empregar, das múltiples opcións dispoñibles na nosa placa.


## Temporizador empregado:

PIT (correo do 25.03.2025)



## Recursos encontrados e usados:

- Manual de referencia do SDK de NXP.
    - Capítulo 17: PIT
- Manual de referencia do MCU.
    - Sección 3.8.2: Timers: PIT Configuration.
    - Sección 32: PIT.

- (https://www.nxp.com/docs/en/supporting-information/Periodic-Interrupt-Timer-Training.pdf)
- (https://mcuxpresso.nxp.com/api_doc/dev/71/group__pit.html)
- (https://www.netburner.com/NBDocs/Developer/html/pitr__sem_8h_source.html)
- (https://community.silabs.com/s/article/periodic-interrupts-using-timers?language=en_US)
- (https://community.st.com/t5/stm32-mcus-products/periodic-interrupt-timer-with-dynamic-period-set/td-p/125387)
- (https://medium.com/swlh/arm-kl25-periodic-interrupt-timer-8d4b184f0088)
- (https://www.youtube.com/watch?v=47rhX7V94Cc)
- (https://www.youtube.com/watch?v=zmOvlNyN_6I)
- ChatGPT para explicacións 

