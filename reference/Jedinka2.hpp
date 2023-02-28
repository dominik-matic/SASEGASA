#include <iostream>
#include <vector>
#include "Specimen.hpp"


class Jedinka2 : public Jedinka<std::vector<double>, double> {
	std::normal_distribution<double> d1{0.0, 0.9};
	std::normal_distribution<double> d2{0.0, 0.1};
	std::vector<std::vector<double>> eqs = {
		{-0.160,  2.320, -3.500, -2.660,  4.620, -171.36699979479872},
		{-0.870,  2.380, -2.770, -4.620,  2.350, -77.77052193414312},
		{-1.380, -4.120, -4.530,  2.480, -0.760, -37.81539134581623},
		{ 4.570,  0.050,  2.240,  2.300, -1.460, 66.49285583480545},
		{ 0.190, -1.200, -0.900, -4.930, -3.120, -142.29787708092266},
		{ 1.260, -0.520,  2.280, -4.750, -1.100, 12.049860325702245},
		{ 1.650,  3.510, -0.050,  0.560, -1.640, -29.538432212221274},
		{-1.400, -1.690, -0.470,  1.340, -2.030, -6.694032679028332},
		{-0.760,  1.300,  1.240,  0.020,  3.190, 10.81258978219227},
		{ 2.320, -1.010,  1.990, -0.820,  3.870, 20.501960308630863},
		{ 2.260, -3.440, -3.510, -2.830,  2.550, 79.76362779005981},
		{ 2.350,  2.500, -1.790,  2.450, -3.820, 26.865253294441743},
		{-4.140, -2.230, -1.220, -4.650,  4.290, -759.72550949069},
		{-0.040, -2.300, -3.980,  3.280, -2.600, 66.24113395811048},
		{ 3.340, -3.600,  4.540, -3.320, -4.460, 253.86877150367545},
		{-4.770,  4.610,  2.250,  1.050,  0.080, 1467.6191660121754},
		{-1.660, -2.550, -0.220, -3.030,  4.510, -231.41576698683608},
		{-0.940, -0.580,  3.370,  1.810, -2.510, 122.58468874880538},
		{ 4.100, -2.470, -2.600, -1.430,  1.020, 535.0289456402061},
		{-1.380, -3.780, -1.240, -1.150,  4.190, -100.00357415185884}
	};
	public:
	Jedinka2() {};
	Jedinka2(std::vector<double> x) {
		this->traits = x;
	}
	
	std::vector<std::shared_ptr<Jedinka<std::vector<double>, double>>> createPop(size_t size, std::default_random_engine& engine) {
		std::vector<std::shared_ptr<Jedinka<std::vector<double>, double>>> pop;
		std::uniform_real_distribution<double> unif(-5, 5);
		for(size_t i = 0; i < size; ++i) {
			std::vector<double> tmp = {unif(engine), unif(engine), unif(engine), unif(engine), unif(engine), unif(engine)};
			pop.push_back(std::make_shared<Jedinka2>(tmp));
		}
		return pop;
	}

	double evaluate() {
		double val = 0;
		double temp;
		for(auto &eq : eqs) {
			temp = 	this->traits[0] * eq[0] +		// ax_1
					this->traits[1] * eq[0] * eq[0] * eq[0] * eq[1] + //b*x_1^3*x_2 
					this->traits[2] * exp(this->traits[3] * eq[2]) * (1 + cos(this->traits[4] * eq[3])) +
					this->traits[5] * eq[3] * eq[4] * eq[4];
			val += (eq[eq.size() - 1] - temp) * (eq[eq.size() - 1] - temp);
		}
		return val / eqs.size();
	}
	
	double mutate(std::default_random_engine& generator) {
		std::uniform_real_distribution<double> unif(0, 1);
		for(size_t i = 0; i < this->traits.size(); ++i) {
			if(unif(generator) <= 0.3) {
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
		auto child = std::make_shared<Jedinka2>();
		std::uniform_real_distribution unif(0.0, 1.0);
		if(unif(generator) >= 0.5) {
			for(size_t i = 0; i < this->traits.size(); ++i) {
				double newTrait = (other->traits[i] + this->traits[i]) / 2;
				child->traits.push_back(newTrait);
			}
		} else {
			for(size_t i = 0; i < this->traits.size(); ++i) {
				if(unif(generator) >= 0.2) {
					double newTrait = (other->traits[i] + this->traits[i]) / 2;
					child->traits.push_back(newTrait);
				} else {
					child->traits.push_back(this->traits[i]);
				}
			}
		}
		return child;
	}



	
	virtual void printSelf() {
		std::cout << "( ";
		for(size_t i = 0; i < this->traits.size(); ++i) {
			std::cout << this->traits[i] << " ";
		}
		std::cout << ")";
	}
};