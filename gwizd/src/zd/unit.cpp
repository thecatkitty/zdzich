#include <zd/containers.hpp>
#include <zd/par/parser.hpp>
#include <zd/text/characters.hpp>
#include <zd/unit.hpp>

using namespace zd;

error
unit::process(lex::lexer &lexer)
{
    par::parser parser{lexer};

    result<par::unique_node> result{};
    while (true)
    {
        result = std::move(parser.handle());
        if (!result)
        {
            error err = std::move(result.error());
            if (err.is<par::parser>(par::parser::error_code::eof))
            {
                // End of file
                break;
            }

            return err;
        }

        auto &node = *result;
        if (node->is<par::include_node>())
        {
            auto inc_node = static_cast<par::include_node *>(node.get());
            if (!inc_node->is_binary)
            {
                // Inclusion directive - #Wstaw
                ustring inc_path{};

                auto &self = _lex.get_path();
                if (!self.empty())
                {
                    // Get parent path
                    auto dir_end = ++find_last_if(self.begin(), self.end(),
                                                  text::is_path_separator);

                    // Create included file path
                    std::for_each(self.begin(), dir_end, [&inc_path](int ch) {
                        inc_path.append(ch);
                    });
                }

                for (auto ch : inc_node->name)
                {
                    inc_path.append(('\\' == ch) ? '/' : ch);
                }

                // Process the included file
                lex::pl_istream inc_stream{inc_path,
                                           _lex.get_stream().get_encoding()};
                lex::lexer      inc_lexer{inc_stream};

                auto status = std::move(process(inc_lexer));
                if (0 != status)
                {
                    return status;
                }

                continue;
            }
        }

        zd::error status = node->generate(&_gen);
        if (!status)
        {
            return status;
        }

        if (on_node)
        {
            on_node(this);
        }
    }

    return {};
}
