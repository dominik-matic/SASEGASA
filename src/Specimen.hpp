#pragma once
#include "EvaluationResult.hpp"
#include <vector>
#include <random>
#include <memory>

class Specimen {
	private:
	public:
	virtual std::shared_ptr<Specimen> createSpecimen(std::default_random_engine&) = 0;
	virtual double evaluate() = 0;
	virtual void mutate(std::default_random_engine&) = 0;
	virtual std::shared_ptr<Specimen> mate(std::shared_ptr<Specimen> other, std::default_random_engine&) = 0;
	virtual void printSelf() = 0;

	std::vector<std::shared_ptr<Specimen>> createPop(size_t size, std::default_random_engine& engine) {
		std::vector<std::shared_ptr<Specimen>> pop;
		for(size_t i = 0; i < size; ++i) {
			pop.push_back(this->createSpecimen(engine));
		}
		return pop;
	}
};