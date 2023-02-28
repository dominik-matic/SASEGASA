

class EvaluationResult {
	protected:
	virtual bool cmp_lt(EvaluationResult const& other) const = 0;
	
	public:
	bool operator<(EvaluationResult const& other) const {
		return this->cmp_lt(other);
	}
};