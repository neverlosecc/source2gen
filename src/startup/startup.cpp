#include <Include.h>
#include <sdk/sdk.h>

#include "tools/console/console.h"

namespace source2_gen
{
    enum class GenStatus : int32_t
    {
        kNone = 0,
        kWorking,
        kFinished,
        kFailed
    };

    std::atomic<GenStatus> gen_status = GenStatus::kNone;

    void Setup()
    {
        while (!GetModuleHandleA("GameOverlayRenderer64.dll") ||
            !GetModuleHandleA("client.dll") ||
            !GetModuleHandleA("engine2.dll") ||
            !GetModuleHandleA("steamclient64.dll") ||
            !GetModuleHandleA("schemasystem.dll"))
            sleep_for(std::chrono::seconds(5));

        fmt::print("{}: Found GameOverlayRenderer64\n", __FUNCTION__);

        try { g_config_manager->Init("config"); }
        catch (std::exception& ex)
        {
            fmt::print("{}\n", ex.what());
            gen_status = GenStatus::kFailed;
            return;
        }

        sdk::g_schema = sdk::GetInterface<CSchemaSystem>("schemasystem.dll", "SchemaSystem_0");
        if (!sdk::g_schema)
        {
            fmt::print("Failed to obtain Schema interface.");
            gen_status = GenStatus::kFailed;
            return;
        }

        fmt::print("{}: Dumping SDK for Source 2\n", __FUNCTION__);

        // Generate SDK
        const auto type_scopes = sdk::g_schema->GetTypeScopes();
        for (auto i = 0; i < type_scopes.Count(); ++i)
        {
            const auto& current = type_scopes.m_pElements[i];
            sdk::GenerateTypeScopeSdk(current);
        }

        sdk::GenerateTypeScopeSdk(sdk::g_schema->GlobalTypeScope());

        gen_status = GenStatus::kFinished;
    }

    void WINAPI main(const HMODULE module)
    {
        auto console = std::make_unique<DebugConsole>();
        console->start("NEVERLOSE :: Source 2 Generator");

        std::jthread setup_thread(&Setup);

        while (gen_status != GenStatus::kFinished)
        {
            if (gen_status == GenStatus::kFailed) break;

            console->update();
            sleep_for(std::chrono::milliseconds(1));
        }

        if (gen_status == GenStatus::kFinished)
            fmt::print("Successful dumped Source 2 SDK, now you can safely close this console.\nVisit: neverlose.cc!\n");
        else if (gen_status == GenStatus::kFailed) 
            fmt::print("Something went wrong\n");

        console->stop();
        console.reset();
        FreeLibraryAndExitThread(module, EXIT_SUCCESS);
    }
} // namespace source2gen
