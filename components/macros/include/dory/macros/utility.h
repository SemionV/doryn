#pragma once

#define DORY_CONCAT_IMPL(x, y) x##y
#define DORY_CONCAT(x, y) DORY_CONCAT_IMPL(x, y)
