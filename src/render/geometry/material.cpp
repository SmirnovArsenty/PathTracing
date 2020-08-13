#include "mth.h"
#include "material.h"
using namespace pt;

material material::default_material() { return { "default", 0.5, 0.5, vec(.5) }; }
