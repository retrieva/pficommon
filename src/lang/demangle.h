#ifndef INCLUDE_GUARD_PFI_LANG_DEMANGLE
#define INCLUDE_GUARD_PFI_LANG_DEMANGLE
#include <string>
#include <typeinfo>

#if __GNUC__ >= 3
#include <cstdlib>
#include <cxxabi.h>

namespace pfi {
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
} // namespace pfi

#else // #if __GNUC__ >= 3

namespace pfi {
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
} // namespace pfi
#endif // #if __GNUC__ >= 3

namespace pfi {
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
} // namespace pfi

#endif // #ifndef INCLUDE_GUARD_PFI_LANG_DEMANGLE
