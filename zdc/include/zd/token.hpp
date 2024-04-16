#include <string>

namespace zd
{

enum class token : int
{
    eof = -1,
    unknown = -1000,
    line_break,
    literal_int,
    literal_str,
    name,
    comma, // ,
    end,   // Koniec
};

struct token_hash
{
    std::size_t
    operator()(const token &tok) const noexcept
    {
        return static_cast<std::size_t>(tok);
    }
};

std::string
to_string(token tok);

} // namespace zd
