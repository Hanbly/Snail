#include "SNLpch.h"

#include "Snail.h"

class Example : public Snail::Application {
public:
	Example() {}
	~Example() {}

};

Snail::Application* Snail::CreateApplication()
{
	return new Example();
}