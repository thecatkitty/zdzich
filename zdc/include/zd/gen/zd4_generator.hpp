#pragma once

#include <list>

#include <zd/gen/generator.hpp>
#include <zd/gen/x86_assembler.hpp>
#include <zd/gen/zd4_section.hpp>
#include <zd/par/node.hpp>

namespace zd
{

namespace gen
{

class zd4_generator : public generator, public zd4_reference_resolver
{
    using section_list = std::list<zd4_section>;

    section_list           _codes;
    section_list::iterator _curr_code;

    zd4_section _data;
    zd4_section _udat;

    std::list<symbol> _symbols;
    unsigned          _symbol_num;

    x86_assembler _as;

    ustring  _path;
    unsigned _line;
    unsigned _column;

  public:
    zd4_generator()
        : _codes{}, _data{zd4_section_data}, _udat{zd4_section_udat, false},
          _symbols{}, _symbol_num{0}, _as{_data}, _path{}, _line{1}, _column{1}
    {
        _codes.emplace_back(zd4_section_code);
        _curr_code = _codes.begin();
        _as.bind_code(*_curr_code);
    }

    static const auto error_origin_tag = error_origin::generator;

    using error_code = generator_error;

    const ustring &
    get_path() const
    {
        return _path;
    }

    unsigned
    get_line() const
    {
        return _line;
    }

    unsigned
    get_column() const
    {
        return _column;
    }

    bool
    process(const par::assignment_node &node) override;

    bool
    process(const par::call_node &node) override;

    bool
    process(const par::condition_node &node) override;

    bool
    process(const par::declaration_node &node) override;

    bool
    process(const par::end_node &node) override;

    bool
    process(const par::emit_node &node) override;

    bool
    process(const par::include_node &node) override
    {
        return false;
    }

    bool
    process(const par::jump_node &node) override;

    bool
    process(const par::label_node &node) override;

    bool
    process(const par::number_node &node) override
    {
        return false;
    }

    bool
    process(const par::object_node &node) override
    {
        return false;
    }

    bool
    process(const par::operation_node &node) override;

    bool
    process(const par::procedure_node &node) override;

    bool
    process(const par::register_node &node) override
    {
        return false;
    }

    bool
    process(const par::string_node &node) override
    {
        return false;
    }

    bool
    process(const par::subscript_node &node) override
    {
        return false;
    }

    void
    link(std::FILE *output);

    bool
    get_symbol_address(unsigned  index,
                       unsigned &section,
                       unsigned &address) const override;

    symbol &
    get_symbol(const ustring &name);

  private:
    static ustring
    get_cname(const ustring &name);

    void
    set_position(const par::node &node);

    bool
    set_symbol(const ustring    &name,
               symbol_type       type,
               zd4_known_section section,
               unsigned          address);

    class nesting_guard
    {
        zd4_generator         &_gen;
        section_list::iterator _parent;

      public:
        nesting_guard(zd4_generator &generator)
            : _gen{generator}, _parent{_gen._curr_code}
        {
            if (zd4_max_code_sections ==
                std::distance(_gen._codes.begin(), _gen._curr_code))
            {
                std::fputs("error: too many nested procedures\n", stderr);
                std::abort();
            }

            _gen._curr_code++;
            if (_gen._codes.end() == _gen._curr_code)
            {
                _gen._codes.emplace_back((unsigned)_gen._codes.size());
                _gen._curr_code = std::prev(_gen._codes.end());
            }

            _gen._as.bind_code(*_gen._curr_code);
        }

        ~nesting_guard()
        {
            _gen._as.bind_code(*_parent);
            _gen._curr_code = _parent;
        }
    };

    friend class zd4_builtins;
};

} // namespace gen

} // namespace zd
