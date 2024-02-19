#include "YYCHelper.h"

namespace YYCHelper::DpiHelper {

#define CAST_TO_ROUND_INT(v) static_cast<int>(std::round(v))
	int GetDpiScaledFontSize(CKContext* ctx, float old_font_size) {
		// get window hwnd
		HWND blcwin_hwnd = reinterpret_cast<HWND>(ctx->GetPlayerRenderContext()->GetWindowHandle());
		if (blcwin_hwnd == INVALID_HANDLE_VALUE) 
			return CAST_TO_ROUND_INT(old_font_size);
		
		// get dpi
		HDC hdc = GetDC(blcwin_hwnd);
		if (hdc == NULL) 
			return CAST_TO_ROUND_INT(old_font_size);
		float dpi_scale_x = GetDeviceCaps(hdc, LOGPIXELSX) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
		float dpi_scale_y = GetDeviceCaps(hdc, LOGPIXELSY) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
		ReleaseDC(blcwin_hwnd, hdc);

		// if x y have different dpi, get average value
		// otherwise use it directly
		float dpi_scale = 0.0f;
		if (dpi_scale_x != dpi_scale_y) 
			dpi_scale = (dpi_scale_x + dpi_scale_y) / 2.0f;
		else
			dpi_scale = dpi_scale_x;

		// return rescaled one
		return CAST_TO_ROUND_INT(old_font_size / dpi_scale);
	}
#undef CAST_TO_ROUND_INT

}
