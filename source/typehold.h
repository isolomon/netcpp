#ifndef LIB_TYPEHOLD_H
#define LIB_TYPEHOLD_H

namespace lib {

template <class T>
struct decay { typedef T type; };

template <class T>
struct decay<T&> { typedef T type; };

template <class T>
struct decay<const T&> { typedef T type; };

template <class T1>
struct typehold
{
    typename decay<T1>::type value;
    typehold(T1 t1) : value(t1) {}
};

template <>
struct typehold<void> { };

template <class T1, class T2>
struct typehold2
{
    typename decay<T1>::type value;
    typename decay<T2>::type value2;
    typehold2(T1 t1, T2 t2) : value(t1), value2(t2) {}
};

template <class T1, class T2, class T3>
struct typehold3
{
    typename decay<T1>::type value;
    typename decay<T2>::type value2;
    typename decay<T3>::type value3;
    typehold3(T1 t1, T2 t2, T3 t3) : value(t1), value2(t2), value3(t3) {}
};

template <class T1, class T2, class T3, class T4>
struct typehold4
{
    typename decay<T1>::type value;
    typename decay<T2>::type value2;
    typename decay<T3>::type value3;
    typename decay<T4>::type value4;
    typehold4(T1 t1, T2 t2, T3 t3, T3 t4) : value(t1), value2(t2), value3(t3), value4(t4) {}
};

template <class T1, class T2, class T3, class T4, class T5>
struct typehold5
{
    typename decay<T1>::type value;
    typename decay<T2>::type value2;
    typename decay<T3>::type value3;
    typename decay<T4>::type value4;
    typename decay<T5>::type value5;
    typehold5(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) : value(t1), value2(t2), value3(t3), value4(t4), value5(t5) {}
};

template <class T1, class T2, class T3, class T4, class T5, class T6>
struct typehold6
{
    typename decay<T1>::type value;
    typename decay<T2>::type value2;
    typename decay<T3>::type value3;
    typename decay<T4>::type value4;
    typename decay<T5>::type value5;
    typename decay<T6>::type value6;
    typehold6(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) : value(t1), value2(t2), value3(t3), value4(t4), value5(t5), value6(t6) {}
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct typehold7
{
    typename decay<T1>::type value;
    typename decay<T2>::type value2;
    typename decay<T3>::type value3;
    typename decay<T4>::type value4;
    typename decay<T5>::type value5;
    typename decay<T6>::type value6;
    typename decay<T7>::type value7;
    typehold7(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7) : value(t1), value2(t2), value3(t3), value4(t4), value5(t5), value6(t6), value7(t7){}
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct typehold8
{
    typename decay<T1>::type value;
    typename decay<T2>::type value2;
    typename decay<T3>::type value3;
    typename decay<T4>::type value4;
    typename decay<T5>::type value5;
    typename decay<T6>::type value6;
    typename decay<T7>::type value7;
    typename decay<T8>::type value8;
    typehold8(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8) : value(t1), value2(t2), value3(t3), value4(t4), value5(t5), value6(t6), value7(t7), value8(t8) {}
};

} // end namespace lib
#endif//LIB_TYPEHOLD_H
