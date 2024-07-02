#include <zd/containers.hpp>
#include <zd/par/parser.hpp>
#include <zd/text/characters.hpp>
#include <zd/unit.hpp>

using namespace zd;

static ustring
_get_parent_directory(const lex::lexer &lex)
{
    ustring path{};

    auto &self = lex.get_path();
    if (!self.empty())
    {
        auto dir_end =
            ++find_last_if(self.begin(), self.end(), text::is_path_separator);
        std::for_each(self.begin(), dir_end, [&path](int ch) {
            path.append(ch);
        });
    }

    return path;
}

static void
_append_path(ustring &target, const ustring &path)
{
    target.append('.');
    target.append('/');
    for (auto ch : path)
    {
        target.append(('\\' == ch) ? '/' : ch);
    }
}

static bool
_file_exists(const ustring &path)
{
    auto file = std::fopen(path.data(), "r");
    if (!file)
    {
        return false;
    }

    std::fclose(file);
    return true;
}

ustring
unit::get_include_path(const lex::lexer &lex, const ustring &inc) const
{
    auto path = std::move(_get_parent_directory(lex));
    _append_path(path, inc);
    if (_file_exists(path))
    {
        return path;
    }

    for (ustring path : _inc_dirs)
    {
        _append_path(path, inc);
        if (_file_exists(path))
        {
            return path;
        }
    }

    return {};
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
                    std::move(get_include_path(lexer, inc_node->name));
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
