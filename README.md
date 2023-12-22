**Questões feitas**: [1 - 15] (11 só pela metade)

**1. Com suas próprias palavras, explique porque programação paralela deixou de ser uma alternativa à computação sequencial convencional.**

> Os chips foram evoluindo e ficando com cada vez mais núcleos, possibilitando assim a execução de software em paralelo. A computação sequencial deixou de ser uma alternativa devido ao baixo desempenho em processadores mainstream, que hoje são multicore, e também trazendo uma possível má gestão de processos em sistemas operacionais modernos projetados visando o processamento multithread, e até não sendo possível o uso em sistemas operacionais mais modernos como no Windows 11. Dessa forma a programação multicore passou a ser quase que obrigatória nos dias atuais.

**2. Explique, na sua opinião, se as modificações feitas à máquina de Von Neuman, como cache e memória virtual, alteram a sua classificação na taxonomia de Flynn e porque. Estenda sua resposta a pipelines, múltiplas issues e multithreding de hardware.**
> Não alteram pelo fato de que cache e memória virtual são abstrações da memória, que são vistas pela máquina de Von Neuman apenas como uma simples memória. Do ponto de vista do programa, a vinda da cache por exemplo, é sentida apenas em termos de velocidade de leitura e escrita, sendo possíveis devido aos algoritmos de cache que funcionam dentro da memória.
> A pipeline teoricamente pode alterar taxonomia de Flynn pois mais de um dado é processado por vez, no decorrer da pipeline. No multiple issues também, pois seriam multiplas instruções a serem inicializadas em um único ciclo de clock. Já o multithreading de hardware, acredito que não há mudança na taxonomia de Flynn pois o mesmo trabalho é alternado entre as threads.

**3. Dê dois exemplos de arquiteturas paralelas, uma do tipo MIMD e outra do tipo SIMD, e explique suas principais diferenças no que diz respeito à forma de processamento paralelo.**
> Cluster para MIMD e GPU para SIMD.
> As GPUs por exemplo recebem uma instrução que é realizada em grandes vetores de dados, sendo processado cada pequena parte em seus pequenos núcleos, que as GPUs tendem a ter vários, bem mais que processadores convencionais. Já os clusters que são exemplos de MIMD funcionam processando diversas instruções atuando em diversos dados, caracterizando assim sua grande utilidade de rodar diversos de serviços de software em uma única grande máquina.

**4. Explique como o uso de uma variável compartilhada em um programa com múltiplos threads em uma arquitetura de memória compartilhada com múltiplas caches privadas pode deteriorar o desempenho paralelo do programa.**

> Variável compartilhada não é muito desejável nos programas pois a cópia que cada thread terá da variável compartilhada, deve ser sempre atualizada, para não correr o risco de haver leitura incorreta dos dados.Tendo muitas caches privadas, e essas caches tiverem que ser atualizadas muitas vezes, ocorrerá uma perda significativa de desempenho no programa.

**5. Explique os conceitos de localidade temporal e espacial no contexto de caches e memória virtual e exemplifique como o programador pode evitar armadilhas de desempenho com o conhecimento desses conceitos.**
> Geralmente regiões de memória tendem a ser acessadas repetidamente e também sequencialmente, simplesmente pela natureza de boa parte dos algoritmos. 
> Para se beneficiar disso, uma boa estratégia é acessar sempre que possível, a memória dessa forma. Sabendo o tamanho da cache teríamos uma noção de até que ponto seria interessante quebrar o problema em partes menores que pudessem ser resolvidos utilizando a memória que cabe na cache. 
> Já para o conceito de memória virtual, o problema é semelhante, porém quando se trata de problemas que utilizam muita memória, em geral, mais memória que a memória principal do computador, onde se caso não for o suficiente, o sistema operacional estende a memória do programa para utilizar memória de disco, o que causa lentidão. Uma boa prática seria particionar o problema para que os pedaços menores consigam rodar utilizando apenas a memória principal.

**6. Falso compartilhamento pode ser tão prejudicial ao desempenho de um programa paralelo quanto o que foi exposto na questão 4. Como isso ocorre e como pode ser evitado?**
>O falso compartilhamento traz o risco do comportamento indesejado do programa, podendo causar muitas vezes a sua falha em execução. A razão disso é que as threads devem ter suas memórias individuais protegidas, e as memórias compartilhadas terem a sua sincronia na hora de ler e escrever, sendo feita corretamente.

**7. O que é a lei de Amdahl e como ela se relaciona com a lei de Gustafson?**
A lei de Amdahl é uma lei que define o speedup para medir o impacto da paralelização de um programa inicialmente serial.
$speedup = \frac{1}{(1 - p) + \frac{P}{N}}$

<img src="https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Frits.github-pages.ucl.ac.uk%2Fintro-hpchtc%2Fmorea%2Flesson2%2Fimages%2Famdahl.png&f=1&nofb=1&ipt=85bd9c7c0a90f388883170b6d714892eb712f2656de0590d979bfccd85be9a82&ipo=images">

Já a lei de Gustafson busca um speedup linear a medida que aumentamos a quantidade de processadores.
Sendo:
$speedup = P + (1 - P) * N$

<img src="https://i.imgur.com/oYF4OsQ.png">

O desafio da paralelização pode ser abordado a partir de duas perspectivas distintas. Uma delas, mais pessimista ou realista (como a visão de Amdahl), destaca as limitações intrínsecas do paralelismo. Essa abordagem busca compreender de maneira mais crítica as restrições que o paralelismo pode impor.

Por outro lado, há uma perspectiva mais otimista, que sugere que ao aumentarmos tanto o tamanho do problema quanto o número de processadores, a relação entre eles tenderia a ser quase linear. Essa visão otimista defende que, ao escalar tanto o tamanho da tarefa quanto os recursos de processamento, podemos alcançar um aumento proporcional na eficiência do paralelismo.

**8. Como se calcula a eficácia de um programa paralelo?**

A efetividade de um programa executado em paralelo pode ser expressa por meio de diversas medidas, sendo as principais o Speedup e a eficiência. **Speedup**: analisa o desempenho do programa ao ser executado em um único núcleo e em P núcleos simultaneamente, seguindo a fórmula: 
$Speedup (S) = \frac{T1}{TP}$ 
Aqui, T1 representa o tempo de execução em um único núcleo, e TP representa o tempo de execução em P núcleos.

**Eficiência**: avalia quão eficazmente os processadores estão sendo utilizados, essencialmente representando o Speedup normalizado pelo número de processadores.
 $Eficiência (E) = \frac{S}{P}$ Nessa fórmula, S é o Speedup e P é o número de processadores.

**9. Quando se mede intervalos de tempo de execução de um programa paralelo, é aconselhável realizar mais de uma medição. É comum se pensar na média como cálculo de agregação desses valores, mas há argumentos em favor do uso da mediana. Quais não esses argumentos? Por outro lado, quando se mede os tempos de uma região paralela do programa, cada thread ou processo tem sua própria medição e, neste caso, nem a média nem a mediana são adequadas, por quê? O que é mais adequado?**
O uso da mediana se apresenta melhor pois lida com outliers. No mundo da computação que é essencialmente experimental, erros acontecem por diversos motivos, e devido a isso o uso da mediana se torna mais confiável pois a média ainda pode ser influenciada por valores que na realidade deveriam ser descartados.
Já em programas paralelos, a mediana pode mascarar esse desbalanceamento de carga. A abordagem mais interessante para metrificar o tempo de programas paralelos seria medir o tempo na região crítica (tempo de execução da thread ou processo mais lento) que normalmente determina o tempo total de execução do programa paralelo, ou seja pegar o maior tempo entre as threads.

**10. Como é possível determinar a escalabilidade de um programa paralelo? O que faz um programa ser escalável, fracamente escalável ou fortemente escalável?**
>Para um programa ser **escalável**, ele deve manter a mesma eficiência e speedup, conforme aumentarmos o tamanho do problema, ou seja, quanto mais o problema cresce, o programa vai ficando mais rápido para compensar o crescimento do problema.
>Uma **escalabilidade fraca**, significa que o programa consegue resolver problemas de tamanhos maiores, mais rapidamente, quando aumentamos o número de processadores. Nesse caso o speedup do progama teria que aumentar juntamente com o aumento do número de processadores e do tamanho do problema.
>Já a **escalabilidade forte**, significa que se simplesmente aumentarmos o número de processadores, e mantermos o tamanho do problema fixo, a velocidade do programa aumenta juntamente com o número de processadores. 


**11. (Peso 2) Explique o passo a passo da metodologia de Foster aplicada ao programa histogram.c para chegar na Figura 2.22 do livro texto e implemente a paralelização equivalente utilizando OpenMP. (página 65)**

>A metodologia de Foster diz:
>1. Particionamento
>2. Comunicação
>3. Agregação
>4. Mapeamento
>
>O programa histogram.c essencialmente deve criar intervalos de valores, e ir contando quantos elementos estão em cada um dos intervalos.
>A metodologia de Foster inicialmente pede para que o programa seja particionado em etapas, sendo elas:
>1. encontrar os intervalos
>2. encontrar os máximos de cada intervalo
>3. realizar a contagem de cada valor em cada intervalo

**12. Download omp_trap_1.c from the books's website, and delete the critical directive. Now compile and run the program with more and more threads and larger and larger values of n. How many threads and how many trapezoids does it take before the result is incorrect?**
> Em meus experimentos, com o aumento do número de threads, e valores de n  pequenos, o resultado permaneceu o mesmo, porém quando aumentado o n, sendo calculado com mais de 1 thread, o resultado ficou ligeiramente diferente.
> Para as entradas de a=0, b=1000, n=32, o resultado foi o mesmo para número de threads indo de 1 até 16. Já para as entradas a=0, b=1000, n=3200, para um número de threads maior que 1, o resultado foi um pouco diferente, variando apenas nas últimas casas decimais.
> Como a diretiva **critical** garante que só uma thread consiga escrever por  vez, não há perigo de mais de uma thread incrementarem o valor do resultado (fazendo que uma das threads incremente somando com o valor do resultado global desatualizado).

**13. In our first attempt to parallelize the program for estimating $\pi$, our program was incorrect. In fact, we used the result of the program when it was run with one thread as evidence that the program  run with two threads was incorrect. Explain why we could "trust"the result of the program when it was run with one thread.**

> Na primeira tentativa de paralelização do programa para estimar $\pi$ proposto pelo livro, que utilizou a fórmula $\pi = 4 \sum_{n=0}^{\infty} \frac{(-1)^n}{2n+1}$, com o código:
> ```cpp
> double factor = 1.0;
> double sum = 0.0;
> # pragma omp parallel for reduction(+:sum)
> for (int k=0; k<n; k++) {
>    sum += factor/(2*k+1);
>    factor = -factor;
>}
>pi_approx = 4*sum;
> ```
> Há um problema relacionado ao fator, que está recebendo o seu valor anterior, o que de forma serial não teria problema, mas quando o programa tem mais de uma linha de execução, todas as threads receberiam o mesmo factor, o factor que na verdade deveria ser da soma de índice zero apenas.
> Para arrumar isso, bastaria definir previamente todos os fatores, ou então chegar a paridade do índice dentro do loop for.

**14. Consider the loop**
```cpp
a[0] = 0;
for (int i=1; i<n; i++) {
	a[i] = a[i-1] + i;
}
```
**There's clearly a loop-caried dependence, as the value of a[i] can't be computed without the value of a[i-1]. Can you see a way to eliminate this dependence and parallelize the loop?**

Sim. A expressão ```a[i] = a[i-1] + i```, com ```a[0] = 0```, é equivalente  a ```a[i] = i * (i + 1) / 2;``` para i >= 1.
Sendo assim, com a expressão dependendo apenas do índice i, o novo loop pode ser paralelizado tranquilamente.

**15. Questão 5.10 do livro texto.**
A  questão pede para que implemente o o test mostrado com a diretiva critical e verificar a perda de desempenho.
```cpp
#   pragma omp parallel
    {
        double my_sum = 0.0;
        for (int i = 0; i<n; i++)
            #pragma omp critical
            my_sum += sin(i);
    }
```
<img src="https://i.imgur.com/a8189eA.png">

O programa utilizando 16 threads foi bem pior devido ao problema apontado na questão, onde devido a diretiva **critical**, as threads executarão uma por vez,  causando muito tempo ocioso.
Na questão também é perguntado  se com o uso da diretiva **atomic**, o código possibilita a execução de updates simultâneos de diferentes variáveis, caso elas estejam protegidas pela diretiva **atomic**. A resposta é não, pois com a diretiva **atomic** não deixa outras threads interromperem o trabalho da thread em questão que está sendo executada. No caso os updates simultâneos em uma variável não acontecerão.