#include <zd/containers.hpp>
#include <zd/gen/zd4_section.hpp>

#ifdef __ia16__
#include <libi86/stdlib.h>

static uint8_t _tmpfile_num{0};
#endif

using namespace zd;
using namespace zd::gen;

zd4_section::zd4_section(bool load) : _pf{nullptr}, _offset{0}, _relocs{}
{
    if (load)
    {
#ifdef __ia16__
        // newlib-ia16 seems to have some misbehaving tmpfile implementation
        auto tmp_dir = std::getenv("TMP");
        tmp_dir = tmp_dir ? tmp_dir : std::getenv("TEMP");
        if (tmp_dir)
        {
            _fname.append(tmp_dir);
            _fname.append("\\");
        }

        char name[13];
        std::sprintf(name, "~ZDSEC%02X.TMP", _tmpfile_num++);
        _fname.append(name);
        _pf = std::fopen(_fname.data(), "wb+");
#else
        _pf = std::tmpfile();
#endif
    }
}

zd4_section::~zd4_section()
{
    if (_pf)
    {
        std::fclose(std::exchange(_pf, nullptr));
#ifdef __ia16__
        std::remove(_fname.data());
#endif
    }
}

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
        _relocs.push_back({_offset + ref.address, ref.section, ref.offset});
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
zd4_section::relocate(std::FILE *output, const uint16_t *bases)
{
    int pos{0};
    std::rewind(_pf);

    auto reloc = _relocs.begin();

    int byte{0};
    while (size() > pos)
    {
        if ((_relocs.end() != reloc) && (reloc->address == pos))
        {
            uint16_t address{};
            std::fread(&address, sizeof(address), 1, _pf);

            uint16_t value = bases[reloc->section] + address + reloc->offset;
            std::fwrite(&value, sizeof(value), 1, output);

            pos += sizeof(address);
            reloc++;
            continue;
        }

        if (EOF == (byte = std::fgetc(_pf)))
        {
            break;
        }

        std::fwrite(&byte, sizeof(uint8_t), 1, output);
        pos++;
    }

    return true;
}
