#pragma once

#include <cstdio>
#include <list>

#include <zd.hpp>
#include <zd/ustring.hpp>

namespace zd
{

namespace gen
{

struct zd4_relocation
{
    unsigned address : 16;
    unsigned section : 4;
    int      offset : 12;
    bool     relative : 1;
    int      _res : 15;
};

enum zd4_known_section
{
    zd4_section_code = 0,
    zd4_section_cod1,
    zd4_section_cod2,
    zd4_section_cod3,
    zd4_max_code_sections,
    zd4_section_data = 8,
    zd4_section_udat,
    zd4_section_unkn = 15
};

struct zd4_reference_resolver
{
    virtual result<std::pair<unsigned, unsigned>>
    get_symbol_address(unsigned index) = 0;
};

class zd4_section
{
    std::FILE                *_pf;
    unsigned                  _offset;
    std::list<zd4_relocation> _relocs;

#ifdef __ia16__
    ustring _fname{};
#endif

  public:
    const unsigned index;

    zd4_section(unsigned index_, bool load = true);

    ~zd4_section();

    uint16_t
    size() const
    {
        return static_cast<uint16_t>(_offset);
    }

    unsigned
    emit(const uint8_t *payload, unsigned size);

    inline unsigned
    emit(const char *payload, unsigned size)
    {
        return emit(reinterpret_cast<const uint8_t *>(payload), size);
    }

    unsigned
    emit_byte(uint8_t byte);

    unsigned
    emit_word(uint16_t word);

    unsigned
    emit_ref(const zd4_relocation &ref);

    unsigned
    reserve(unsigned size);

    result<void>
    relocate(std::FILE              *output,
             const uint16_t         *bases,
             zd4_reference_resolver *resolver);
};

} // namespace gen

} // namespace zd
