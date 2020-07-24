#include <global.hh>
#include <checkArgs.hpp>
uint64_t sumasS = 0;
uint64_t *sumasP = nullptr;
uint64_t *arreglo_c = nullptr;
uint64_t *arregloEnParalelo = nullptr;
uint64_t *arregloEnSerie = nullptr;

void sumaSerie(size_t pos,
				size_t beginIndex,
				size_t endIndex)
{
	for (size_t i = beginIndex; i < endIndex; ++i)
	{
		sumasS += arregloEnParalelo[i];
	}
}
void sumaParalela(size_t pos,
				  size_t beginIndex,
				  size_t endIndex)
{
	for (size_t i = beginIndex; i < endIndex; ++i)
	{
		sumasP[pos] += arregloEnParalelo[i];
	}
}

void LlenadoDelArreglo(size_t beginIdx, size_t endIdx, uint32_t limiteInferior, uint32_t limiteSuperior, size_t Rnetype)
{
	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<uint32_t> nRandom(limiteInferior, limiteSuperior);
	for (size_t i = beginIdx; i < endIdx; ++i)
	{
		uint32_t random = nRandom(rng);
		switch (Rnetype)
		{
		case 0:
			arregloEnParalelo[i] = random;

			break;

		case 1:
			arregloEnSerie[i] = random;
			break;

		}
	}
}

int main(int argc, char **argv)
{

	uint64_t totalElementos;
	uint64_t numThreads;
	uint32_t limInf;
	uint32_t limSup;
	uint64_t resultadoSP = 0;
	std::vector<std::thread *> threads;
	std::vector<std::thread *> threads2;

	auto argumentos = (std::shared_ptr<checkArgs>)new checkArgs(argc, argv);

	totalElementos = argumentos->getArgs().tamProblema;
	numThreads = argumentos->getArgs().numThreads;
	limInf = argumentos->getArgs().limInferior;
	limSup = argumentos->getArgs().limSuperior;

//Llenado del arreglo de forma secuencial

	arregloEnSerie = new uint64_t[totalElementos];
	auto start = std::chrono::high_resolution_clock::now();
	LlenadoDelArreglo(0, totalElementos, limInf, limSup, 1);
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoArrayS = elapsed.count();

//Llenado del arreglo de forma paralela

	arregloEnParalelo = new uint64_t[totalElementos];
	for (size_t i = 0; i < numThreads; ++i)
	{
		threads.push_back(new std::thread(LlenadoDelArreglo,
										  i * (totalElementos) / numThreads,
										  (i + 1) * (totalElementos) / numThreads, limInf, limSup, 0));
	}
//Tiempo de llenado de forma paralela

	start = std::chrono::system_clock::now();
	for (auto &thrs : threads)
	{
		thrs->join();
	}
	end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> duracionLlenado = end - start;
	auto tiempoArrayP = duracionLlenado.count();

//Suma de Threads

	sumasP = new uint64_t[totalElementos];
	for (size_t i = 0; i < numThreads; ++i)
	{

		threads2.push_back(new std::thread(sumaParalela,
										   i,
										   i * (totalElementos) / numThreads,
										   (i + 1) * (totalElementos) / numThreads));
	}

//Tiempo de suma de los Threads

	start = std::chrono::system_clock::now();
	for (auto &thr : threads2)
	{
		thr->join();
	}
	for (size_t i = 0; i < numThreads; i++)
	{
		resultadoSP += sumasP[i];
	}

	end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli> duracionSuma = end - start;
	auto tiempoSumaP = duracionSuma.count();

//Tiempo de sumaSecuencial

	start = std::chrono::high_resolution_clock::now();
	sumaSerie(0,0,totalElementos);
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoSumaS = elapsed.count();



//Llenado OpenMP
	
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

//Tiempo Suma OpenMP

	uint64_t acumulador = 0;
	start = std::chrono::high_resolution_clock::now();	
	#pragma omp parallel for reduction(+:acumulador) num_threads(numThreads)
	for(size_t i = 0; i < totalElementos; ++i){
		acumulador += vectorL[i];
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoSumaOpenMP = elapsed.count();





	std::cout << "=======LLenado y Suma de numeros de un Arreglo======= " << std::endl;
	
	std::cout << "Tiempo de LLenado secuencial: " << tiempoArrayS << "[ms]" << std::endl;
	std::cout << "Tiempo de LLenado con threads: " << tiempoArrayP << "[ms]" << std::endl;
	std::cout << "Tiempo de LLenado con OpenMP: " << tiempoArrayOpenMP << "[ms]" << std::endl;
	std::cout << "SpeedUp de llenado(s/p): " << (double) tiempoArrayS/tiempoArrayP << std::endl;
	std::cout << "SpeedUp de llenado(s/o): " << (double) tiempoArrayS/tiempoArrayOpenMP << std::endl;

	std::cout << "================================================= " << std::endl;
	
	std::cout << "suma secuencial: " << sumasS << std::endl;
	std::cout << "suma con threads: " << resultadoSP << std::endl;
	std::cout << "suma con OpenMP: " << acumulador << std::endl;
	
	std::cout << "================================================= " << std::endl;

	std::cout << "Tiempo de suma secuencial: " << tiempoSumaS << "[ms]"<< std::endl;
	std::cout << "Tiempo de suma con Threads: " << tiempoSumaP << "[ms]"<< std::endl;
	std::cout << "Tiempo de suma con OpenMP: " << tiempoSumaOpenMP << "[ms]"<< std::endl;
	std::cout << "SpeedUp de suma(s/p): " << (double) tiempoSumaS/tiempoSumaP << std::endl;
	std::cout << "SpeedUp de suma(s/o): " << (double) tiempoSumaS/tiempoSumaOpenMP << std::endl;
	

	return(EXIT_SUCCESS);
}

