# TSSOO-taller03

Cristian Valencia Avila - cristian.valenciaa@alumnos.uv.cl


# 1. Introducción 
En el desarrollo del siguiente taller al igual que se hizo en el taller02, se realiza el llenado de un arreglo y además la suma respectiva de los valores guardados en el arreglo dinámico cque se crea en el primer módulo, en este caso se pide como requisito la implementación con OpenMP para ambos modulos. Los dos módulos son los siguientes:
1.- Modulo de llenado de arreglo 
2.- Modulo de suma del arreglo
Para realizar pruebas de desempeño para dichos módulos, se realizan una serie de pruebas en una máquina virtual y a la vez en el computador anfitrión de las cuales se saca el promedio de los tiempos de ejecución del proceso de cada módulo. Estos procesos se implementaran con OpenMP, y con esto se calculará su tiempo de ejecución, para tener una visualización del desempeño de cada modulo para realizar una comparativa con lo realizado en el taller02.
 
# 2. Diseño de la Solución

El taller posee dos módulos principales los cuales son el llenado de un arreglo dinámico y la suma de los elementos del arreglo ambos procesos de manera paralela.
El modelado de la solución de los módulos es el siguiente:

![Diagrama solución](http://imgfz.com/i/pRj1LuH.png)

El diagrama  presentado corresponde al mismo propuesto para la solución del taller02, ya que se pide exactamente lo mismo, con la diferencia de la implementación, exigiendose en este usar OpenMP. A continuación se explica cada modulo por separado.

### 2.1 Modulo 1 

En primer lugar se crea un arreglo, el cual es llenado con datos de forma aleatoria en la siguiente sección:
```
auto vectorL = new uint64_t[totalElementos];
	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<uint32_t> nRandom(limInf, limSup);
	start = std::chrono::system_clock::now();
	#pragma omp parallel for  num_threads(numThreads)
	for(size_t i = 0; i < totalElementos; ++i){	
		vectorL[i] = nRandom(rng);
	}
	end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> duracionLlenadoOml = end - start;
	auto tiempoArrayOpenMP = duracionLlenadoOml.count();
```


### 2.2 Modulo 2

En el siguiente módulo se realiza el siguiente proceso, el cual corresponde a la suma de los elementos guardados en el arreglo creado en el módulo 1. Como se puede ver en la siguiente sección: 

```
uint64_t acumulador = 0;
	start = std::chrono::high_resolution_clock::now();	
	#pragma omp parallel for reduction(+:acumulador) num_threads(numThreads)
	for(size_t i = 0; i < totalElementos; ++i){
		acumulador += vectorL[i];
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoSumaOpenMP = elapsed.count();
```
