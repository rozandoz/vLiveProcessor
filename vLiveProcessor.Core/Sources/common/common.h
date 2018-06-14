#pragma once

#define DECLARE_ACCESSOR(type, name) \
    type name() const { return m_##name; }

#define DECLARE_MUTATOR(type, name) \
     void name(type val) { m_##name = val; }

#define DECLARE_ACCESSOR_AND_MUTATOR(type, name) \
    DECLARE_ACCESSOR(type, name) \
    DECLARE_MUTATOR(type, name)

#ifdef WIN32
#define _func__ __FUNCTION__
#else 
#define _func__ __func__
#endif