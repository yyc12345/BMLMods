#pragma once

#if defined(YYCMOD_BML_USED)
#include <BML/BMLAll.h>
typedef CKSTRING BML_CKSTRING;
typedef BOOL BML_BOOL;
#define BML_TOCKSTRING(s) (s)
#define BML_VISITOR (m_bml)
#elif defined(YYCMOD_BMLP_USED)
#include <BMLPlus/BMLAll.h>
typedef const char* BML_CKSTRING;
typedef CKBOOL BML_BOOL;
#define BML_TOCKSTRING(s) TOCKSTRING(s)
#define BML_VISITOR (m_BML)
#else
#error "UNKNOW BML!"
#endif

#include <Windows.h>
#include <string>
#include <functional>
#include <vector>

namespace YYCHelper {
	namespace StringHelper {

		void StdstringPrintf(std::string& strl, const char* format, ...);
		void StdstringVPrintf(std::string& strl, const char* format, va_list argptr);

		bool CKStringEqual(CKSTRING str1, CKSTRING str2);

	}

	namespace EncodingHelper {

		bool WcharToChar(const wchar_t* src, std::string& dest, const UINT codepage);
		bool WcharToChar(const std::wstring& src, std::string& dest, const UINT codepage);

		bool CharToWchar(const char* src, std::wstring& dest, const UINT codepage);
		bool CharToWchar(const std::string& src, std::wstring& dest, const UINT codepage);

		bool CharToChar(const char* src, std::string& dest, const UINT src_codepage, const UINT dest_codepage);
		bool CharToChar(const std::string& src, std::string& dest, const UINT src_codepage, const UINT dest_codepage);

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

