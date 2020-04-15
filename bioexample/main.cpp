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

	bio::BinaryBuffer buff;

	/* try {
		buff.open(bio::MODE::WRITE, "test.test");
	}catch(myexcept::myexcept& e){
		e(__func__, "Cannot open file");
		std::cout << e.get() << std::endl;
	} */

	for(int i = 0; i < 5000; i++){
		try{
			bio::serialize<int>(buff, i);
		}catch(myexcept::myexcept& e){
			e(__FUNCTION__, "Cannot serialize");
			std::cout << e.get() << std::endl;
			return -1;
		}
	}

	return 0;
}