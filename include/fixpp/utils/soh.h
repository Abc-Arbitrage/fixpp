/* visitor.h
   Mathieu Stefani, 07 april 2017
   
  A simple header file exposing the SOH character.

  By default, SOH should be the real \x01 SOH character. However, in tests
  it is way simpler to use a Pipe '|' as an SOH character.

  This header makes it easy to override the SOH character
*/

#pragma once

#if !defined(SOH_CHARACTER)
  #define SOH_CHARACTER '\x01'
#endif

namespace Fixpp
{

    static constexpr const char SOH = SOH_CHARACTER;

} // namespace Fixpp
