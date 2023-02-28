#include "../../src/Specimen.hpp"
#include "../../src/SASEGASA.hpp"
#include <set>
#include <algorithm>
#include <map>

struct Client {
	std::set<size_t> likes;
	std::set<size_t> dislikes;
};

std::vector<std::string> allIngredients;
std::vector<Client> clients;

std::vector<size_t> ingInds;


class Pizza : public Specimen {
	public:
	std::vector<bool> ingredients;

	Pizza() {}
	Pizza(std::vector<bool> ingredients) : ingredients(ingredients) {}


	virtual std::shared_ptr<Specimen> createSpecimen(std::default_random_engine& engine) {
		static std::uniform_int_distribution<> dis(1, allIngredients.size());
		size_t N = dis(engine);
		std::vector<bool> newIngredients(allIngredients.size(), false);

		std::shuffle(ingInds.begin(), ingInds.end(), engine);
		for(size_t i = 0; i < N; ++i) {
			newIngredients[ingInds[i]] = true;
		}
		return std::shared_ptr<Specimen>(new Pizza(newIngredients));
	}


	virtual double evaluate() {
		double score = 0;
		for(auto &c : clients) {
			double satisfied = 0;
			double dissatisfied = 0;
			for(auto &ind : c.likes) {
				if(this->ingredients[ind]) {
					++satisfied;
				}
			}
			for(auto &ind : c.dislikes) {
				if(this->ingredients[ind]) {
					++dissatisfied;
				}
			}

			if(c.likes.size() == 0) {
				satisfied = 1;
			} else {
				satisfied = satisfied / c.likes.size();
			}
			if(c.dislikes.size() != 0) {
				dissatisfied = dissatisfied / c.dislikes.size();
			}
			//std::cout << (satisfied - dissatisfied) << "\n";
			//if(satisfied == 1 && dissatisfied == 0) {
			//	--score;
			//}
			//std::cout << satisfied << "\n";
			if(satisfied == 1 && dissatisfied == 0) {
				score = score - 1;
			} else {
				score = score - 0.0 * (1 - dissatisfied);
			}
			//score = score - (satisfied - dissatisfied);
		}
		return score;
	}

	virtual void mutate(std::default_random_engine& engine) {
		static std::uniform_real_distribution<double> unif(0, 1);
		
		if(unif(engine) < 0.3) { // mutation chance 30%
			for(size_t i = 0; i < this->ingredients.size(); ++i) {
				if(unif(engine) < 0.1) {
					this->ingredients[i] = !this->ingredients[i];
				}
			}
		}
	}

	virtual std::shared_ptr<Specimen> mate(std::shared_ptr<Specimen> other, std::default_random_engine& engine) {
		if(typeid(*this) != typeid(*other)) {
			std::cerr << "oepsie woepsie, a bit of a fucky wucky\n";
			return std::shared_ptr<Specimen>(this);
		}
		auto that = std::static_pointer_cast<Pizza>(other);
		
		static std::uniform_real_distribution<double> unif(0, 1);
		std::vector<bool> newIngredients(allIngredients.size(), false);
		
		for(size_t i = 0; i < this->ingredients.size(); ++i) {
			if(unif(engine) <= 0.65) {
				newIngredients[i] = this->ingredients[i];
			} else {
				newIngredients[i] = that->ingredients[i];
			}
		}

		return std::shared_ptr<Specimen>(new Pizza(newIngredients));
	}

	virtual void printSelf() {
		size_t count = 0;
		std::string ingString = " ";
		for(size_t i = 0; i < this->ingredients.size(); ++i) {
			if(this->ingredients[i]) {
				++count;
				ingString = ingString + " " + allIngredients[i];
			}
		}
		std::cout << count << ingString;

	}
};

double getSatisfied(std::vector<bool> solution) {
	double score = 0;
	for(auto &c : clients) {
		bool wontLike = false;
		for(auto &ind : c.likes) {
			if(solution[ind] == false) {
				wontLike = true;
				break;
			}
		}
		if(wontLike) continue;

		for(auto &ind : c.dislikes) {
			if(solution[ind] == true) {
				wontLike = true;
				break;
			}
		}
		if(wontLike) continue;
		score = score + 1;
	}
	return score;
}

int main() {

	int numOfClients;
	int numOfIngredients;
	std::string ingName;

	std::map<std::string, size_t> ingToIndex;

	std::cin >> numOfClients;
	for(size_t i = 0; i < numOfClients; ++i) {
		Client newClient;

		for(size_t ld = 0; ld < 2; ++ld){
			std::cin >> numOfIngredients;
			for(size_t j = 0; j < numOfIngredients; ++j) {
				std::cin >> ingName;
				size_t ingIndex;
				if(ingToIndex.find(ingName) == ingToIndex.end()) {
					ingIndex = allIngredients.size();
					allIngredients.push_back(ingName);
					ingToIndex[ingName] = ingIndex;
				} else {
					ingIndex = ingToIndex[ingName];
				}
				if(ld == 0) {
					newClient.likes.insert(ingIndex);
				} else {
					newClient.dislikes.insert(ingIndex);
				}
			}
		}
		clients.push_back(newClient);

	}

	for(size_t i = 0; i < allIngredients.size(); ++i) {
		ingInds.push_back(i);
	}

	//for(auto &c : clients) {
	//	std::cout << "Client:\n\tLikes:";
	//	for(auto &ind: c.likes) {
	//		std::cout << " " << allIngredients[ind];
	//	}
	//	std::cout << "\n\tDislikes:";
	//	for(auto &ind: c.dislikes) {
	//		std::cout << " " << allIngredients[ind];
	//	}
	//	std::cout << "\n";
	//	
	//}

	Specimen* prototype = new Pizza();
	SASEGASA alg(	500,	// popSize
					25,		// villageNum
					0.30,	// succChildren
					2,		// compFactor
					25,		// maxGeneratedChildrenFactor
					1);		// elitism
	alg.setVerbose(true);
	auto best = alg.execute(prototype);
	
	auto bestPizza = std::static_pointer_cast<Pizza>(best); 
	bestPizza->printSelf();

	std::cout << "\n\nSolution satisfies " << getSatisfied(bestPizza->ingredients) << " out of " << clients.size() << " clients.\n";



	return 0;
}