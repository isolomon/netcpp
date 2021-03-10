#ifndef LIB_DELEGATE_H
#define LIB_DELEGATE_H

#include "config.h"
#include <new>

// try to use a little larger HolderSize if encounters bad alloc
#define DECLARE_DELEGATE_CLASS(NAME, TEMPLATE, PARAM, ARGS) \
TEMPLATE \
{ \
    NAME() { type = 0; } \
    NAME (R (*entry)(PARAM)) { \
        if (sizeof(ptrfn) > HolderSize) throw std::bad_alloc(); \
        new(holder) ptrfn(); ((ptrfn*)holder)->entry = entry; type = 1; \
    } \
    template <class T> NAME(T* obj, R (T::*ent)(PARAM)) { \
        if (sizeof(memfn<T>) > HolderSize) throw std::bad_alloc(); \
        new(holder) memfn<T>(obj, ent); type = 2; \
    } \
    bool valid () const { return type > 0; } \
    bool equal (R (*entry)(PARAM)) const { return type == 1 && ((ptrfn*)holder)->entry == entry; } \
    template <class T> bool equal (T* obj, R (T::*ent)(PARAM)) const { return type == 2 && ((memfn<T>*)&holder)->equal(obj, ent); } \
    R invoke (PARAM) const { if (type > 0) return ((base*)holder)->invoke(ARGS); else return R(); } \
    inline R operator() (PARAM) const { return invoke(ARGS); } \
    void disconnect () { type = 0; } \
private: \
    struct base { virtual R invoke(PARAM) = 0; virtual ~base() {} }; \
    template <class T> struct memfn : public base { \
      typedef R (T::*Entry)(PARAM); \
      T* object; Entry entry; \
      virtual R invoke (PARAM) { return (object->*entry)(ARGS); } \
      memfn(T* obj, Entry ent) : object(obj), entry(ent) {} \
      bool equal (T* obj, Entry ent) { return object == obj && entry == ent; } \
    }; \
    struct ptrfn : public base { \
      R (*entry)(PARAM); \
      virtual R invoke (PARAM) { return (*entry)(ARGS); } \
    }; \
    enum { HolderSize = 28 }; \
    int type; char holder[HolderSize]; \
}

#define TMPL_DECL_0    template <class R> struct delegate<R, void>
#define TYPE_LIST_0
#define ARGS_LIST_0

#define TMPL_DECL_1    template <class R, class P1> struct delegate
#define TYPE_LIST_1    P1 p
#define ARGS_LIST_1    p

#define TMPL_DECL_2    template <class R, class P1, class P2> struct delegate2
#define TYPE_LIST_2    P1 p1, P2 p2
#define ARGS_LIST_2    p1, p2

#define TMPL_DECL_3    template <class R, class P1, class P2, class P3> struct delegate3
#define TYPE_LIST_3    P1 p1, P2 p2, P3 p3
#define ARGS_LIST_3    p1, p2, p3

#define TMPL_DECL_4    template <class R, class P1, class P2, class P3, class P4> struct delegate4
#define TYPE_LIST_4    P1 p1, P2 p2, P3 p3, P4 p4
#define ARGS_LIST_4    p1, p2, p3, p4

#define TMPL_DECL_5    template <class R, class P1, class P2, class P3, class P4, class P5> struct delegate5
#define TYPE_LIST_5    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5
#define ARGS_LIST_5    p1, p2, p3, p4, p5

#define TMPL_DECL_6    template <class R, class P1, class P2, class P3, class P4, class P5, class P6> struct delegate6
#define TYPE_LIST_6    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6
#define ARGS_LIST_6    p1, p2, p3, p4, p5, p6


DECLARE_DELEGATE_CLASS(delegate,  TMPL_DECL_1, TYPE_LIST_1, ARGS_LIST_1);

DECLARE_DELEGATE_CLASS(delegate,  TMPL_DECL_0, TYPE_LIST_0, ARGS_LIST_0);

DECLARE_DELEGATE_CLASS(delegate2, TMPL_DECL_2, TYPE_LIST_2, ARGS_LIST_2);

DECLARE_DELEGATE_CLASS(delegate3, TMPL_DECL_3, TYPE_LIST_3, ARGS_LIST_3);

DECLARE_DELEGATE_CLASS(delegate4, TMPL_DECL_4, TYPE_LIST_4, ARGS_LIST_4);

DECLARE_DELEGATE_CLASS(delegate5, TMPL_DECL_5, TYPE_LIST_5, ARGS_LIST_5);

DECLARE_DELEGATE_CLASS(delegate6, TMPL_DECL_6, TYPE_LIST_6, ARGS_LIST_6);

#endif //LIB_DELEGATE_H
