// Copyright (c)2008-2011, Preferred Infrastructure Inc.
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of Preferred Infrastructure nor the names of other
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef JUBATUS_UTIL_LANG_DEMANGLE_H_
#define JUBATUS_UTIL_LANG_DEMANGLE_H_
#include <string>
#include <typeinfo>

#if __GNUC__ >= 3
#include <cstdlib>
#include <cxxabi.h>

namespace jubatus {
namespace util {
namespace lang {
namespace detail {
class malloced_string_holder {
public:
    explicit malloced_string_holder(char const* s)
        : str_(s)
    {}

    ~malloced_string_holder() {
        ::std::free(static_cast<void*>(const_cast<char*>(str_)));
    }

    char const* str() const {
        return str_;
    }

private:
    // noncopyable, nonmovable
    malloced_string_holder(malloced_string_holder const&);
    malloced_string_holder const& operator=(malloced_string_holder const&);

    char const* str_;
};
} // namespace detail

inline
::std::string demangle(char const* typeid_name)
{
    int status;
    using detail::malloced_string_holder;
    malloced_string_holder s(::abi::__cxa_demangle(typeid_name, 0, 0, &status));
    if (status == 0)
        return s.str();
    else
        return typeid_name;
}

inline
::std::string demangle(::std::string const& typeid_name)
{
    return demangle(typeid_name.c_str());
}
} // namespace lang
} // namespace util
} // namespace jubatus

#else // #if __GNUC__ >= 3

namespace jubatus {
namespace util {
namespace lang {
inline
::std::string demangle(char const* typeid_name)
{
    return typeid_name;
}

inline
::std::string demangle(::std::string const& typeid_name)
{
    return typeid_name;
}
} // namespace lang
} // namespace util
} // namespace jubatus
#endif // #if __GNUC__ >= 3

namespace jubatus {
namespace util {
namespace lang {
template <typename T>
::std::string get_typename()
{
    return demangle(typeid(T).name());
}

template <typename T>
::std::string get_static_typename(T const&)
{
    return demangle(typeid(T).name());
}

template <typename T>
::std::string get_dynamic_typename(T const& x)
{
    return demangle(typeid(x).name());
}
} // namespace lang
} // namespace util
} // namespace jubatus

#endif // #ifndef JUBATUS_UTIL_LANG_DEMANGLE_H_

