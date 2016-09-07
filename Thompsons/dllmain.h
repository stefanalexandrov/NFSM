// dllmain.h : Declaration of module class.

class CThompsonsModule : public ATL::CAtlDllModuleT< CThompsonsModule >
{
public :
	DECLARE_LIBID(LIBID_ThompsonsLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_THOMPSONS, "{7141F20D-63A8-4D5C-9DFE-6D3B3806A82F}")
};

extern class CThompsonsModule _AtlModule;
