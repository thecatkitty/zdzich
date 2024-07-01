#include <zd/containers.hpp>
#include <zd/par/parser.hpp>
#include <zd/text/characters.hpp>
#include <zd/unit.hpp>

using namespace zd;

ustring
unit::get_include_path(const lex::lexer &lex, const ustring &inc)
{
    // Inclusion directive - #Wstaw
    ustring path{};

    auto &self = lex.get_path();
    if (!self.empty())
    {
        // Get parent path
        auto dir_end =
            ++find_last_if(self.begin(), self.end(), text::is_path_separator);

        // Create included file path
        std::for_each(self.begin(), dir_end, [&path](int ch) {
            path.append(ch);
        });
    }

    for (auto ch : inc)
    {
        path.append(('\\' == ch) ? '/' : ch);
    }

    auto file = std::fopen(path.data(), "r");
    if (!file)
    {
        return "";
    }

    std::fclose(file);
    return path;
}

error
unit::process(lex::lexer &lexer)
{
    par::parser parser{lexer};

    result<par::unique_node> result{};
    while (true)
    {
        _line = parser.get_line();
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
                auto inc_path =
                    std::move(get_include_path(_lex, inc_node->name));
                if (inc_path.empty())
                {
                    return error{*this, error_code::no_include,
                                 std::move(inc_node->name), errno};
                }

                // Process the included file
                lex::pl_istream inc_stream{inc_path,
                                           _lex.get_stream().get_encoding()};
                lex::lexer      inc_lexer{inc_stream};

                auto status = std::move(process(inc_lexer));
                if (!status)
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
