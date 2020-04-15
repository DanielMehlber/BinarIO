#include <iostream>
#include "binario.h"

#define out std::cout
#define end std::endl


class A : public bio::BinarIObject {
public:
	int a = 5;
	float b = 0.5f;
	char* text = "Hallo";
	double arr[5] = {1, 2, 3, 4, 5};
	void serialize(bio::BinaryBuffer& buffer) {
	}

	void deserialize(bio::BinaryBuffer& buffer) {
	}

	A() {};
};


int main() {
	out << "Example started" << end;

	int i = 5;

	bio::BinaryBuffer buffer;
	try
	{
		buffer.open(bio::MODE::WRITE, "test.test");
	}
	catch(myexcept::myexcept& e)
	{
		e(__FUNCTION__, "Cannot open file to write");
		std::cerr << e << std::endl;
	}
	
	

	try
	{
		bio::serialize<int>(buffer, i);
	}
	catch(myexcept::myexcept& e)
	{
		e(__FUNCTION__, "Cannot serialize data");
		std::cerr << e << std::endl;
	}
	
	buffer.close();

	try{
		buffer.open(bio::MODE::READ, "test.test");
	}catch(myexcept::myexcept& e){
		e(__FUNCTION__, "Cannot open file to read");
		std::cerr << e << std::endl;	
	}

	try
	{
		i = bio::deserialize<int>(buffer);
	}
	catch(myexcept::myexcept& e)
	{
		e(__FUNCTION__, "Cannot deserialize data");
		std::cerr << e << '\n';
	}

	out << i << end;
	

	buffer.close();

	out << "Example finished" << end;
}