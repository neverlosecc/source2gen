#include <include.h>
#include <sdk/interfaces/schemasystem/schema.h>

#include "tools/address.h"

CUtlTSHash<CSchemaClassBinding*> CSchemaSystemTypeScope::GetClasses() const
{
    return *address_t(this).offset(
        g_config_manager->Get<std::ptrdiff_t>("classes")
    ).get<CUtlTSHash<CSchemaClassBinding*>*>(0);
}

CUtlTSHash<CSchemaEnumBinding*> CSchemaSystemTypeScope::GetEnums() const
{
    return *address_t(this).offset(
        g_config_manager->Get<std::ptrdiff_t>("enums")
    ).get<CUtlTSHash<CSchemaEnumBinding*>*>(0);
}

CUtlVector<CSchemaSystemTypeScope*> CSchemaSystem::GetTypeScopes()
{
    return *address_t(this).offset(
        g_config_manager->Get<std::ptrdiff_t>("typescopes")
    ).get<CUtlVector<CSchemaSystemTypeScope*>*>(0);
}
