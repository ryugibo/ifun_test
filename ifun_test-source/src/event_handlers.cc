// PLEASE ADD YOUR EVENT HANDLER DECLARATIONS HERE.

#include "event_handlers.h"

#include <funapi.h>
#include <glog/logging.h>

#include "ifun_test_loggers.h"
#include "ifun_test_messages.pb.h"

#include "ifun_test_json_adder.h"
#include "ifun_test_game_data_accessor.h"

#include "ifun_test_exception.h"

namespace ifun_test {

	////////////////////////////////////////////////////////////////////////////////
	// Session open/close handlers
	////////////////////////////////////////////////////////////////////////////////

	void OnSessionOpened(const Ptr<Session> &session) {
		logger::SessionOpened(to_string(session->id()), WallClock::Now());
		LOG(INFO) << "SESSION OPEN : " << WallClock::Now() << " - " << to_string(session->id());
	}


	void OnSessionClosed(const Ptr<Session> &session, SessionCloseReason reason) {
		logger::SessionClosed(to_string(session->id()), WallClock::Now());
		LOG(INFO) << "SESSION CLOSE : " << WallClock::Now() << " - " << to_string(session->id());

		if (reason == kClosedForServerDid) {
			// Server has called session->Close().
		} else {
			string nick_name = AccountManager::FindLocalAccount(session);
			AccountManager::SetLoggedOut(nick_name);
		}
		/*
		else if (reason == kClosedForIdle) {
			// The session has been idle for long time.
		}
		else if (reason == kClosedForUnknownSessionId) {
			// The session was invalid.
		}
		*/
	}

	////////////////////////////////////////////////////////////////////////////////
	// Rpc connect/disconnect  handlers
	////////////////////////////////////////////////////////////////////////////////
	void OnServerConnected(const Rpc::PeerId &peer) {
		Rpc::Tags	tags;
		Rpc::GetPeerTags(&tags, peer);

		
	}

	void OnServerDisconnected(const Rpc::PeerId &peer) {

	}

	////////////////////////////////////////////////////////////////////////////////
	// Client message handlers.
	//
	// (Just for your reference. Please replace with your own.)
	////////////////////////////////////////////////////////////////////////////////

	void OnAccountLogin(const Ptr<Session> &session, const Json &message) {
		// Thank to a JSON schema we specified registering a handler,
		// we are guaranteed that the passsed "message" is in the following form.
		//
		//   {"facebook_uid":"xxx", "facebook_access_token":"xxx"}
		//
		// So no more validation is necessary.
		string fb_uid = message["facebook_uid"].GetString();
		string fb_access_token = message["facebook_access_token"].GetString();

		// Below shows how to initiate Facebook authentication.
		AuthenticationKey fb_auth_key = MakeFacebookAuthenticationKey(fb_access_token);

		AccountAuthenticationRequest request("Facebook", fb_uid, fb_auth_key);
		AccountAuthenticationResponse response;
		if (not AuthenticateSync(request, &response)) {
			// system error
			LOG(ERROR) << "authentication system error";
			return;
		}

		if (response.success) {
			// login success
			LOG(INFO) << "login success";

			// You can have the Engine manage logged-in accounts through "AccountManager".
			AccountManager::CheckAndSetLoggedIn(fb_uid, session);

			// We also leave a player activity log.
			// This log can be used when you need to do customer services.
			// To customize an activity log, please refer to the reference manual.
			logger::PlayerLoggedIn(to_string(session->id()), fb_uid, WallClock::Now());

		}
		else {
			// login failure
			LOG(INFO) << "login failure";

		}
	}


	void OnEchoMessage(const Ptr<Session> &session, const Json &message) {
		// Since we have not specified a JSON schema for "echo" below,
		// the passed "message" may have malformed data.
		// We should manually validate it before use.
		//
		// Assuming "echo" is in the form below:
		//
		//   {"message":"xxx"}

		static const char *kMessage = "message";

		if (not message.HasAttribute(kMessage)) {
			LOG(ERROR) << "Invalid message";
			return;
		}

		if (not message[kMessage].IsString()) {
			LOG(ERROR) << "Invalid message";
			return;
		}

		// OK. The message looks good.
		string message_from_client = message["message"].GetString();

		// We sends the content back to the client.
		Json response;
		response[kMessage] = message_from_client;
		session->SendMessage("echo", response);

		LOG(INFO) << "message: " << message_from_client;
	}


	void OnPbufEchoMessage(const Ptr<Session> &session, const Ptr<FunMessage> &message) {
		// This is a Google protocol buffer example.
		// Engine invokes the handler only when the "message" is legitimate.
		// So, we do not have to check "required" fields.
		// But it's your responsibility to make sure required "optional" fields exist.
		if (not message->HasExtension(pbuf_echo)) {
			LOG(ERROR) << "Invalid message";
			return;
		}
		const PbufEchoMessage &pbuf_message = message->GetExtension(pbuf_echo);
		const string &msg = pbuf_message.msg();
		LOG(INFO) << "pbuf message: " << msg;

		// Constructs a response message.
		Ptr<FunMessage> response(new FunMessage);
		PbufEchoMessage *echo_response = response->MutableExtension(pbuf_echo);
		echo_response->set_msg(msg);

		// Wires the message.
		session->SendMessage("pbuf_echo", response);
	}

	void OnJoin(const Ptr<Session> &session, const Json &payload) {
		Json response;
		response["Result"] = "200";
		response["Reason"] = "Success";

		string nick_name_str = payload["NickName"].GetString();

		try {
			Ptr<User> CreatedUser = DataBaseAccessor::CreateUser(nick_name_str);

			DataBaseAccessor::RegisterAccountOnLogin(nick_name_str, session);

			DataJsonAdder::UserInfoToJson(CreatedUser, response);

		} catch (IfunTestBaseException &e) {
			response["Result"] = e.ErrorCode();
			response["Reason"] = e.ErrorReason();
		}
		session->SendMessage("result", response);

		logger::PlayerJoinIn(to_string(session->id()), nick_name_str, WallClock::Now());
		LOG(INFO) << "JOIN : " << WallClock::Now() << " - " << to_string(session->id()) << " - " << nick_name_str;
	}

	void OnLogin(const Ptr<Session> &session, const Json &payload) {
		Json response;
		response["Result"] = "200";
		response["Reason"] = "Success";

		string user_uuid_str = payload["Id"].GetString();

		try {
			Ptr<User> FoundUser = DataBaseAccessor::GetUserById(user_uuid_str);
			DataBaseAccessor::RegisterAccountOnLogin(FoundUser->GetNickName(), session);

			ifun_test::DataJsonAdder::UserInfoToJson(FoundUser, response);

		} catch (IfunTestBaseException &e) {
			response["Result"] = e.ErrorCode();
			response["Reason"] = e.ErrorReason();
		}
		session->SendMessage("result", response);

		logger::PlayerLoggedIn(to_string(session->id()), user_uuid_str, WallClock::Now());
		LOG(INFO) << "LOGIN : " << WallClock::Now() << " - " << to_string(session->id()) << " - " << user_uuid_str;
	}

	void OnLogout(const Ptr<Session> &session, const Json &payload) {
		DataBaseAccessor::UnregisterAccountOnLogout(session);

		logger::PlayerLoggedOut(to_string(session->id()), WallClock::Now());
		LOG(INFO) << "LOGOUT : " << WallClock::Now() << " - " << to_string(session->id());
	}

	void OnDeleteDeck(const Ptr<Session> &session, const Json &payload) {
		Json response;
		response["Result"] = "200";
		response["Reason"] = "Success";
to_string(session->id());
		string deck_idx_str = payload["DeckIdx"].GetString();
		int deck_idx = std::stoi(deck_idx_str);

		try {
			Ptr<User> FoundUser = DataBaseAccessor::GetUserBySession(session);
			DataBaseAccessor::DeleteUserDeck(FoundUser, deck_idx);
		} catch (IfunTestBaseException &e) {
			response["Result"] = e.ErrorCode();
			response["Reason"] = e.ErrorReason();
		}
		session->SendMessage("result", response);

		logger::PlayerDeleteDeck(to_string(session->id()), deck_idx_str);
		LOG(INFO) << "DELETE DECK : " << WallClock::Now() << " - " << to_string(session->id()) << " - " << deck_idx_str;
	}

	void OnSaveDeck(const Ptr<Session> &session, const Json &payload) {
		Json response;
		response["Result"] = "200";
		response["Reason"] = "Success";

		string deck_name_str = payload["DeckName"].GetString();
		string deck_char_str = payload["DeckCharacter"].GetString();
		string deck_card_str = payload["DeckCard"].GetString();
		string deck_idx_str = payload["DeckIdx"].GetString();
		int deck_idx = std::stoi(deck_idx_str);
		try {
			Ptr<User> FoundUser = DataBaseAccessor::GetUserBySession(session);
			DataBaseAccessor::SaveUserDeck(FoundUser, deck_name_str, deck_char_str, deck_card_str, deck_idx);
		} catch (IfunTestBaseException &e) {
			response["Result"] = e.ErrorCode();
			response["Reason"] = e.ErrorReason();
		}
		session->SendMessage("result", response);

		logger::PlayerSaveDeck(to_string(session->id()), deck_idx_str, deck_name_str, deck_char_str, deck_card_str);
		LOG(INFO) << "SAVE DECK : " << WallClock::Now() << " - " << to_string(session->id()) << " - " << deck_idx_str << " - " << deck_name_str << " - " << deck_char_str << " - " << deck_card_str;
	}

	void OnClearDungeon(const Ptr<Session> &session, const Json &payload) {
		Json response;
		response["Result"] = "200";
		response["Reason"] = "Success";
		std::string ParamClearDungeonName = payload["DungeonName"].GetString();

		try {
			Ptr<User> FoundUser = DataBaseAccessor::GetUserBySession(session);
			DataBaseAccessor::AddClearDungeon(FoundUser, ParamClearDungeonName);
		} catch (IfunTestBaseException &e) {
			response["Result"] = e.ErrorCode();
			response["Reason"] = e.ErrorReason();
		}

		session->SendMessage("result", response);

		logger::PlayerClearDungeon(to_string(session->id()), ParamClearDungeonName, WallClock::Now());
		LOG(INFO)<< "CLEAR DUNGEON : " << " - " << to_string(session->id()) << " - " << ParamClearDungeonName;
	}

	void OnBuyGoods(const Ptr<Session> &session, const Json &payload)
	{
		Json response;
		response["Result"] = "200";
		response["Reason"] = "Success";

		std::string GoodsId = payload["GoodsId"].GetString();
		try {
			Ptr<User> FoundUser = DataBaseAccessor::GetUserBySession(session);

			std::function<std::map<string, string>()> buy_function;
			buy_function = DataBaseAccessor::ProcessPurchase(FoundUser, GoodsId);

			std::map<string, string> changed_map = buy_function();

			Json &UpdateInfoObject = response["UpdateInfo"];
			UpdateInfoObject.SetObject();

			ifun_test::DataJsonAdder::MapToJson(changed_map, UpdateInfoObject);
		} catch (IfunTestBaseException &e) {
			response["Result"] = e.ErrorCode();
			response["Reason"] = e.ErrorReason();
		}

		session->SendMessage("result", response);

		logger::PlayerBuyGoods(to_string(session->id()), GoodsId);
		LOG(INFO) << "BUY GOODS : " << WallClock::Now() << " - " << to_string(session->id()) << " - " << GoodsId;
	}




	////////////////////////////////////////////////////////////////////////////////
	// Timer handler.
	//
	// (Just for your reference. Please replace with your own.)
	////////////////////////////////////////////////////////////////////////////////

	void OnTick(const Timer::Id &timer_id, const WallClock::Value &clock) {
		// PLACE HERE YOUR TICK HANDLER CODE.
	}

	////////////////////////////////////////////////////////////////////////////////
	// Rpc handler
	//
	// (Just for your reference. Please replace with your own.)
	////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////
	// Extend the function below with your handlers.
	////////////////////////////////////////////////////////////////////////////////
	void RegisterEventHandlers() {
		RegisterCommonEventHanders();
		{
			HandlerRegistry::Register("join", OnJoin);
			HandlerRegistry::Register("login", OnLogin);
			HandlerRegistry::Register("logout", OnLogout);
			HandlerRegistry::Register("save_deck", OnSaveDeck);
			HandlerRegistry::Register("delete_deck", OnDeleteDeck);
			HandlerRegistry::Register("clear_dungeon", OnClearDungeon);
			HandlerRegistry::Register("buy_goods", OnBuyGoods);
		}
		// For Initialize GameData.
		GameDataAccessor::Instance();
	}

	void RegisterCommonEventHanders() {
		{
			HandlerRegistry::Install2(OnSessionOpened, OnSessionClosed);
		}
	}

	void RegisterCommonRpcHandlers() {
		Rpc::RegisterConnectHandler(OnServerConnected);
		Rpc::RegisterDisconnectHandler(OnServerDisconnected);
	}  // namespace ifun_test
}