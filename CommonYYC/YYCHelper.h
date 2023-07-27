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

// VERSION DEFINE
#define YYCMOD_VERSION_BLINGUIST "1.0.0"
#define YYCMOD_VERSION_FONTCRAFT "2.0.0"
#define YYCMOD_VERSION_BASECMOCFG "1.0.0"
#define YYCMOD_VERSION_BALLANCEOPTIFINE "1.0.0"
#define YYCMOD_VERSION_EXTRASECTOR "1.0.0"

namespace YYCHelper {
	namespace StringHelper {

		void StdstringPrintf(std::string& strl, const char* format, ...);
		void StdstringVPrintf(std::string& strl, const char* format, va_list argptr);

		bool CKStringEqual(const char* str1, const char* str2);

	}

	namespace EncodingHelper {

		bool WcharToChar(const wchar_t* src, std::string& dest, const UINT codepage);
		bool WcharToChar(const std::wstring& src, std::string& dest, const UINT codepage);

		bool CharToWchar(const char* src, std::wstring& dest, const UINT codepage);
		bool CharToWchar(const std::string& src, std::wstring& dest, const UINT codepage);

		bool CharToChar(const char* src, std::string& dest, const UINT src_codepage, const UINT dest_codepage);
		bool CharToChar(const std::string& src, std::string& dest, const UINT src_codepage, const UINT dest_codepage);

	}

	namespace BMLPlusPatch {
#if defined(YYCMOD_BMLP_USED)
		class CKBehaviorIOWrapper : public CKBehaviorIO {
		public:
			XSObjectPointerArray* GetLinks();
		};
#endif
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

