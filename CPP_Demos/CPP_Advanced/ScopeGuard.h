#pragma once
//���˴����ŵ�����������
#include "boost/align.hpp"
#include "boost/cast.hpp"

#include <iostream>
class ScopeGuard
{
public:
	template<class Callable>
	ScopeGuard(Callable&& fn) :fn_(std::forward<Callable>(fn)) {}
	ScopeGuard(ScopeGuard&& other) :fn_(std::move(other.fn_)) { other.fn_ = nullptr; }

	~ScopeGuard() {
		//?must?not?throw
		if (fn_)fn_();

	}

	ScopeGuard(const ScopeGuard&) = delete;

	void operator = (const ScopeGuard&) = delete;

private:
	std::function<void()>   fn_;
};

typedef struct struct_a {
	int a;
	double b;
}struct_a;

#define CONCAT_(a,b) a##b
#define CONCAT(a,b) CONCAT_(a,b)
#define DEFER(fn) ScopeGuard CONCAT(__defer__, __LINE__) = [&](){fn;}
