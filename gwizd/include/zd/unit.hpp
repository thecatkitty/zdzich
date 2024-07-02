#include <zd/gen/generator.hpp>
#include <zd/lex/lexer.hpp>

namespace zd
{

class unit
{
    lex::lexer        &_lex;
    std::list<ustring> _inc_dirs;

    const ustring *_path;
    unsigned       _line;

    gen::generator &_gen;

  public:
    typedef void (*node_callback)(unit *);

    unit(lex::lexer          &lexer,
         gen::generator      &generator,
         std::list<ustring> &&inc_dirs = {})
        : _lex{lexer}, _inc_dirs{std::move(inc_dirs)}, _path{&lexer.get_path()},
          _line{1}, _gen{generator}, on_node{nullptr}
    {
    }

    const ustring &
    get_path() const
    {
        return *_path;
    }

    unsigned
    get_line() const
    {
        return _line;
    }

    unsigned
    get_column() const
    {
        return 0;
    }

    error
    process()
    {
        return process(_lex);
    }

    node_callback on_node;

    static const auto error_origin_tag = error_origin::runner;

    enum class error_code : uint8_t
    {
        no_include = 0,
    };

  private:
    ustring
    get_include_path(const lex::lexer &lex, const ustring &inc) const;

    error
    process(lex::lexer &lexer);
};

} // namespace zd
