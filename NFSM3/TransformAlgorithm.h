#pragma once

#include "State.h"
#include <memory>

class TransformAlgorithm {
public:
	virtual StateCouple transform() = 0;
	virtual std::wstring get_log() const = 0;
	virtual std::shared_ptr<State> get_states() const = 0;
	virtual int get_number_of_states() const = 0;
	virtual ~TransformAlgorithm() {};
};
