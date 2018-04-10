#pragma once

#define REFTIMES_PER_SEC        10000000
#define REFTIMES_PER_MILLISEC   10000

#define DECLARE_ACCESSOR(type, name) \
    type name() const { return m_##name; }

#define DECLARE_MUTATOR(type, name) \
     void name(type val) { m_##name = val; }

#define DECLARE_ACCESSOR_AND_MUTATOR(type, name) \
    DECLARE_ACCESSOR(type, name) \
    DECLARE_MUTATOR(type, name)