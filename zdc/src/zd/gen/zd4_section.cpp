#include <climits>

#include <zd/containers.hpp>
#include <zd/gen/zd4_section.hpp>

using namespace zd;
using namespace zd::gen;

unsigned
zd4_section::emit(const uint8_t        *payload,
                  unsigned              size,
                  const zd4_relocation *refs,
                  size_t                ref_count)
{
    if (!_pf)
    {
        return UINT_MAX;
    }

    if (size != std::fwrite(payload, 1, size, _pf))
    {
        return UINT_MAX;
    }

    for (auto &ref : range<zd4_relocation>(refs, ref_count))
    {
        _relocs.push_back({_offset + ref.offset, ref.section});
    }

    return std::exchange(_offset, _offset + size);
}

unsigned
zd4_section::reserve(unsigned size)
{
    if (_pf)
    {
        for (size_t i = 0; i < size; i++)
        {
            std::fputc(0, _pf);
        }
    }

    return std::exchange(_offset, _offset + size);
}

bool
zd4_section::relocate(const uint16_t *bases)
{
    int pos{0};
    std::rewind(_pf);

    auto reloc = _relocs.begin();

    int byte{0};
    while (size() > pos)
    {
        if ((_relocs.end() != reloc) && (reloc->offset == pos))
        {
            uint16_t offset{};
            std::fread(&offset, sizeof(offset), 1, _pf);

            uint16_t address = bases[reloc->section] + offset;
            std::printf("%02X %02X ", address & 0xFF, (address >> 8) & 0xFF);

            pos += sizeof(offset);
            reloc++;
            continue;
        }

        if (EOF == (byte = std::fgetc(_pf)))
        {
            break;
        }

        std::printf("%02X ", byte);
        pos++;
    }

    return true;
}

void
zd4_section::list_relocations() const
{
    for (auto &reloc : _relocs)
    {
        std::printf("\t%1X:%04X\n", reloc.section, reloc.offset);
    }
}
