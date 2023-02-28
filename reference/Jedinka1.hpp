#include <iostream>
#include <vector>
#include "Specimen.hpp"


class Jedinka1 : public Jedinka<std::vector<double>, double> {
	public:
	Jedinka1(std::vector<double> x) {
		this->traits = x;
	}
	
	std::vector<std::shared_ptr<Jedinka<std::vector<double>, double>>> createPop(size_t size, std::default_random_engine& engine) {
		std::vector<std::shared_ptr<Jedinka<std::vector<double>, double>>> pop;
		std::uniform_real_distribution<double> unif(-5, 5);
		for(size_t i = 0; i < size; ++i) {
			std::vector<double> tmp = {unif(engine), unif(engine)};
			pop.push_back(std::make_shared<Jedinka1>(tmp));
		}
		return pop;
	}

	double evaluate() {
		return this->traits[0] * this->traits[0] + (this->traits[1] - 1) * (this->traits[1] - 1);
	}
	
	double mutate(std::default_random_engine& generator) {
		std::uniform_real_distribution unif(0.0, 1.0);
		std::normal_distribution<double> d1{0.0, 0.9};
		std::normal_distribution<double> d2{0.0, 0.1};

		for(size_t i = 0; i < this->traits.size(); ++i) {
			if(unif(generator) >= 0.3) {
				if(unif(generator) >= 0.5) {
					this->traits[i] += d1(generator);
				} else {
					this->traits[i] += d2(generator);
				}
			}
		}
		return this->evaluate();
	}

	std::shared_ptr<Jedinka<std::vector<double>, double>> mate(std::shared_ptr<Jedinka<std::vector<double>, double>> other, std::default_random_engine& generator) {
		std::uniform_real_distribution unif(0.0, 1.0);
		std::vector<double> newTraits;
		if(unif(generator) >= 0.5) {
			for(size_t i = 0; i < this->traits.size(); ++i) {
				double newTrait = (other->traits[i] + this->traits[i]) / 2;
				newTraits.push_back(newTrait);
			}
		} else {
			for(size_t i = 0; i < this->traits.size(); ++i) {
				if(unif(generator) >= 0.2) {
					double newTrait = (other->traits[i] + this->traits[i]) / 2;
					newTraits.push_back(newTrait);
				} else {
					newTraits.push_back(this->traits[i]);
				}
			}
		}
		return std::make_shared<Jedinka1>(newTraits);
	}



	
	virtual void printSelf() {
		std::cout << "( ";
		for(size_t i = 0; i < this->traits.size(); ++i) {
			std::cout << this->traits[i] << " ";
		}
		std::cout << ")";
	}
};