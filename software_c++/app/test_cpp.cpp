/*
 * check https://bitbashing.io/embedded-cpp.html
 */

extern "C" {
#include <hf-risc.h>
}

class Geeks
{
private:
	char geekname[50];
	int id;
public:
	Geeks(char *name, int id);
	~Geeks() { };
	void printName();
	void printId();
};

Geeks::Geeks(char *name, int id)
{
	strcpy(this->geekname, name);
	this->id = id;
}

/*
Geeks::~Geeks(void)
{
	strcpy(this->geekname, "\0");
	this->id = 0;
}
*/

void Geeks::printName()
{
	printf("Geekname is: %s\n", this->geekname);
}

void Geeks::printId()
{
	printf("id is: %d\n", this->id);
}


class Shit
{
public:
	void testMethod() { printf("olar\n"); };
	void testMethod2() { printf("olar2\n"); };
};

class Geeks2 : public Shit
{
public:
	void testMethod2() { printf("hey2\n"); };
	void testMethod3() { printf("hey3\n"); };

};

void main_app() {
	Geeks obj1((char *)"xyz", 11);
	Geeks obj2((char *)"abcd", 22);
	Geeks2 obj3;
	Geeks2 *p;

	obj1.printName();
	obj1.printId();

	obj2.printName();
	obj2.printId();
	
	p = &obj3;
	p->testMethod();
	p->testMethod2();
	p->testMethod3();
}
