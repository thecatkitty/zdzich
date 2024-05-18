#include <zd/containers.hpp>
#include <zd/gen/zd4_section.hpp>

#ifdef __ia16__
#include <libi86/stdlib.h>

static uint8_t _tmpfile_num{0};
#endif

using namespace zd;
using namespace zd::gen;

zd4_section::zd4_section(unsigned index_, bool load)
    : index{index_}, _pf{nullptr}, _offset{0}, _relocs{}
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
zd4_section::emit(const uint8_t *payload, unsigned size)
{
    if (!_pf)
    {
        return UINT_MAX;
    }

    if (size != std::fwrite(payload, 1, size, _pf))
    {
        return UINT_MAX;
    }

    return std::exchange(_offset, _offset + size);
}

void
zd4_section::emit_byte(uint8_t byte)
{
    std::fwrite(&byte, sizeof(byte), 1, _pf);
    _offset += sizeof(byte);
}

void
zd4_section::emit_word(uint16_t word)
{
    std::fwrite(&word, sizeof(word), 1, _pf);
    _offset += sizeof(word);
}

void
zd4_section::emit_ref(const zd4_relocation &ref)
{
    _relocs.push_back({_offset, ref.section, ref.offset, ref.relative});
    emit_word(ref.address);
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
zd4_section::relocate(std::FILE                    *output,
                      const uint16_t               *bases,
                      const zd4_reference_resolver *resolver)
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

            if (zd4_section_unkn == reloc->section)
            {
                unsigned sym_section;
                unsigned sym_address;
                if (!resolver->get_symbol_address(address, sym_section,
                                                  sym_address))
                {
                    std::fprintf(stderr, "error: unresolved symbol number %u\n",
                                 address);
                    return false;
                }

                reloc->section = sym_section;
                reloc->address = sym_address;
                address = reloc->address;
            }

            uint16_t abs_source = bases[index] + pos;
            uint16_t abs_target = bases[reloc->section] + address;
            int      diff = abs_target - abs_source;

            uint16_t value = reloc->relative ? diff + reloc->offset
                                             : bases[reloc->section] + address +
                                                   reloc->offset;
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
