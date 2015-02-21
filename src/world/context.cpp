#include "world/context.hpp"

using namespace world;

//----------------------------//
//----- Static variables -----//

Context world::context;

//-----------------------//
//----- Interaction -----//

void Context::updateLastPlayed() noexcept
{
    worldsData.updateLastPlayed(*info);
}
