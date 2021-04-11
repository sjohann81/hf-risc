#include <hf-risc.h>

/* Animal superclass: attributes and methods */
struct animal_s {
	struct animal_vtab_s const *vptr;
	char name[30];
};

/* animal_eat() and animal_noise() are virtual methods */
struct animal_vtab_s {
	void (*eat)();
	void (*noise)();
};

void animal_eat(struct animal_s *animal)
{
	(*animal->vptr->eat)();
}

void animal_noise(struct animal_s *animal)
{
	(*animal->vptr->noise)();
}

void animal_name(struct animal_s *animal)
{
	printf("%s\n", animal->name);
}

void animal_ctor(struct animal_s *animal, char *name)
{
	static struct animal_vtab_s const vtbl = {&animal_eat, &animal_noise};
	animal->vptr = &vtbl;
	strcpy(animal->name, name);
}


/* Dog subclass: attributes and methods
 * Inheritance from Animal superclass
 * dog_food() and dog_bark() are virtual (polymorphism)
 */
struct dog_s {
	struct animal_s animal;
	int age;
	int weight;
};

void dog_food()
{
	printf("dog food\n");
}

void dog_bark()
{
	printf("wooof!\n");
}

int dog_age(struct dog_s *dogo)
{
	return dogo->age;
}

int dog_weight(struct dog_s *dogo)
{
	return dogo->weight;
}

/* Dog constructor */
void dog_ctor(struct dog_s *dogo, char *name, int age, int weight)
{
	/* call superclass constructor */
	animal_ctor(&dogo->animal, name);
	
	/* initialize subclass attributes / virtual functions */
	static struct animal_vtab_s const vtbl = {&dog_food, dog_bark};
	dogo->animal.vptr = &vtbl;
	dogo->age = age;
	dogo->weight = weight;
}

/* Cat subclass: attributes and methods
 * Inheritance from Animal superclass
 * cat_food() and cat_meow() are virtual (polymorphism)
 */
struct cat_s {
	struct animal_s animal;
	int age;
	int weight;
};

void cat_food()
{
	printf("cat food\n");
}

void cat_meow()
{
	printf("meeoww!\n");
}

int cat_age(struct cat_s *cato)
{
	return cato->age;
}

int cat_weight(struct cat_s *cato)
{
	return cato->weight;
}

/* Cat constructor */
void cat_ctor(struct cat_s *cato, char *name, int age, int weight)
{
	/* call superclass constructor */
	animal_ctor(&cato->animal, name);
	
	/* initialize subclass attributes / virtual functions */
	static struct animal_vtab_s const vtbl = {&cat_food, cat_meow};
	cato->animal.vptr = &vtbl;
	cato->age = age;
	cato->weight = weight;
}

int main(void)
{
	struct dog_s dogo;
	struct cat_s cato;
	
	dog_ctor(&dogo, "harold", 3, 25);
	printf("name: "); animal_name((struct animal_s *)&dogo);
	printf("age: %d\n", dog_age(&dogo));
	printf("weight: %d\n", dog_weight(&dogo));
	animal_eat((struct animal_s *)&dogo);
	animal_noise((struct animal_s *)&dogo);
	
	cat_ctor(&cato, "miu", 2, 4);
	printf("name: "); animal_name((struct animal_s *)&cato);
	printf("age: %d\n", cat_age(&cato));
	printf("weight: %d\n", cat_weight(&cato));
	animal_eat((struct animal_s *)&cato);
	animal_noise((struct animal_s *)&cato);

	return 0;
}
