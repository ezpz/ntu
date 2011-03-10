#ifndef JRB_NTU_HH__
#define JRB_NTU_HH__

#include <stdexcept>
#include <exception>
#include "bool.hh"

typedef unsigned char byte;


/**
 * Thrown when trying to use an NTU with a type it
 * does not contain
 */
struct InvalidType : public std::exception {
    virtual const char* what() const throw() {
        return "InvalidType";
    }
} type_exception;


/** 
 * Unused template arguments default to the empty type
 */
struct empty {};


/** 
 * The size of a type
 * We use this instead of sizeof to allow for a zero-sized
 * empty type.
 */
template< typename T >
struct TypeSize {
    const static std::size_t size = sizeof (T);
};


/** 
 * Specialization for the empty type. In our case, 0.
 */
template<> struct TypeSize< empty > {
    const static std::size_t size = 0;
};


/** 
 * Compile-time max(X,Y) 
 */
template< std::size_t N1, std::size_t N2 >
struct MaxSize {
    const static std::size_t size = N1 < N2 ? N2 : N1;
};


/** 
 * True iff Q is a member of [T1,T2,T3,T4]
 * The result is built at compile time but used at runtime.
 */
template< typename Q, typename T1, typename T2, typename T3, typename T4 >
struct Member {
    typedef Or< Same< Q, T1 >::value, Same< Q, T2 >::value > first_pair;
    typedef Or< Same< Q, T3 >::value, Same< Q, T4 >::value > second_pair;
    typedef Or< first_pair::value, second_pair::value > expression;
    static const bool value = expression::value;
};


/** 
 * Result is the maximum sized type of [T1,T2,T3,T4]
 */
template< typename T1, typename T2, typename T3, typename T4 >
struct MaxType {
    typedef MaxSize< TypeSize< T1 >::size, TypeSize< T2 >::size > first_pair;
    typedef MaxSize< TypeSize< T3 >::size, TypeSize< T4 >::size > second_pair;
    typedef MaxSize< first_pair::size, second_pair::size > expression;
    const static std::size_t size = expression::size;
};


/** 
 * Provides mechanism by which types can be saved (for proper destructor call)
 * See SavedType for complete implementation
 */
struct TypeSaver { virtual ~TypeSaver () {} };

/**
  * Specific saved type
  * This is how we track the 'current' type of the union
  */
template< typename T >
struct SavedType : public TypeSaver {
    T * t_;
    SavedType (T * t) { t_ = t; }
    ~SavedType () { t_->T::~T (); }
};

/**
 * Union that will support up to 4 types
 * These types can be complex types with non-trivial destructors
 * Supported operations of:
 *  T t, *pt;
 *  t = ntu;
 *  ntu = t;
 *  pt = ntu;
 *  Where ntu is the union holding (at least) the type T
 */
template< typename T1, typename T2 = empty, 
    typename T3 = empty, typename T4 = empty >
union NonTrivialUnion {
private:
    typedef MaxType< T1, T2, T3, T4 > max_type;
    TypeSaver * ts_;
    byte bytes_[max_type::size + sizeof (TypeSaver *)];
    NonTrivialUnion (); // unimplemented for same reasons as union
public:

    /**
     * In this implementation, an object is required to creat an NTU
     */
    template< typename T >
    NonTrivialUnion (const T& t) {
        ts_ = new SavedType< T > (new (bytes_ + sizeof (TypeSaver *)) T (t));
    }

    /** Cleans the current stored object as well */
    ~NonTrivialUnion () { delete ts_; }

    /**
     * Assignment of a new value. 
     * This is a type-checked operation.
     * The last object's destructors are properly managed here
     */
    template< typename T >
    T& operator= (const T& t) {
        if (! Member<T, T1, T2, T3, T4>::value) {
            // Invalid type assignment
            throw type_exception;
        }
        delete ts_;
        ts_ = new SavedType< T > (new (bytes_ + sizeof (TypeSaver *)) T (t));
        return *(reinterpret_cast< T* >(bytes_ + sizeof (TypeSaver *)));
    }

    /**
     * Direct memory access
     * This is a bounds-checked operation
     */
    byte& operator[] (std::size_t i) { 
        if (i > max_type::size)
            throw std::range_error("i > max_type::size");
        return bytes_[sizeof (TypeSaver *) + i];
    }

    /**
     * implicit cast operation. 
     * This is a type-checked operation
     * Allows for:
     *  int i = 1, j = 0;
     *  NonTrivialUnion<int> ntu(i);
     *  j = ntu;
     * Note: A copy is returned here and the original object is untouched
     * Warning:
     *  In the case of objects that overload their own operator= this
     *  is a source of concern. For instance, std::string will not work here
     *  due to an ambiguity between its three choices of:
     *      operator=(const char *)
     *      operator=(std::string)
     *      operator=(char *)
     *  The type, T, here can not be determined.
     *  The hack/workaround is to call the operator directly. For instance:
     *      std::string str;
     *      NonTrivialUnion< int, long, Thing, std::string > ntu(s);
     *      str = ntu.operator std::string ();
     *  This is unfortunate, but I do not currently know of a very good
     *  alternative.
     */
    template< typename T >
    operator T () {
        if (! Member<T, T1, T2, T3, T4>::value) {
            // Invalid type assignment
            throw type_exception;
        }
        return *(reinterpret_cast< T* >(bytes_ + sizeof (TypeSaver *)));
    }

    /**
     * implicit cast to pointer-to-object
     * This is a type-checked operation
     * Allows for:
     *  int i = 1, *pi;
     *  NonTrivialUnion<int> ntu(i);
     *  pi = ntu;
     *  *pi = 10; // modifies ntu
     */
    template< typename T >
    operator T* () {
        if (! Member<T, T1, T2, T3, T4>::value) {
            // Invalid type assignment
            throw type_exception;
        }
        return reinterpret_cast< T* >(bytes_ + sizeof (TypeSaver *));
    }
};

#endif //JRB_NTU_HH__
