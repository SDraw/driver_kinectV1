#include "stdafx.h"

#include "Core/Core.h"

#ifdef _DEBUG
int main(void)
{
#else
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
#endif
    Core *l_core = new Core();
    if(l_core->Intitialize())
    {
        while(l_core->DoPulse());
        l_core->Terminate();
    }
    delete l_core;

    return EXIT_SUCCESS;
}