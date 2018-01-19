/// \file ROOT/RIntegerSequence.hxx
/// \ingroup Base StdExt
/// \author Danilo Piparo
/// \date 2018-01-19

/*************************************************************************
 * Copyright (C) 1995-2018, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_RIntegerSequence
#define ROOT_RIntegerSequence

#if __cplusplus < 201403L && !defined(_MSC_VER)

namespace std {

/// Class template integer_sequence
template <typename _Tp, _Tp... _Idx>
struct integer_sequence {
   typedef _Tp value_type;
   static constexpr size_t size() { return sizeof...(_Idx); }
};

template <typename _Tp, _Tp _Num, typename _ISeq = typename _Build_index_tuple<_Num>::__type>
struct _Make_integer_sequence;

template <typename _Tp, _Tp _Num, size_t... _Idx>
struct _Make_integer_sequence<_Tp, _Num, _Index_tuple<_Idx...>> {
   static_assert(_Num >= 0, "Cannot make integer sequence of negative length");

   typedef integer_sequence<_Tp, static_cast<_Tp>(_Idx)...> __type;
};

/// Alias template make_integer_sequence
template <typename _Tp, _Tp _Num>
using make_integer_sequence = typename _Make_integer_sequence<_Tp, _Num>::__type;

/// Alias template index_sequence
template <size_t... _Idx>
using index_sequence = integer_sequence<size_t, _Idx...>;

/// Alias template make_index_sequence
template <size_t _Num>
using make_index_sequence = make_integer_sequence<size_t, _Num>;

/// Alias template index_sequence_for
template <typename... _Types>
using index_sequence_for = make_index_sequence<sizeof...(_Types)>;
}

#else
#include <utility>
#endif

#endif
