#include "PrecompCommon.h"

//#include "RenderOverlay.h"
//
//#include "detours.h"
//
//#include <guichan/sfml.hpp>
//
//#define CINTERFACE
//#include <d3d9.h>
//#include <d3dx9.h>
//#pragma comment(lib, "d3d9.lib")
////#pragma comment(lib, "d3dx9.lib")
//
//const long XP_OFFSET_DEVICE_PRESENT = 0x00040EA0;
//const long XP_OFFSET_SWAP_CHAIN_PRESENT = 0x00039230;
//
//const long VISTA_OFFSET_DEVICE_PRESENT = 0x00048CDB;
//const long VISTA_OFFSET_SWAP_CHAIN_PRESENT = 0x00048DB4;
//
//unsigned char EXPECTED_OPCODES_DEVICE_PRESENT[5] = {0x8B, 0xFF, 0x55, 0x8B, 0xEC};
//unsigned char EXPECTED_OPCODES_SWAP_CHAIN_PRESENT[5] = {0x8B, 0xFF, 0x55, 0x8B, 0xEC};
//
//void* address_DevicePresent = (void*)0xe10ab04f;
////void* address_SwapChainPresent;
//
//unsigned char backup_DevicePresent[5];
//unsigned char patch_DevicePresent[5];
//unsigned char backup_SwapChainPresent[5];
//unsigned char patch_SwapChainPresent[5];
//
//bool patches_created = false;
//bool resources_created = false;
//
////////////////////////////////////////////////////////////////////////////
//
//class RenderOverlayDirectX : public RenderOverlay
//{
//public:
//	bool Initialize();
//	void PostInitialize();
//	void Update();
//	void Render();
//
//	void SetColor(const obColor &col);
//
//	void PushMatrix();
//	void PopMatrix();
//	void Translate(const Vector3f &v);
//
//	void DrawLine(const Vector3f &p1, const Vector3f &p2);
//	void DrawPolygon(const Vector3List &verts);
//	void DrawRadius(const Vector3f p, float radius);
//	void DrawAABB(const AABB &aabb);
//
//	~RenderOverlayDirectX();
//};
//
////////////////////////////////////////////////////////////////////////////
//// Hooked functions
//
//
//namespace Hooked
//{
//	
//};
//
//__declspec(naked) void* ResolveVirtualFunction(IDirect3DDevice9* pDevice, ...) {
//	__asm {
//		mov eax, dword ptr ss:[esp+0x08]
//		add eax, 0x8
//			cmp byte ptr ds:[eax-1], 0xA0
//			mov eax, dword ptr ds:[eax]
//		je normal_index
//			and eax, 0xFF
//normal_index:
//		mov ecx, eax
//			mov eax, dword ptr ss:[esp+0x4]
//		mov eax, dword ptr ds:[eax]
//		mov eax, dword ptr ds:[eax+ecx]
//		retn
//	}
//}
//
//void WriteHook(void* address, unsigned char* patch) 
//{
//	// This will write the five-byte buffer at 'patch' to 'address',
//	// after making sure that it's safe to write there
//
//	// Set access
//	DWORD old_protect = 0;
//	if (VirtualProtect(address, 5, PAGE_EXECUTE_READWRITE, &old_protect) == FALSE) return;
//	if (IsBadWritePtr(address, 5) != FALSE) return;
//
//	memcpy(address, reinterpret_cast<void*> (patch), 5);
//}
//
//HRESULT __stdcall DevicePresentHook(IDirect3DDevice9* device, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) {
//	// The hook function for IDirect3DDevice9::Present
//
//	//PrePresent(device);
//	WriteHook(address_DevicePresent, backup_DevicePresent);
//	HRESULT return_value = device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
//	WriteHook(address_DevicePresent, patch_DevicePresent);
//	//PostPresent(device);
//
//	return return_value;
//}
//
////HRESULT __stdcall SwapChainPresentHook(IDirect3DSwapChain9* swap_chain, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags) {
////	// The hook function for IDirect3DSwapChain9::Present
////
////	IDirect3DDevice9* device = NULL;
////	if FAILED(swap_chain->GetDevice(&device)) {
////		// This is pretty unlikely, but it doesn't pay to gamble
////		return swap_chain->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
////	}
////
////	//PrePresent(device);
////	WriteHook(address_SwapChainPresent, backup_SwapChainPresent);
////	HRESULT return_value = swap_chain->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
////	WriteHook(address_SwapChainPresent, patch_SwapChainPresent);
////	//PostPresent(device);
////
////	return return_value;
////}
//bool VerifyAddresses() {
//	// We can't afford to go overwriting arbitrary memory addresses without being
//	// absolutely sure that they are indeed the right functions
//
//	if (memcmp(backup_DevicePresent, EXPECTED_OPCODES_DEVICE_PRESENT, 5) != 0) return false;
//	//if (memcmp(backup_SwapChainPresent, EXPECTED_OPCODES_SWAP_CHAIN_PRESENT, 5) != 0) return false;
//
//	return true;
//}
//
////////////////////////////////////////////////////////////////////////////
//
//bool RenderOverlayDirectX::Initialize()
//{
//	// We define this as a ThreadProc so that it may be used with CreateRemoteThread
//	// This function may be called many times, but must succeed at least once for the hooks to work
//
//	IDirect3DDevice9 * d3dDevice = 0;
//
//	IDirect3D9 * d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
//	if(!d3d9)
//		return false;
//
//	D3DPRESENT_PARAMETERS present_parameters;
//	ZeroMemory(&present_parameters, sizeof(present_parameters));
//	present_parameters.Windowed = true;
//	present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
//	present_parameters.EnableAutoDepthStencil = true;
//	present_parameters.AutoDepthStencilFormat = D3DFMT_D16;
//	present_parameters.hDeviceWindow = GetActiveWindow();
//	present_parameters.BackBufferWidth = 800;
//	present_parameters.BackBufferHeight = 600;
//	present_parameters.BackBufferFormat = D3DFMT_R5G6B5;
//	present_parameters.MultiSampleType = D3DMULTISAMPLE_NONE;
//	IDirect3D9_CreateDevice
//	if( FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,
//		GetActiveWindow(), D3DCREATE_SOFTWARE_VERTEXPROCESSING,
//		&present_parameters,&d3dDevice)))
//	{
//		return false;
//	}
//
//	address_DevicePresent = ResolveVirtualFunction(d3dDevice, &IDirect3DDevice9::Present);
//	//address_SwapChainPresent = ResolveVirtualFunction(d3dDevice, &IDirect3DDevice9::SwapChains);
//
//	d3dDevice->Release();
//	d3d9->Release();
//
//	// Look for a suitable DLL
//	char* address_d3d9 = reinterpret_cast<char*> (GetModuleHandleA("d3d9.dll"));
//	if (address_d3d9 == NULL)
//		return false;
//
//	/*DWORD offset_device_present = 0;
//	DWORD offset_swap_chain_present = 0;*/
//
//	// Identify OS
//	/*OSVERSIONINFO version_info;
//	version_info.dwOSVersionInfoSize = sizeof(version_info);
//	GetVersionEx(&version_info);*/
//
//	//if (version_info.dwMajorVersion == 5) {
//	//	// XP
//	//	offset_device_present = XP_OFFSET_DEVICE_PRESENT;
//	//	offset_swap_chain_present = XP_OFFSET_SWAP_CHAIN_PRESENT;
//	//} else if (version_info.dwMajorVersion == 6) {
//	//	// Vista
//	//	offset_device_present = VISTA_OFFSET_DEVICE_PRESENT;
//	//	offset_swap_chain_present = VISTA_OFFSET_SWAP_CHAIN_PRESENT;
//	//} else {
//	//	return false;
//	//}
//
//	// Calculate addresses
//	//address_DevicePresent = reinterpret_cast<void*> (address_d3d9 + offset_device_present);
//	//address_SwapChainPresent = reinterpret_cast<void*> (address_d3d9 + offset_swap_chain_present);
//
//	// Create backups
//	if (!patches_created)
//	{
//		DWORD old_protect = 0;
//
//		DWORD lastError = 0; 
//		if (!VirtualProtect(address_DevicePresent, 5, PAGE_EXECUTE_READWRITE, &old_protect)) 
//		{
//			lastError = GetLastError();
//			return false;
//		}
//		memcpy(backup_DevicePresent, address_DevicePresent, 5);
//
//		/*if (!VirtualProtect(address_SwapChainPresent, 5, PAGE_EXECUTE_READWRITE, &old_protect))
//		{
//			lastError = GetLastError();
//			return false;
//		}
//		memcpy(backup_SwapChainPresent, address_SwapChainPresent, 5);*/
//
//		if (!VerifyAddresses())
//			return false;
//
//		// We need the DLL to add a reference to itself here to prevent it being unloaded
//		// before the process terminates, as there is no way to guarantee that unloading won't
//		// occur at an inopportune moment and send everything down the pan.
//		// This means that the DLL can't ever be unloaded, but that's the price we pay.
//		/*char file_name_self[MAX_PATH];
//		GetModuleFileName(module_self, file_name_self, MAX_PATH);
//		LoadLibrary(file_name_self);*/
//
//		// Create patches
//		{
//			DWORD from_int, to_int, offset;
//
//			// Device::Present
//			from_int = reinterpret_cast<DWORD> (address_DevicePresent);
//			to_int = reinterpret_cast<DWORD> (&DevicePresentHook);
//			offset = to_int - from_int - 5;
//
//			patch_DevicePresent[0] = 0xE9; // Rel32 JMP
//			*(reinterpret_cast<DWORD*> (patch_DevicePresent + 1)) = offset;
//
//			// SwapChain::Present
//			//from_int = reinterpret_cast<DWORD> (address_SwapChainPresent);
//			//to_int = reinterpret_cast<DWORD> (&SwapChainPresentHook);
//			//offset = to_int - from_int - 5;
//
//			//patch_SwapChainPresent[0] = 0xE9; // Rel32 JMP
//			//*(reinterpret_cast<DWORD*> (patch_SwapChainPresent + 1)) = offset;
//		}
//
//		patches_created = true;
//	}
//
//	// Install hooks
//	WriteHook(address_DevicePresent, patch_DevicePresent);
//	//WriteHook(address_SwapChainPresent, patch_SwapChainPresent);
//
//	return true;
//}
//
//RenderOverlayDirectX::~RenderOverlayDirectX()
//{
//	// Remove all patches
//	WriteHook(address_DevicePresent, backup_DevicePresent);
//	//WriteHook(address_SwapChainPresent, backup_SwapChainPresent);
//
//	// Reset access
//	DWORD old_protect = 0;
//	DWORD new_protect = PAGE_EXECUTE_READ;
//	VirtualProtect(address_DevicePresent, 5, new_protect, &old_protect);
//	//VirtualProtect(address_SwapChainPresent, 5, new_protect, &old_protect);
//}
//
//void RenderOverlayDirectX::PostInitialize()
//{
//	// overlays are see through
//	DW.mTop->setOpaque(false);
//	DW.Hud.mContainer->setOpaque(false);
//	DW.Goals.mContainer->setOpaque(false);
//	DW.Script.mContainer->setOpaque(false);
//	DW.Log.mContainer->setOpaque(false);
//	DW.StateTree.mContainer->setOpaque(false);
//	DW.Profiler.mContainer->setOpaque(false);
//}
//
//void RenderOverlayDirectX::PushMatrix()
//{
//	//glPushMatrix();
//}
//
//void RenderOverlayDirectX::PopMatrix()
//{
//	//glPopMatrix();
//}
//
//void RenderOverlayDirectX::Translate(const Vector3f &v)
//{
//	//glTranslatef(v.x,v.y,v.z);
//}
//
//void RenderOverlayDirectX::Render()
//{
//	//glPushAttrib(GL_ALL_ATTRIB_BITS);
//
//	//glDisable(GL_LIGHTING);
//	//glDisable(GL_CULL_FACE);
//	//glEnable(GL_DEPTH_TEST);
//	//glDisable(GL_TEXTURE_2D);
//
//	//glEnable(GL_SCISSOR_TEST);
//	//glPointSize(2.0);
//	//glLineWidth(2.0);
//
//	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	//glMatrixMode(GL_TEXTURE);
//	//glPushMatrix();
//	//glLoadIdentity();
//
//	//glMatrixMode(GL_PROJECTION);
//	//glPushMatrix();
//	//glLoadIdentity();
//	//trueglLoadMatrixf(Matrices.Projection);
//
//	///*static float fov = 90.f;
//	//GLdouble aspect = (GLdouble)ViewPort.width / (GLdouble)ViewPort.height;
//	//gluPerspective(fov,aspect,1,5000);*/
//
//	//glMatrixMode(GL_MODELVIEW);
//	//glPushMatrix();
//	//glLoadIdentity();
//	//trueglLoadMatrixf(Matrices.ModelView);
//	///*trueglViewPort(ViewPort.x,ViewPort.y,ViewPort.width,ViewPort.height);
//
//	//gluLookAt(p.x,p.y,p.z,look.x,look.y,look.z,0,0,1);*/
//
//	//RenderOverlayUser::OverlayRenderAll(this,Viewer);
//
//	////glColor3f(1.f,0.f,1.f);
//
//
//	///*glTranslatef(876.548f, 675.407f, 80.125f);
//
//	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
//	//GLUquadric *q = gluNewQuadric();
//	//gluQuadricOrientation(q,GLU_OUTSIDE);
//	//gluCylinder(q,32,32,72,20,1);
//	//gluQuadricOrientation(q,GLU_INSIDE);
//	//gluCylinder(q,32,32,72,20,1);
//	//gluDeleteQuadric(q);*/
//
//	///*glBegin(GL_TRIANGLES);
//	//glPushMatrix();
//	//glScalef(0.2f,0.2f,0.2f);
//	//for(int t = 0; t < TestTris.size(); ++t)
//	//{
//	//const Triangle &tri = TestTris[t];
//
//	//glVertex3fv(TestVerts[tri.i0]);
//	//glVertex3fv(TestVerts[tri.i1]);
//	//glVertex3fv(TestVerts[tri.i2]);
//	//}
//	//glEnd();
//	//glPopMatrix();*/
//
//	////////////////////////////////////////////////////////////////////////////
//
//	//glMatrixMode(GL_MODELVIEW);
//	//glPopMatrix();
//
//	//glMatrixMode(GL_TEXTURE);
//	//glPopMatrix();
//
//	//glMatrixMode(GL_PROJECTION);
//	//glPopMatrix();
//
//	//glPopAttrib();
//
//	//DW.Core.mGui->draw();
//	////#ifdef ENABLE_DEBUG_WINDOW
//	////	DW.Update();
//	////	if(g_Gui)
//	////	{
//	////		Prof(debug_window);
//	////
//	////		g_Gui->getTop()->setSize(g_Window->GetWidth(), g_Window->GetHeight());
//	////
//	////		sf::Event Event;
//	////		while(g_Window->GetEvent(Event))
//	////			g_Input->pushInput(Event);
//	////
//	////		g_Gui->logic();
//	////
//	////		//GameContext::RestoreGui();
//	////		//g_Window->BeginOpenGL();
//	////		g_Gui->draw();
//	////		//g_Window->EndOpenGL();
//	////
//	////		//g_Window->Display();
//	////		//GameContext::RestoreGame();
//	////	}
//	////#else
//	////	Prof_update(0);
//	////#endif
//
//	///*glPushAttrib(GL_ALL_ATTRIB_BITS);
//
//	//g_Graphics->_beginDraw();
//
//	//gcn::Rectangle clip;
//	//clip.x = clip.y = 0;
//	//clip.width = 800;
//	//clip.height = 600;
//	//g_Graphics->pushClipArea(clip);
//	//g_Graphics->drawText("DEEZE NUTS",10,10);
//	//g_Graphics->popClipArea();
//
//	//g_Graphics->_endDraw();
//
//	//glPopAttrib();*/
//
//	//CountProjection = 0;
//	//CountModelView = 0;
//}
//
//void RenderOverlayDirectX::Update()
//{
//	/*Utils::GetLocalEyePosition(Viewer.EyePos);
//	Utils::GetLocalFacing(Viewer.Facing);
//
//	Render();*/
//}
//
//void RenderOverlayDirectX::SetColor(const obColor &col)
//{
//	//glColor4f(col.rF(),col.gF(),col.bF(),col.aF());
//}
//
//void RenderOverlayDirectX::DrawLine(const Vector3f &p1, const Vector3f &p2)
//{
//	/*glBegin(GL_LINES);
//	glVertex3fv(p1);
//	glVertex3fv(p2);
//	glEnd();*/
//}
//
//void RenderOverlayDirectX::DrawPolygon(const Vector3List &verts)
//{
//	/*if(!verts.empty())
//	{
//		glBegin(GL_TRIANGLE_FAN);
//		for(obuint32 i = 0; i < verts.size(); ++i)
//		{
//			glVertex3fv(verts[i]);
//		}
//		glEnd();
//	}	*/
//}
//
//void RenderOverlayDirectX::DrawRadius(const Vector3f p, float radius)
//{
//	//glPushMatrix();
//	//glTranslatef(p.x, p.y, p.z);
//
//	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
//
//	//GLUquadric *q = gluNewQuadric();
//	//gluQuadricOrientation(q,GLU_OUTSIDE);
//	//gluDisk(q,radius,radius,16,1);
//	////gluQuadricOrientation(q,GLU_INSIDE);
//	////gluCylinder(q,32,32,72,20,1);
//	//gluDeleteQuadric(q);
//
//	//glPopMatrix();
//}
//
//void RenderOverlayDirectX::DrawAABB(const AABB_t &aabb)
//{
//	//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
//
//	//float verts[8*3] =
//	//{
//	//	aabb.m_Mins[0], aabb.m_Mins[1], aabb.m_Mins[2],
//	//	aabb.m_Maxs[0], aabb.m_Mins[1], aabb.m_Mins[2],
//	//	aabb.m_Maxs[0], aabb.m_Mins[1], aabb.m_Maxs[2],
//	//	aabb.m_Mins[0], aabb.m_Mins[1], aabb.m_Maxs[2],
//	//	aabb.m_Mins[0], aabb.m_Maxs[1], aabb.m_Mins[2],
//	//	aabb.m_Maxs[0], aabb.m_Maxs[1], aabb.m_Mins[2],
//	//	aabb.m_Maxs[0], aabb.m_Maxs[1], aabb.m_Maxs[2],
//	//	aabb.m_Mins[0], aabb.m_Maxs[1], aabb.m_Maxs[2],
//	//};
//	//static const float dim[6] =
//	//{
//	//	0.95f, 0.55f, 0.65f, 0.85f, 0.65f, 0.85f, 
//	//};
//	//static const unsigned char inds[6*5] =
//	//{
//	//	0,  7, 6, 5, 4,
//	//	1,  0, 1, 2, 3,
//	//	2,  1, 5, 6, 2,
//	//	3,  3, 7, 4, 0,
//	//	4,  2, 6, 7, 3,
//	//	5,  0, 4, 5, 1,
//	//};
//
//	//glBegin(GL_QUADS);
//	//const unsigned char* in = inds;
//	//for (int i = 0; i < 6; ++i)
//	//{
//	//	//float d = dim[*in]; in++;
//	//	glVertex3fv(&verts[*in*3]); in++;
//	//	glVertex3fv(&verts[*in*3]); in++;
//	//	glVertex3fv(&verts[*in*3]); in++;
//	//	glVertex3fv(&verts[*in*3]); in++;
//	//}
//	//glEnd();
//}
//
////////////////////////////////////////////////////////////////////////////
//
//RenderOverlay *CreateDXRenderOverlay()
//{
//	return new RenderOverlayDirectX;
//}
