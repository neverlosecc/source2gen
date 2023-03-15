#include <Include.h>

namespace {
	void OnProcessAttach(const HMODULE h_module)
	{
		if (std::thread main_thread([h_module] { source2_gen::main(h_module); }); main_thread.joinable())
		{
			main_thread.detach();
		}
	}

	void OnProcessDetach()
	{
		// Add any necessary clean-up code here
	}
}

BOOL APIENTRY DllMain(const HMODULE module, const DWORD reason, LPVOID reserved [[maybe_unused]] )
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH: OnProcessAttach(module);
		break;
	case DLL_PROCESS_DETACH: OnProcessDetach();
		break;
	default: break;
	}

	return TRUE;
}
