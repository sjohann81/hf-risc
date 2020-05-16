# Trabalho Prático 1

### ***** WORK IN PROGRESS *****

Este trabalho consiste na implementação de um escalonador preemptivo. A ideia é utilizar o algoritmo Razão Monotonica para que se possa gerar as simulações e analisar resultados. Para realizar o trabalho será necessário definir um modelo de estados baseado em FSM (*Finite State Machine*) que permita a gerencia de aplicações multithread. Deve-se fazer o uso de recursos periféricos ao processador como temporizadores e tratadores também.


## Definições

Para a implementação existem alguns requisitos:

- Período para a troca de contexto deve ser entre 5ms e 50ms ```(Definiremos 30ms)```
- Criar estrutura de controle com os estados **READY**, **RUNNING** e **BLOCKED** (```Utilizaremos um vetor como estrutura por ser mais simples```)
- Fila de tarefas com funções de gerenciamentos (*gets* e *sets* de tarefas na fila) (```Utilizaremos um vetor como estrutura assim como a anterior```)
- Escolha do algoritmo de escalonamento (```O algoritmo utilizado será o de razão monotonica```)
- Temporizador, para geração de interrupções e troca de contexto (```Ainda estudando sobre o que utilizar ```)

## Setup do trabalho
Primeiro passo é compilar o simulador para isso segue os passos abaixo:

```
git clone https://github.com/pereiraguilherme/hf-risc.git
cd hf-risc/tools/sim/hf_riscv_sim/
make
```

Depois de compilar o processador nós podemos utilizar o processador para rodar algumas aplicações existentes dentro da pasta  ```app```, segue os passos para compilação e execução:

```
cd ~/hf-risc/software
make hello /*hello pode ser substituito por qualquer applicação existente no makefile*/

```
 O comando **make** vai gerar varios arquivos dentro da pasta, inclusive um **.bin** que será utilizado para executar a aplicação:
 ```
 ../tools/sim/hf_riscv_sim/hf_riscv_sim code.bin

 ```
