//
// Created by gibo on 8/14/15.
//

#ifndef IFUN_TEST_EXCEPTION_H
#define IFUN_TEST_EXCEPTION_H

#include <boost/exception/all.hpp>

namespace ifun_test {
	class IfunTestBaseException : public boost::exception, public std::exception {

	public:
		IfunTestBaseException()
				: _ErrorCode("100"),
				  _ErrorReason("Unknown Error")
		{}

		IfunTestBaseException(const char *ErrorCode, const char *ErrorReason)
				: _ErrorCode(ErrorCode),
	              _ErrorReason(ErrorReason)
		{}

		const char *ErrorCode() const noexcept {
			return _ErrorCode;
		}
		const char *ErrorReason() const noexcept {
			return _ErrorReason;
		}

	private:

		const char *_ErrorCode;
		const char *_ErrorReason;
	};
}

#define DEFINE_IFUN_TEST_EXCEPTION(_NAME, _ERROR_CODE, _ERROR_REASON) \
namespace ifun_test { \
	class _NAME : public ifun_test::IfunTestBaseException { \
	public: \
		_NAME() \
		: ifun_test::IfunTestBaseException(_ERROR_CODE, _ERROR_REASON) {} \
	};\
}

// 100 : Not Processed Excpetion
// 200 : Successful

// Session
DEFINE_IFUN_TEST_EXCEPTION(IfunTestSessionException, "101", "Requested session is closed.");

// Gamedata Databse
DEFINE_IFUN_TEST_EXCEPTION(IfunTestGameDataException, "102", "Not found game data");

// AccountManager Exception
DEFINE_IFUN_TEST_EXCEPTION(IfunTestAccountException, "103", "Error on account manager");

// User Database
DEFINE_IFUN_TEST_EXCEPTION(IfunTestDBCreateException, "301", "Failed create DB data.");
DEFINE_IFUN_TEST_EXCEPTION(IfunTestDBFetchException, "302", "Failed fetch from DB data.");

/// * Protocol Custom Exception
// Dungeon Protocol
DEFINE_IFUN_TEST_EXCEPTION(IfunTestDungeonParamException, "401", "You can't clear this dungoen.");

// Shop Protocol
DEFINE_IFUN_TEST_EXCEPTION(IfunTestShopPaymentException, "401", "Not enough payment.");

// Save Deck Protocol
DEFINE_IFUN_TEST_EXCEPTION(IfunTestSaveDeckIdxException, "401", "Can't save that index.");
DEFINE_IFUN_TEST_EXCEPTION(IfunTestSaveDeckCardException, "402", "Included a card that you don't have.");
DEFINE_IFUN_TEST_EXCEPTION(IfunTestSaveDeckCharException, "403", "Included a character that you don't have.");

// Delete Deck Protocol
DEFINE_IFUN_TEST_EXCEPTION(IfunTestDeleteDeckIdxException, "401", "Can't save that index.");


#endif //IFUN_TEST_EXCEPTION_H
