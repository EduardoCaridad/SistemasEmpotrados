
Práctica 5 - IPC
Requisitos de finalización
Apertura: martes, 8 de abril de 2025, 15:30
Cierre: martes, 29 de abril de 2025, 23:59

Fai unha implementación dun sistema multifío de produtores e consumidores, que se comuniquen mediante unha cola de mensaxes:
- os produtores van poñendo datos (empregaremos datos arbitrarios) na cola de mensaxes, empregando un pequeno delay para simular que lle leva un tempo a tarefa de obter cada dato. 
- os consumidores van recollendo cada dato da cola, simulando tamén un procesamento do mesmo con outro pequeno delay.
 
No LCD móstrase, nos dous díxitos da dereita o número actual de produtores e de consumidores (un díxito para mostrar cada un destes números), mentres que os dous díxitos da esquerda mostran a cantidade de datos pendentes na cola de mensaxes (>=0): cada vez que un produtor introduce un novo dato, aumenta (+1) esa conta, que diminue (-1) cada consumidor cando recolle un elemento para procesalo.
Ten que haber intercambio efectivo de datos entre os produtores e consumidores, aínda que sexan datos arbitrarios.
 
A nosa implementación empregará FreeRTOS ou outro RTOS se o preferides. Cos botóns poderás cambiar o número de produtores e consumidores: o botón esquerdo conmutará entre 0-1-2-3-4-5 produtores, e o botón dereito entre 0-1-2-3-4-5 consumidores.Todos os fíos terán a mesma prioridade.
