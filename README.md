# FreeRTOS

## Prise en main

1. Le fichier main.h et main.c se situent dans le dosseir Core, sous Inc et Src respectivement.

2. Les balises BEGIN et END indiquent l'emplacment des codes utilisateurs. En dehors, le code ne sera pas prit en compte.

## FreeRTOS, tâches et sémaphores

### Tâche simple

1. La taille du heap est utilisé pour allouer de la mémoire aux tâches. Selon les besoins du programme, la taille alloué peut etre augmenté. Par défault, 15 Ko sont alloés pour FREERTOS sur STM32F74. 

2. Le rôle de la macro portTICK_PERIOD_MS, définit à 1, est de définir la fréquence de FreeRTOS à 1 ms.
La fonction VTaskDelay attend en paramètre le nombre de tick afin d'effectuer une temporisation.
Donc, on va diviser le délais souhaité par la période du tick de l'OS. 
