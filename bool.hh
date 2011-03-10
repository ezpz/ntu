#ifndef JRB_NTU_BOOL__HH__
#define JRB_NTU_BOOL__HH__

/** 
 * Same<T1,T2>::value == false
 * T1 != T2 (type equality)
 */
template< typename T1, typename T2 >
struct Same { 
    static const bool value = false;
};

/** 
 * Same<T1,T2>::value == true
 * T1 != T2 (type equality)
 */
template < typename T > 
struct Same< T, T > { 
    static const bool value = true; 
};

/** 
 * Compile-type OR of two boolean values
 */
template < bool B1, bool B2 >
struct Or {
    static const bool value = B1 || B2;
};

/**
 * Compile-time AND of two boolean values
 */
template < bool B1, bool B2 >
struct And {
    static const bool value = B1 && B2;
};


#endif //JRB_NTU_BOOL__HH__
