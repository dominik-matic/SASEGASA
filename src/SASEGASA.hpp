#include <iostream>
#include <random>
#include "Specimen.hpp"

class SASEGASA{
	bool verbose = true;
	unsigned popSize;
	unsigned villageNum;
	float succChildren;
	float compFactor;
	unsigned maxGeneratedChildrenFactor;
	unsigned elitism;

	void evaluatePop(std::vector<std::shared_ptr<Specimen>>& pop, std::vector<double>& errors) {
		for(size_t i = 0; i < pop.size(); ++i) {
			errors.push_back(pop[i]->evaluate());
		}
	}

	// T-tournament selection
	std::pair<size_t, size_t> select(size_t t, std::vector<double>& errors, size_t start, size_t end, std::default_random_engine generator) {
		std::uniform_int_distribution<size_t> dist(start, end - 1);
		size_t parents[2];
		do {
			for(size_t i = 0; i < 2; ++i) {
				parents[i] = dist(generator);
				for(size_t j = 1; j < t; ++j) {
					size_t newParentInd = dist(generator);
					if(errors[newParentInd] < errors[parents[i]]) {
						parents[i] = newParentInd;
					}
				}
			}
		} while(parents[0] == parents[1]);

		if(errors[parents[0]] < errors[parents[1]]) {
			size_t temp = parents[1];
			parents[1] = parents[0];
			parents[0] = temp;
		}
		auto ret = std::make_pair<size_t, size_t>((size_t) parents[0], (size_t) parents[1]);
		return ret;
	}

	size_t findBest(std::vector<double>& errors, size_t start, size_t end) {
		size_t bestErrorInd = start;
		for(size_t i = start + 1; i < end; ++i) {
			if(errors[i] < errors[bestErrorInd]) {
				bestErrorInd = i;
			}
		}
		return bestErrorInd;
	}

	void printBest(std::vector<std::shared_ptr<Specimen>>& pop, std::vector<double>& errors, size_t start, size_t end) {
		size_t bestInd = findBest(errors, start, end);
		pop[bestInd]->printSelf();
		std::cout << "\terror = " << errors[bestInd] << "\t";
	}

	public:
	SASEGASA(unsigned popSize = 40,
			unsigned villageNum = 5,
			float succChildren = 0.4,
			float compFactor = 1,
			unsigned maxGeneratedChildrenFactor = 30,
			unsigned elitism = 1) :
			popSize(popSize),
			villageNum(villageNum),
			succChildren(succChildren),
			compFactor(compFactor),
			maxGeneratedChildrenFactor(maxGeneratedChildrenFactor),
			elitism(elitism) {}
	


	void setVerbose(bool v) { verbose = v; }

	std::shared_ptr<Specimen> execute(Specimen* prototype) {
		std::default_random_engine generator;
		generator.seed(time(0));

		// stvori inicijalnu populaciju i evaluiraj
		std::vector<std::shared_ptr<Specimen>> pop = prototype->createPop(popSize, generator);	
		std::vector<double> errors;
		evaluatePop(pop, errors);

		//for(size_t i = 0; i < pop.size(); ++i) {
		//	pop[i]->printSelf();
		//	std::cout << "\n";
		//}
		
		if(verbose) {
			std::cout << "Best at start:\n";
			printBest(pop, errors, 0, pop.size());
			std::cout << "\n";
		}
		
		
		/**
		 * glavna petlja
		 * Pseudokod:
		 * za svako selo dok ono ne konvergira ili dosegne maxIters:
		 * 		dok nema preko succChildren * villages[0].size() djece
		 * 			biraj
		 * 			križaj
		 * 			mutiraj
		 * 			ako je bolje od roditelja (compFactor) dodaj u selo
		 * 			ako nije dodaj u pool
		 * 		ako nema dovoljno succ children u pop, idi na drugo selo
		 * 		popuni ostatak sela iz poola
		 * smanji village size
		 * ako je village size = 0 toeto gotovo
		*/
		unsigned vNum = this->villageNum;
		unsigned popPerVillage;
		unsigned extra;
		while(true) {

			size_t start, end;
			popPerVillage = popSize / vNum;
			extra = popSize - popPerVillage * vNum;
			
			// za svako selo
			size_t currentSelo = 0;
			for(start = 0, end = popPerVillage; start < popSize; start = end, end = start + popPerVillage) {
				if(extra) { ++end; --extra; }
				std::vector<std::shared_ptr<Specimen>> newPop, pool;
				std::vector<double> newErrors, poolErrors;
				for(size_t j = 0; j < 10000; ++j) {
					newPop.clear();
					pool.clear();
					newErrors.clear();
					poolErrors.clear();

					// elitizam
					size_t bestErrInd = this->findBest(errors, start, end);
					newPop.push_back(pop[bestErrInd]);
					newErrors.push_back(errors[bestErrInd]);

					// biraj, krizaj, mutiraj
					size_t counter = 0;
					while(newPop.size() < (size_t) (this->succChildren * (end - start)) && counter < this->maxGeneratedChildrenFactor * (end-start)) {
						// biraj
						auto parents = this->select(2, errors, start, end, generator);
						size_t worseParentInd = parents.first;
						size_t betterParentInd = parents.second;
						
						// krizaj i mutiraj
						std::shared_ptr<Specimen> newChild = pop[betterParentInd]->mate(pop[worseParentInd], generator);
						newChild->mutate(generator);
						double newError = newChild->evaluate();
						
						// dodaj u novu djecu ako je bolji, inace u pool 
						if(newError < (errors[worseParentInd] - (errors[worseParentInd] - errors[betterParentInd]) * this->compFactor)) {
							newPop.push_back(newChild);
							newErrors.push_back(newError);
						} else {
							//std::cout << "Worse! ";
							//std::cout << errors[betterParentInd] << " " << errors[worseParentInd] << " " << newError << "\n";
							pool.push_back(newChild);
							poolErrors.push_back(newError);
						}
						++counter;
					}
					// ako se desila konvergencija, idi na iduce selo
					if(newPop.size() < (size_t) (this->succChildren * (end - start))) {
						if(verbose) std::cout << "Village " << ++currentSelo << " converged in " << j + 1 << " iterations. Best in village: ";
						break;
					}

					// popuni iz poola ostatak nove populacije
					for(size_t k = 0; k < pool.size(); ++k) {
						if(newPop.size() == (end - start)) {
							break;
						}
						newPop.push_back(pool[k]);
						newErrors.push_back(poolErrors[k]);
					}

					// ako u poolu nije bilo dovoljno ljudi, generiraj dovoljno
					while(newPop.size() < (end - start)) {
						auto parents = this->select(2, errors, start, end, generator);

						std::shared_ptr<Specimen> newChild = pop[parents.first]->mate(pop[parents.second], generator);
						newChild->mutate(generator);
						double newError = newChild->evaluate();
						newPop.push_back(newChild);
						newErrors.push_back(newError);
					}

					// zamijeni trenutnu populaciju s novom
					for(size_t k = start, l = 0; k < end; ++k, ++l) {
						pop[k] = newPop[l];
						errors[k] = newErrors[l];
					}

				}
				if(verbose) printBest(pop, errors, start, end);
				if(verbose) std::cout << "\n";
				
			} // end za svako selo
			if(verbose) {
				std::cout << "Best in all villages:\t";
				printBest(pop, errors, 0, pop.size()); std::cout << "\n";
			}
			if(--vNum == 0) {
				break;
			}
			if(verbose) {
				std::cout << "Moving on to " << vNum;
				if(vNum != 1) {
					std::cout << " villages\n";
				} else {
					std::cout << " village\n";
				}
			}
		}
		return pop[this->findBest(errors, 0, pop.size())];

	}

};