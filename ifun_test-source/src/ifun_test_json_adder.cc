//
// Created by gibo on 7/31/15.
//

#include "ifun_test_json_adder.h"
#include "ifun_test_game_data_accessor.h"
#include "ifun_test_exception.h"

using namespace ifun_test;

void DataJsonAdder::UserInfoToJson(Ptr<User> UserInfo, Json& TargetJson)
{
    if (!TargetJson.IsObject())
    {
        BOOST_THROW_EXCEPTION(IfunTestBaseException());
    }
    if (nullptr == UserInfo)
    {
        BOOST_THROW_EXCEPTION(IfunTestDBFetchException());
    }
    Json& UserInfoJson = TargetJson["UserInfo"];
    UserInfoJson.SetObject();
    UserInfoJson["Id"] = boost::lexical_cast<std::string>(UserInfo->Id());
    UserInfoJson["NickName"] = UserInfo->GetNickName();
    UserInfoJson["Dice"] = boost::lexical_cast<std::string>(UserInfo->GetDice());
    UserInfoJson["Gem"] = boost::lexical_cast<std::string>(UserInfo->GetGem());
    UserInfoJson["Gold"] = boost::lexical_cast<std::string>(UserInfo->GetGold());
    UserInfoJson["ManaShard"] = boost::lexical_cast<std::string>(UserInfo->GetManaShard());
    UserInfoJson["BoosterPack"] = boost::lexical_cast<std::string>(UserInfo->GetBoosterPack());

    UserInfoJson["ClearStages"] = UserInfo->GetClearStages()->GetList();
    UserInfoJson["Cards"] = UserInfo->GetHaveCards()->GetList();
    UserInfoJson["Characters"] = UserInfo->GetHaveCharacters()->GetList();

    Json DeckArray;
    DeckArray.SetArray();
    for (int Deck_i = 0 ; Deck_i < UserInfo->GetHaveDecks().Size() ; Deck_i++)
    {
        Ptr<Deck> CurrDeck = UserInfo->GetHaveDecks().GetAt(Deck_i);

        Json DeckInfo;

        DeckInfo["DeckName"] = CurrDeck->GetName();
        DeckInfo["DeckCharacter"] = CurrDeck->GetHero();
        DeckInfo["DeckCard"] = CurrDeck->GetCardList();

        DeckArray.PushBack(DeckInfo);
    }
    UserInfoJson["Decks"] = DeckArray;
}

void DataJsonAdder::MapToJson(std::map<string,string> &Map, Json& TargetJson)
{
    if (!TargetJson.IsObject())
    {
        BOOST_THROW_EXCEPTION(IfunTestBaseException());
    }
    for (auto val : Map)
    {
        TargetJson[val.first] = val.second;
    }
}

void DataBaseAccessor::RegisterAccountOnLogin(const std::string nick_name, const Ptr<Session> session)
{
    if (!session)
    {
        BOOST_THROW_EXCEPTION(IfunTestSessionException());
    }
    Ptr<Session> OldSession = AccountManager::FindLocalSession(nick_name);
    if (OldSession)
    {
        UnregisterAccountOnLogout(OldSession);
    }

    if (!AccountManager::CheckAndSetLoggedIn(nick_name, session))
    {
        BOOST_THROW_EXCEPTION(IfunTestAccountException());
    }
}

void DataBaseAccessor::UnregisterAccountOnLogout(const Ptr<Session> session)
{
    if (!session)
    {
        BOOST_THROW_EXCEPTION(IfunTestSessionException());
    }
    std::string account = AccountManager::FindLocalAccount(session);
    session->Close();
    AccountManager::SetLoggedOut(account);
}

bool DataBaseAccessor::IsCanEarnItem(const Ptr<User> &user_info, ShopItemType type, string hero_id, int amount, std::function<std::map<string,string>()> &process_earn) {

    bool bIsCanEarn = false;

    switch (type)
    {
        case ShopItemType::kCharacter:
        {
            if (string::npos == user_info->GetHaveCharacters()->GetList().find(hero_id)) {
                bIsCanEarn = true;
                process_earn = [&, user_info, hero_id]()->std::map<string,string>{
                    std::map<string,string> change_value;
                    user_info->GetHaveCharacters()->SetList(user_info->GetHaveCharacters()->GetList() + ", " + hero_id);
                    change_value["Characters"] = user_info->GetHaveCharacters()->GetList();
                    return change_value;
                };
            } else {
                bIsCanEarn = false;
            }
        } break;

        case ShopItemType::kGem:
        {
            bIsCanEarn = true;
            process_earn = [&, user_info, amount]()->std::map<string,string>{
                std::map<string,string> change_value;
                user_info->SetGem(user_info->GetGem() + amount);
                change_value["Gem"] = boost::lexical_cast<std::string>(user_info->GetGem());
                return change_value;
            };
        } break;

        case ShopItemType::kGold:
        {
            bIsCanEarn = true;
            process_earn = [&, user_info, amount]()->std::map<string,string>{
                std::map<string,string> change_value;
                user_info->SetGold(user_info->GetGold() + amount);
                change_value["Gold"] = boost::lexical_cast<std::string>(user_info->GetGold());
                return change_value;
            };
        } break;

        case ShopItemType::kBooster:
        {
            bIsCanEarn = true;
            process_earn = [&, user_info, amount]()->std::map<string,string>{
                std::map<string,string> change_value;
                user_info->SetBoosterPack(user_info->GetBoosterPack() + amount);
                change_value["BoosterPack"] = boost::lexical_cast<std::string>(user_info->GetBoosterPack());
                return change_value;
            };
        } break;
    }
    return bIsCanEarn;
}
bool DataBaseAccessor::IsCanUseCurrency(const Ptr<User> &user_info, ShopCurrencyType type, int amount, std::function<std::map<string,string>()> &process_use) {

    bool bIsCanUse = false;
    switch (type)
    {
        case ShopCurrencyType::kGem:
        {
            if (user_info->GetGem() >= amount) {
                bIsCanUse = true;
                process_use = [&, amount, user_info]()->std::map<string,string>{
                    std::map<string,string> change_value;
                    user_info->SetGem(user_info->GetGem() - amount);
                    change_value["Gem"] = boost::lexical_cast<std::string>(user_info->GetGem());
                    return change_value;
                };
            } else {
                bIsCanUse = false;
            }
        } break;

        case ShopCurrencyType::kGold:
        {
            if (user_info->GetGold() >= amount) {
                bIsCanUse = true;
                process_use = [&, amount, user_info]()->std::map<string,string>{
                    std::map<string,string> change_value;
                    user_info->SetGold(user_info->GetGold() - amount);
                    change_value["Gold"] = boost::lexical_cast<std::string>(user_info->GetGold());
                    return change_value;
                };
            } else {
                bIsCanUse = false;
            }
        } break;

        case ShopCurrencyType::kCash:
        {
            bIsCanUse = true;
            process_use = [&, amount, user_info]()->std::map<string,string>{
                std::map<string,string> change_value;
                return change_value;
            };
        }

    }
    return bIsCanUse;
}


bool DataBaseAccessor::IsCanBuyGoods(const Ptr<User> &user_info, const string &goods_id, std::function<std::map<string, string>()> &process_buy) {
    bool bIsProcessed = false;
    bool bCanBuyItem = false;

    if (string::npos != goods_id.find("hero")) {
        const ShopHeroInfoObject *ShopHeroInfoData = dynamic_cast<const ShopHeroInfoObject*>(GameDataAccessor::Instance().GetData(EGameDataTable::kShopHeroInfo, goods_id));
        if (nullptr == ShopHeroInfoData)
        {
            BOOST_THROW_EXCEPTION(IfunTestGameDataException());
        }
        ShopItemType ItemType = ShopItemType::kCharacter;
        std::string ItemPaymentType = ShopHeroInfoData->GetPaymentType();
        int ItemPrice = ShopHeroInfoData->GetPrice();
        bIsProcessed = true;
        std::function<std::map<string, string>()> function_use_currency;
        bool bIsCanUseCurrency = false;
        if (0 == ItemPaymentType.compare("gem")) {
            bIsCanUseCurrency = IsCanUseCurrency(user_info, ShopCurrencyType::kGem, ItemPrice, function_use_currency);
        } else if (0 == ItemPaymentType.compare("gold")) {
            bIsCanUseCurrency = IsCanUseCurrency(user_info, ShopCurrencyType::kGem, ItemPrice, function_use_currency);
        } else {
            BOOST_THROW_EXCEPTION(IfunTestBaseException());
        }
        if (bIsCanUseCurrency) {
            bCanBuyItem = true;
            std::function<std::map<string, string>()> function_earn_character;
            if (IsCanEarnItem(user_info, ItemType, ShopHeroInfoData->GetHeroId(), 1, function_earn_character))
            {
                process_buy = [&, function_use_currency, function_earn_character]()->std::map<string, string>{
                    std::map<string, string> changed_value;
                    std::map<string,string> use_map = function_use_currency();
                    for (auto used_info : use_map)
                    {
                        changed_value[used_info.first] = used_info.second;
                    }
                    std::map<string,string> earn_map = function_earn_character();
                    for (auto earn_info : earn_map)
                    {
                        changed_value[earn_info.first] = earn_info.second;
                    }
                    return changed_value;
                };
            }
        }
    } else {
        const ShopInfoObject *ShopInfoData = dynamic_cast<const ShopInfoObject*>(GameDataAccessor::Instance().GetData(EGameDataTable::kShopInfo, goods_id));

        if (nullptr == ShopInfoData)
        {
            BOOST_THROW_EXCEPTION(IfunTestGameDataException());
        }
        string ItemPaymentType = ShopInfoData->GetPaymenType();
        string ItemGoodsType = ShopInfoData->GetGoodsType();
        int ItemPrice = ShopInfoData->GetPrice();
        int ItemQuantity = ShopInfoData->GetQuantity();
        ShopItemType earn_item_type;
        std::function<std::map<string, string>()> function_use_currency;

        if (0 == ItemGoodsType.compare("gold")) {
            earn_item_type = ShopItemType::kGold;
        } else if (0 == ItemGoodsType.compare("booster")) {
            earn_item_type = ShopItemType::kBooster;
        } else if (0 == ItemGoodsType.compare("gem")) {
            earn_item_type = ShopItemType::kGem;
        } else {
            BOOST_THROW_EXCEPTION(IfunTestBaseException());
        }
        bool bIsCanUseCurrency = false;
        if (0 == ItemPaymentType.compare("gem")) {
            bIsCanUseCurrency = IsCanUseCurrency(user_info, ShopCurrencyType::kGem, ItemPrice, function_use_currency);
        } else if (0 == ItemPaymentType.compare("gold")) {
            bIsCanUseCurrency = IsCanUseCurrency(user_info, ShopCurrencyType::kGold, ItemPrice, function_use_currency);
        } else if (0 == ItemPaymentType.compare("cash")) {
            bIsCanUseCurrency = IsCanUseCurrency(user_info, ShopCurrencyType::kCash, ItemPrice, function_use_currency);
        }
        if (bIsCanUseCurrency)
        {
            bCanBuyItem = true;
            std::function<std::map<string, string>()> function_earn_item;
            if (IsCanEarnItem(user_info, earn_item_type, goods_id, ItemQuantity, function_earn_item))
            {
                process_buy = [&, function_use_currency, function_earn_item]()->std::map<string,string>{
                    std::map<string, string> changed_value;
                    std::map<string,string> use_map = function_use_currency();
                    for (auto used_info : use_map)
                    {
                        changed_value[used_info.first] = used_info.second;
                    }
                    std::map<string,string> earn_map = function_earn_item();
                    for (auto earn_info : earn_map)
                    {
                        changed_value[earn_info.first] = earn_info.second;
                    }
                    return changed_value;
                };
            }
        }
        bIsProcessed = true;
    }

    return bIsProcessed && bCanBuyItem;
}

Ptr<User> DataBaseAccessor::GetUserById(const std::string &user_uuid)
{
    return GetUserById(boost::lexical_cast<Object::Id>(user_uuid));
}

Ptr<User> DataBaseAccessor::GetUserById(const Object::Id &user_uuid)
{
    Ptr<User> FoundUser = User::Fetch(user_uuid);
    if (!FoundUser) {
        BOOST_THROW_EXCEPTION(IfunTestDBFetchException());
    }
    return FoundUser;
}

Ptr<User> DataBaseAccessor::CreateUser(const std::string nickname)
{
    if (nickname.length() == 0) {
        BOOST_THROW_EXCEPTION(IfunTestBaseException());
    }
    Ptr<User> CreatedUser = User::Create(nickname);
    if (!CreatedUser) {
        BOOST_THROW_EXCEPTION(IfunTestDBCreateException());
    }
    Ptr<const Json> json_data = ResourceManager::GetJsonData("default_user.json");
    if (!json_data) {
        BOOST_THROW_EXCEPTION(IfunTestBaseException());
    }

    Ptr<User::OpaqueData> opaque_data = User::CreateOpaqueDataFromJson((*json_data)["default_user"]);
    if (!opaque_data) {
        BOOST_THROW_EXCEPTION(IfunTestBaseException());
    }
    if (!CreatedUser->PopulateFrom(opaque_data)) {
        BOOST_THROW_EXCEPTION(IfunTestBaseException());
    }
    return CreatedUser;
}

Ptr<User> DataBaseAccessor::GetUserByNickName(const std::string &nick_name)
{
    Ptr<User> FoundUser = User::FetchByNickName(nick_name);
    if (!FoundUser) {
        BOOST_THROW_EXCEPTION(IfunTestDBFetchException());
    }
    return FoundUser;
}

Ptr<User> DataBaseAccessor::GetUserBySession(const Ptr<Session> &session)
{
    string nick_name = AccountManager::FindLocalAccount(session);
    Ptr<User> FoundUser = DataBaseAccessor::GetUserByNickName(nick_name);
    if (!FoundUser) {
        BOOST_THROW_EXCEPTION(IfunTestBaseException());
    }
    return FoundUser;
}

void DataBaseAccessor::DeleteUserDeck(Ptr<User> UserInfo, int DeckIdx)
{
    if (!UserInfo) {
        BOOST_THROW_EXCEPTION(IfunTestDBFetchException());
    }
    ArrayRef<Ptr<Deck> > DeckArray = UserInfo->GetHaveDecks();
    if (DeckArray.Size() > DeckIdx) {
        DeckArray.EraseAt(DeckIdx);
    }
    else {
        BOOST_THROW_EXCEPTION(IfunTestDeleteDeckIdxException());
    }
}

void DataBaseAccessor::SaveUserDeck(Ptr<User> UserInfo, std::string DeckName, std::string DeckChar, std::string DeckCard, int DeckIdx)
{
    if (!UserInfo) {
        BOOST_THROW_EXCEPTION(IfunTestDBFetchException());
    }
    ArrayRef<Ptr<Deck> > DeckArray = UserInfo->GetHaveDecks();
    Ptr<Deck> TargetDeck = nullptr;
    if (DeckArray.Size() > DeckIdx) {
        TargetDeck = DeckArray.GetAt(DeckIdx);
        if (!TargetDeck) {
            BOOST_THROW_EXCEPTION(IfunTestDBFetchException());
        }
    }
    else if (DeckArray.Size() == DeckIdx) {
        TargetDeck = Deck::Create();
        if (!TargetDeck) {
            BOOST_THROW_EXCEPTION(IfunTestDBCreateException());
        }
        DeckArray.PushBack(TargetDeck);
    }
    if (!TargetDeck) {
        BOOST_THROW_EXCEPTION(IfunTestSaveDeckIdxException());
    }
    TargetDeck->SetName(DeckName);
    TargetDeck->SetHero(DeckChar);
    TargetDeck->SetCardList(DeckCard);
}

void DataBaseAccessor::AddClearDungeon(Ptr<User> TargetUser, std::string DungeonName)
{
    if (!TargetUser) {
        BOOST_THROW_EXCEPTION(IfunTestDBFetchException());
    }
    if (DungeonName.length() == 0) {
        BOOST_THROW_EXCEPTION(IfunTestGameDataException());
    }
    if (!GameDataAccessor::Instance().GetData(EGameDataTable::kDungeonInfo, DungeonName)) {
        BOOST_THROW_EXCEPTION(IfunTestGameDataException());
    }

    Ptr<Stage> UserStages = TargetUser->GetClearStages();
    if (!UserStages) {
        BOOST_THROW_EXCEPTION(IfunTestDBFetchException());
    }
    UserStages->SetList(UserStages->GetList().append(DungeonName));
}

std::function<std::map<string, string>()> DataBaseAccessor::ProcessPurchase(Ptr<User> TargetUser, std::string GoodsId)
{
    std::function<std::map<string, string>()> buy_function;
    if (!IsCanBuyGoods(TargetUser, GoodsId, buy_function))
    {
        BOOST_THROW_EXCEPTION(IfunTestShopPaymentException());
    }

    return buy_function;
}