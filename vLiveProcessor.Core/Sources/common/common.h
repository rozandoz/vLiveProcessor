#pragma once

#define REFTIMES_PER_SEC        10000000
#define REFTIMES_PER_MILLISEC   10000

#define DECLARE_ACCESSOR(type, name) \
    type name() const { return m_##name; }