extern "C" {
#include <hf-risc.h>
}

class dog
{
public:
    dog()
    {
        printf("Constructor called\n");
        bark() ;
    }

    ~dog()
    {
        bark();
    }

    virtual void bark()
    {
        printf("Virtual method called\n");
    }

    void seeCat()
    {
        bark();
    }
};

class Yellowdog : public dog
{
public:
        Yellowdog()
        {
            printf("Derived class Constructor called\n");
        }
        void bark()
        {
            printf("Derived class Virtual method called\n");
        }
};

void main_app()
{
/*    Yellowdog *d;

    d = new Yellowdog;

    d->seeCat();
*/
    Yellowdog d;

    d.seeCat();
}
