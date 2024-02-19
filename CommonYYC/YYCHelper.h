#pragma once

#if defined(YYCMOD_BML_USED)
#include <BML/BMLAll.h>
typedef CKSTRING YYCBML_CKSTRING;
typedef BOOL YYCBML_BOOL;
#define YYCBML_TOCKSTRING(s) (s)
#define YYCBML_VISITOR (m_bml)
#elif defined(YYCMOD_BMLP_USED)
#include <BMLPlus/BMLAll.h>
typedef const char* YYCBML_CKSTRING;
typedef CKBOOL YYCBML_BOOL;
#define YYCBML_TOCKSTRING(s) TOCKSTRING(s)
#define YYCBML_VISITOR (m_BML)
#else
#error "UNKNOW BML!"
#endif

#include <Windows.h>
#include <string>
#include <functional>
#include <vector>

/*
Version Bump up Priniciple:

Version string is constructed by this format: `major.minor.build`
* Increase `major` if the whole mod was refactored, or interface changed.
* Increase `minor` if some small patch or feature added.
* Increase `build` if no change but BML / BMLP version changed. 
  If we need do some changes to port to new BML / BMLP, do not increase this, increase `minor` or `major` instead.
* If a greater factor increased, all lower factors should be reset to 0.
*/

// VERSION DEFINE
#define YYCMOD_VERSION_BLINGUIST "1.3.0"
#define YYCMOD_VERSION_FONTCRAFT "4.1.0"
#define YYCMOD_VERSION_BASECMOCFG "1.0.1"
#define YYCMOD_VERSION_BALLANCEOPTIFINE "1.0.1"
#define YYCMOD_VERSION_EXTRASECTOR "1.0.1"
#define YYCMOD_VERSION_BALLANCEBBOR "1.0.0"

namespace YYCHelper {
	namespace StringHelper {

		void StdstringPrintf(std::string& strl, const char* format, ...);
		void StdstringVPrintf(std::string& strl, const char* format, va_list argptr);

		bool CKStringEqual(const char* str1, const char* str2);

	}

	namespace DpiHelper {
		/**
		 * @brief Convert 100% DPI font size to user DPI.
		 * @details See https://learn.microsoft.com/zh-cn/windows/win32/learnwin32/dpi-and-device-independent-pixels
		 * @param ctx[in] CKContext for fetching necessary window HANDLE.
		 * @param old_font_size[in] The font size input.
		 * @return A new font size with int type rescaled by Windows DPI settings.
		*/
		int GetDpiScaledFontSize(CKContext* ctx, float old_font_size);
	}

	namespace EncodingHelper {

		bool WcharToChar(const wchar_t* src, std::string& dest, const UINT codepage);
		bool WcharToChar(const std::wstring& src, std::string& dest, const UINT codepage);

		bool CharToWchar(const char* src, std::wstring& dest, const UINT codepage);
		bool CharToWchar(const std::string& src, std::wstring& dest, const UINT codepage);

		bool CharToChar(const char* src, std::string& dest, const UINT src_codepage, const UINT dest_codepage);
		bool CharToChar(const std::string& src, std::string& dest, const UINT src_codepage, const UINT dest_codepage);

	}

	namespace ColorStringParser {

		/**
		 * @brief Parse color string. Support hex digit and rgb.
		 * @param colstr[in] The input color string.
		 * @param value[out] The references receiving parsed color.
		 * @return true if success. false when failed and out value will be set to default color (white)
		*/
		bool ParseColorString(const char* colstr, VxColor& result);

	}

	/**
	 * The namespace which patch something different between BML and BMLP
	*/
	namespace BMLPatches {

		/**
		 * @brief Get links from CKBehaviorIO.
		 * @remark
		 * BML use some dirty way and expose this GetLinks interface.
		 * But BMLP not, BMLP more vanilla so we need patch it and support this function like BML.
		 * @return The links list.
		*/
		XSObjectPointerArray* CKBehaviorIO_GetLinks(CKBehaviorIO* that);

	}

	namespace ModEvents {

		template<typename TArg, typename TRet>
		class ModEventDispatcher {
		public:
			using ListenerCallback = std::function<void(TArg, TRet)>;
			using HostHandler = std::function<TRet(TArg)>;

		private:
			using ListenerPair = std::pair<TArg, ListenerCallback>;

			HostHandler mHostHandler;
			std::vector<ListenerPair> mListenerCallbacks;

		public:
			ModEventDispatcher() :
				mHostHandler(nullptr), mListenerCallbacks() {
				;
			}
			~ModEventDispatcher() {
				;
			}

			void RegisterHandler(HostHandler handler) {
				// set handler
				if (handler == nullptr) return;
				this->mHostHandler = handler;

				// process existed listener
				for (const auto& pair : mListenerCallbacks) {
					pair.second(pair.first, this->mHostHandler(pair.first));
				}
				mListenerCallbacks.clear();
			}
			void UnregisterHandler(void) {
				// set handler
				this->mHostHandler = nullptr;
			}
			void ListenEvent(ListenerCallback callback, TArg args) {
				if (this->mHostHandler == nullptr) {
					// push into list and wait host
					mListenerCallbacks.emplace_back(std::make_pair(args, callback));
				} else {
					// execute immediately
					callback(args, this->mHostHandler(args));
				}
			}

		};

	}

}

