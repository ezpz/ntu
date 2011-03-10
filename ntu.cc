#include <iostream>
#include "ntu.hh"

struct Type1 { 
    byte bytes_[16];
    Type1 () { std::cerr << "Type1()" << std::endl; }
    Type1 (const Type1&) { std::cerr << "Type1(Type1)" << std::endl; }
    ~Type1 () { std::cerr << "~Type1()" << std::endl; }
};

struct Type2 { 
    byte bytes_[32];
    Type2 () { std::cerr << "Type2()" << std::endl; }
    Type2 (const Type2&) { std::cerr << "Type2(Type2)" << std::endl; }
    ~Type2 () { std::cerr << "~Type2()" << std::endl; }
    void speak () { std::cerr << "Type2::speak" << std::endl; }
};

struct Type3 { 
    byte bytes_[64];
    Type3 () { std::cerr << "Type3()" << std::endl; }
    Type3 (const Type3&) { std::cerr << "Type3(Type3)" << std::endl; }
    ~Type3 () { std::cerr << "~Type3()" << std::endl; }
};

int main () {

    Type1 t1a;
    Type2 t2a, t2b, *pt2 = 0;

    std::cerr << std::endl << std::endl << "Begin NTU testing\n" << std::endl;

    NonTrivialUnion< Type1, Type2, Type3 > ntu(t1a);

    std::cerr << std::endl;

    std::cerr << "sizeof(Type3): " << sizeof(Type3) << std::endl;
    std::cerr << "sizeof(void*): " << sizeof(void*) << std::endl;
    std::cerr << "sizeof(ntu)  : " << sizeof(ntu) << std::endl;

    std::cerr << std::endl;

    // Assign a new value, invoking both copy constructor and
    // destructor of old value
    ntu = t2a;

    std::cerr << std::endl;

    // Assign pointer-to-type. 
    pt2 = ntu;
    pt2->speak ();

    std::cerr << std::endl;

    // Assign to same type. 
    t2b = ntu;
    t2b.speak ();

    std::cerr << std::endl;

    try {
        // Assign from an invalid type (should throw)
        // No destructor invoked
        int x = 10;
        ntu = x;
    } catch (InvalidType) {
        std::cerr << "invalid assignment" << std::endl;
    }

    std::cerr << std::endl;

    try {
        // Assign to an invalid type (should throw)
        int x = ntu;
        x = 0; // kill 'unused x' warning
    } catch (InvalidType) {
        std::cerr << "invalid copy" << std::endl;
    }

    std::cerr << std::endl;

    std::cerr << "End NTU testing\n" << std::endl;

    return 0;
}

