#include <cstdint>
#include <cstdio>
#include <list>
#include <utility>

#include <zd/ustring.hpp>

namespace zd
{

namespace gen
{

struct zd4_relocation
{
    unsigned offset : 16;
    unsigned section : 4;
};

enum zd4_known_section
{
    zd4_section_code = 0,
    zd4_section_data = 1,
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
    zd4_section(bool load = true);

    ~zd4_section();

    uint16_t
    size() const
    {
        return static_cast<uint16_t>(_offset);
    }

    unsigned
    emit(const uint8_t        *payload,
         unsigned              size,
         const zd4_relocation *refs = nullptr,
         size_t                ref_count = 0);

    inline unsigned
    emit(const char           *payload,
         unsigned              size,
         const zd4_relocation *refs = nullptr,
         size_t                ref_count = 0)
    {
        return emit(reinterpret_cast<const uint8_t *>(payload), size, refs,
                    ref_count);
    }

    unsigned
    reserve(unsigned size);

    bool
    relocate(std::FILE *output, const uint16_t *bases);
};

} // namespace gen

} // namespace zd
