#pragma once

#include "IEnumerator.h"

#include <vector>

class CoroutineManager
{
public:
	static CoroutineManager& GetInstance();

	void Start(IEnumerator&& routine);
	void Update();

private:
	std::vector<IEnumerator> coroutines;
};