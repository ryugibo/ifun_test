//
// Created by gibo on 7/31/15.
//

#ifndef IFUN_TEST_JSON_ADDER_H
#define IFUN_TEST_JSON_ADDER_H

#include <funapi.h>
#include "ifun_test_object.h"

enum class ShopItemType {
    kGold,
    kCharacter,
    kGem,
    kBooster,
};

enum class ShopCurrencyType {
    kGold,
    kCash,
    kGem
};

namespace ifun_test{
    namespace DataJsonAdder{

        void UserInfoToJson(Ptr<User> UserInfo, Json& TargetJson);
        void MapToJson(std::map<string, string> &SourceMap, Json& TargetJson);
    }

    namespace DataBaseAccessor {
        void RegisterAccountOnLogin(const std::string nick_name, const Ptr<Session> session);
        void UnregisterAccountOnLogout(const Ptr<Session> session);

        Ptr<User> GetUserById(const std::string &);
        Ptr<User> GetUserById(const Object::Id &);

        Ptr<User> GetUserByNickName(const std::string &);

        Ptr<User> GetUserBySession(const Ptr<Session> &session);

        Ptr<User> CreateUser(const std::string nickname);

        void DeleteUserDeck(Ptr<User>, int);
        void SaveUserDeck(Ptr<User>, std::string DeckName, std::string DeckChar, std::string DeckCard, int DeckIdx);

        void AddClearDungeon(Ptr<User>, std::string);

        bool IsCanEarnItem(const Ptr<User> &user_info, ShopItemType type, string hero_id, int amount, std::function<std::map<string,string>()> &process_earn);
        bool IsCanUseCurrency(const Ptr<User> &user_info, ShopCurrencyType type, int amount, std::function<std::map<string,string>()> &process_use);
        bool IsCanBuyGoods(const Ptr<User> &user_info, const string &goods_id, std::function<std::map<string, string>()> &process_buy);

        std::function<std::map<string, string>()> ProcessPurchase(Ptr<User>, std::string);
    }
};


#endif //IFUN_TEST_JSON_ADDER_H
