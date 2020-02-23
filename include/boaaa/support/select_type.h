#ifndef BOAAA_SUPPORT_SELECT_TYPE_H
#define BOAAA_SUPPORT_SELECT_TYPE_H

#include <type_traits>

namespace boaaa
{
#define __n sizeof...(TYPES)

	template<int N, bool REVERSE, template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class TYPE, class ...TYPES>
	struct _select_type
	{
		using T = typename _select_type<N - 1, REVERSE, COMPARE_FUNC, COMPARE, TYPES...>::type;

		typedef std::conditional_t<REVERSE,
			std::conditional_t<COMPARE_FUNC<TYPE, COMPARE>::value, TYPE, T>,
			std::conditional_t<COMPARE_FUNC<COMPARE, TYPE>::value, TYPE, T>> type;
	};

	template<bool REVERSE, template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class TYPE, class ...TYPES>
	struct _select_type<0, REVERSE, COMPARE_FUNC, COMPARE, TYPE, TYPES...>
	{
		typedef std::conditional_t<REVERSE,
			std::conditional_t<COMPARE_FUNC<TYPE, COMPARE>::value, TYPE, void>,
			std::conditional_t<COMPARE_FUNC<COMPARE, TYPE>::value, TYPE, void>> type;
	};

	template<template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class ...TYPES>
	struct select_type {
		using type = typename _select_type<__n - 1, false, COMPARE_FUNC, COMPARE, TYPES...>::type;
	};

	template<template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class ...TYPES>
	struct select_type_reverse {
		using type = typename _select_type<__n - 1, true, COMPARE_FUNC, COMPARE, TYPES...>::type;
	};

	template<template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class ...TYPES>
	using select_type_t		    = typename select_type<COMPARE_FUNC, COMPARE, TYPES...>::type;

	template<template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class ...TYPES>
	using select_type_reverse_t = typename select_type_reverse<COMPARE_FUNC, COMPARE, TYPES...>::type;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	template<int N, bool REVERSE, template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class TYPE, class TYPE_SELECTION, class ...TYPES>
	struct _select_type_for {
		using T = typename _select_type_for<N - 2, REVERSE, COMPARE_FUNC, COMPARE, TYPES...>::type;

		typedef std::conditional_t<REVERSE,
			std::conditional_t<COMPARE_FUNC<TYPE, COMPARE>::value, TYPE_SELECTION, T>,
			std::conditional_t<COMPARE_FUNC<COMPARE, TYPE>::value, TYPE_SELECTION, T>> type;
	};

	template<bool REVERSE, template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class TYPE, class TYPE_SELECTION, class ...TYPES>
	struct _select_type_for<0, REVERSE, COMPARE_FUNC, COMPARE, TYPE, TYPE_SELECTION, TYPES...> {
		typedef std::conditional_t<REVERSE,
			std::conditional_t<COMPARE_FUNC<TYPE, COMPARE>::value, TYPE_SELECTION, void>,
			std::conditional_t<COMPARE_FUNC<COMPARE, TYPE>::value, TYPE_SELECTION, void>> type;
	};

	template<bool REVERSE, template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class TYPE, class TYPE_SELECTION, class ...TYPES>
	struct _select_type_for<1, REVERSE, COMPARE_FUNC, COMPARE, TYPE, TYPE_SELECTION, TYPES...> {
		typedef std::conditional_t<REVERSE,
			std::conditional_t<COMPARE_FUNC<TYPE, COMPARE>::value, TYPE_SELECTION, void>,
			std::conditional_t<COMPARE_FUNC<COMPARE, TYPE>::value, TYPE_SELECTION, void>> type;
	};

	template<template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class ...TYPES>
	struct select_type_for {
		using type = typename _select_type_for<__n - 1, false, COMPARE_FUNC, COMPARE, TYPES...>::type;
	};

	template<template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class ...TYPES>
	struct select_type_reverse_for {
		using type = typename _select_type_for<__n - 1, true, COMPARE_FUNC, COMPARE, TYPES...>::type;
	};

	template<template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class ...TYPES>
	using select_type_for_t         = typename select_type_for<COMPARE_FUNC, COMPARE, TYPES...>::type;

	template<template<class T1, class T2> class COMPARE_FUNC, class COMPARE, class ...TYPES>
	using select_type_reverse_for_t = typename select_type_reverse_for<COMPARE_FUNC, COMPARE, TYPES...>::type;
}

#undef __n

#endif //BOAAA_SUPPORT_SELECT_TYPE_H