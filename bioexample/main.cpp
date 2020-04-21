#include <iostream>
#include "binario.h"

#define out std::cout
#define end std::endl


class A : public bio::BinarIObject {
public:
	int a = 5;
	float b = 0.5f;
	char* text = "Hallo";
	void serialize(bio::BinaryBuffer& buffer) {
		bio::serialize(buffer, a);
		bio::serialize(buffer, b);
		bio::serializeArray<char>(buffer, text, std::strlen(text) + 1);
	}

	void deserialize(bio::BinaryBuffer& buffer) {
		a = bio::deserialize<int>(buffer);
		b = bio::deserialize<float>(buffer);
		text = bio::deserializeArray<char>(buffer);
	}

	A() {};
};


int main() {
	out << "Example started" << end;

	bio::BinaryBuffer buffer(5000);

	A a;
	buffer.open(bio::MODE::WRITE, "test.test");
	a.serialize(buffer);
	buffer.close();

	buffer.open(bio::MODE::READ, "test.test");
	a.deserialize(buffer);
	buffer.close();

	out << a.a << ", " << a.b << ", " << a.text << end;

	out << "Example finished" << end;
}