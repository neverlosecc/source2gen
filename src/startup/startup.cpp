#include <Include.h>
#include <sdk/sdk.h>

#include "tools/console/console.h"

namespace source2_gen
{
	std::atomic_bool is_finished = false;

	void Setup()
	{
		while (!GetModuleHandleA("GameOverlayRenderer64.dll") ||
			!GetModuleHandleA("client.dll") ||
			!GetModuleHandleA("engine2.dll") ||
			!GetModuleHandleA("steamclient64.dll") ||
			!GetModuleHandleA("schemasystem.dll"))
			sleep_for(std::chrono::seconds(5));

		fmt::print("{}: Found GameOverlayRenderer64\n", __FUNCTION__);

		sdk::g_schema = sdk::GetInterface<CSchemaSystem>("schemasystem.dll", "SchemaSystem_0");
		if (!sdk::g_schema) { throw std::runtime_error(fmt::format("Failed to obtain Schema interfaces.")); }

		fmt::print("{}: Dumping SDK for Source 2\n", __FUNCTION__);

		// Generate SDK
		const auto type_scopes = sdk::g_schema->GetTypeScopes();
		for (auto i = 0; i < type_scopes.Count(); ++i)
		{
			const auto& current = type_scopes.m_pElements[i];
			sdk::GenerateTypeScopeSdk(current);
		}

		sdk::GenerateTypeScopeSdk(sdk::g_schema->GlobalTypeScope());

		is_finished = true;
	}

	void WINAPI main(const HMODULE module)
	{
		auto console = std::make_unique<DebugConsole>();
		console->start("NEVERLOSE :: Source 2 Generator");

		std::jthread setup_thread(&Setup);

		while (!is_finished)
		{
			console->update();
			sleep_for(std::chrono::milliseconds(1));
		}

		fmt::print("Successfuly dumped Source 2 SDK, now you can safely close this console.\nVisit: neverlose.cc!\n");
		console->stop();
		console.reset();
		FreeLibraryAndExitThread(module, EXIT_SUCCESS);
	}
} // namespace source2gen
