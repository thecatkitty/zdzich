#include <map>

#include <Zdzich/ProgramDos.hpp>

using namespace Zdzich;

static std::map<Bajt, StrumieńDos> _streams;

StrumieńDos &
ProgramDos::operator[](Bajt uchwyt)
{
    if (_streams.end() == _streams.find(uchwyt))
    {
        _streams[uchwyt] = StrumieńDos(uchwyt);
    }

    return _streams[uchwyt];
}
