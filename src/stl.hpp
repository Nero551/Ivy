#include <functional>
namespace stl
{
template <typename Signature> using function = std::copyable_function<Signature>;
}
