#include "../src/Specimen.hpp"
#include "../src/SASEGASA.hpp"

class DerivedSpecimen : public Specimen {
	public:
	double x;

	DerivedSpecimen() : x(100) {}
	DerivedSpecimen(double x) : x(x) {}

	virtual std::shared_ptr<Specimen> createSpecimen(std::default_random_engine& engine) {
		std::uniform_real_distribution<> dis(-10000, 10000);
		return std::shared_ptr<Specimen>(new DerivedSpecimen(dis(engine)));
	}

	virtual double evaluate() {
		return (sin(this->x) + 2) * (this->x) * (this->x);
	}

	virtual void mutate(std::default_random_engine& engine) {
		std::uniform_real_distribution<double> unif(0, 1);
		std::normal_distribution<> norm{0.0, 3.0};
		if(unif(engine) <= 0.3) {
			this->x + norm(engine);
		}
	}

	virtual std::shared_ptr<Specimen> mate(std::shared_ptr<Specimen> other, std::default_random_engine&) {
		if(typeid(*this) != typeid(*other)) {
			std::cout << "fucky wucky\n";
			return std::shared_ptr<Specimen>(this);
		}
		auto that = std::static_pointer_cast<DerivedSpecimen>(other);
		double newX = this->x + that->x;
		return std::shared_ptr<Specimen>(new DerivedSpecimen(newX));
	}

	virtual void printSelf() {
		std::cout << "f(" << this->x << ") = " << this->evaluate();
	}
};

int main() {

	Specimen* prototype = new DerivedSpecimen();
	SASEGASA alg;
	alg.execute(prototype);
	//Jedinka2 prototype({0, 0, 0, 0, 0, 0});
	//SASEGASA alg;
	//alg.execute(prototype);

	return 0;
}