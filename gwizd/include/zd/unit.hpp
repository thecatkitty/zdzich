#include <zd/gen/generator.hpp>
#include <zd/lex/lexer.hpp>

namespace zd
{

class unit
{
    lex::lexer     &_lex;
    gen::generator &_gen;

  public:
    typedef void (*node_callback)(unit *);

    unit(lex::lexer &lexer, gen::generator &generator)
        : _lex{lexer}, _gen{generator}, on_node{nullptr}
    {
    }

    error
    process()
    {
        return process(_lex);
    }

    node_callback on_node;

  private:
    error
    process(lex::lexer &lexer);
};

} // namespace zd
