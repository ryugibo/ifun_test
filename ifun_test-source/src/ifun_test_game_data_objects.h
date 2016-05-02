//
// Created by gibo on 8/12/15.
//

#ifndef IFUN_TEST_GAME_DATA_OBJECTS_H
#define IFUN_TEST_GAME_DATA_OBJECTS_H

#include <funapi.h>

#include <boost/range/algorithm/remove_if.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

namespace ifun_test {
    namespace EGameDataTable {
        enum Type {
	        kAreaInfo,
	        kCardInfo,
	        kDungeonInfo,
	        kHeroInfo,
	        kMinionInfo,
	        kShopInfo,
	        kShopHeroInfo
        };

    }

    class GameDataObject {

    public:

	    GameDataObject(const Json &ParseTarget) { }

	    static std::string GetTableName(EGameDataTable::Type DataTableType) {
		    std::string table_name;
		    switch (DataTableType) {
			    case EGameDataTable::kAreaInfo: {
				    table_name = "area_info";
			    }
		            break;

			    case EGameDataTable::kCardInfo: {
				    table_name = "card_info";
			    }
		            break;

			    case EGameDataTable::kDungeonInfo: {
				    table_name = "dungeon_info";
			    }
		            break;

			    case EGameDataTable::kHeroInfo: {
				    table_name = "hero_info";
			    }
		            break;

			    case EGameDataTable::kMinionInfo: {
				    table_name = "minion_info";
			    }
		            break;

			    case EGameDataTable::kShopInfo: {
				    table_name = "shop_info";
			    }
		            break;

			    case EGameDataTable::kShopHeroInfo: {
				    table_name = "shop_hero_info";
			    }
		            break;
		    }
		    return table_name;
	    }

	    static GameDataObject *CreateGameDataObject(EGameDataTable::Type DataTableType, const Json &data);

	    virtual const std::string &GetIdColumnValue() = 0;

	    static void ParseArrayData(const std::string &Array, std::vector<std::string> &TargetVector) {
		    std::string ParseRes = Array;
		    ParseRes.erase(boost::remove_if(ParseRes, boost::is_any_of("()\"")), ParseRes.end());
		    if (ParseRes.size() != 0) {
			    boost::algorithm::split(TargetVector, ParseRes, boost::is_any_of(","), boost::token_compress_on);
		    }
	    }

    };

    class AreaInfoObject : public GameDataObject {

    private:

	    // Is Index
	    std::string _area_name;
	    std::vector<std::string> _dungeon_list;

    public:

	    AreaInfoObject(const Json &ParseTarget)
			    : GameDataObject(ParseTarget) {
		    _area_name = ParseTarget["AreaName"].GetString();

		    ParseArrayData(ParseTarget["DungeonList"].GetString(), _dungeon_list);
	    }

	    virtual const std::string &GetIdColumnValue() final { return GetAreaName(); }

	    const std::string &GetAreaName() const { return _area_name; }

	    const std::vector<std::string> &GetDungeonList() const { return _dungeon_list; }
    };

    class CardInfoObject : public GameDataObject {

    private:

	    std::string _card_key;
	    std::string _rarity;
	    std::string _category;
	    int _cost;
	    std::string _trigger_type;
	    std::string _spell_type;
	    bool _request_target;

    public:

	    CardInfoObject(const Json &ParseTarget)
			    : GameDataObject(ParseTarget) {
		    _card_key = ParseTarget["CardKey"].GetString();
		    _rarity = ParseTarget["Rarity"].GetString();
		    _category = ParseTarget["Category"].GetString();
		    _cost = ParseTarget["Cost"].GetInteger();
		    _trigger_type = ParseTarget["TriggerType"].GetString();
		    _spell_type = ParseTarget["SpellType"].GetString();
		    _request_target = (ParseTarget["RequestTarget"].GetString().compare("TRUE") == 0);
	    }

	    virtual const std::string &GetIdColumnValue() final { return GetCardKey(); }

	    const std::string &GetCardKey() const { return _card_key; }

	    const std::string &GetRarity() const { return _rarity; }

	    const std::string &GetCategory() const { return _category; }

	    const int &GetCost() const { return _cost; }

	    const std::string &GetTriggerType() const { return _trigger_type; }

	    const std::string &GetSpellType() const { return _spell_type; }

	    const bool &GetRequestTarget() const { return _request_target; }
    };

    class DungeonInfoObject : public GameDataObject {

    private:

	    std::string _dungeon_key;
	    std::string _map_name;
	    std::string _dungeon_type;
	    std::string _reward_type;
	    int _reward_value;
	    std::string _enemy_id;
	    std::string _open_condition_dungeon;
	    std::vector<std::string> _appear_card;

    public:

	    DungeonInfoObject(const Json &ParseTarget)
			    : GameDataObject(ParseTarget) {
		    _dungeon_key = ParseTarget["DungeonKey"].GetString();
		    _map_name = ParseTarget["MapName"].GetString();
		    _dungeon_type = ParseTarget["DungeonType"].GetString();
		    _reward_type = ParseTarget["RewardType"].GetString();
		    _reward_value = ParseTarget["RewardValue"].GetInteger();
		    _enemy_id = ParseTarget["EnemyId"].GetString();
		    _open_condition_dungeon = ParseTarget["OpenConditionDungeon"].GetString();

		    ParseArrayData(ParseTarget["AppearCard"].GetString(), _appear_card);
	    }

	    virtual const std::string &GetIdColumnValue() final { return GetDungeonKey(); }

	    const std::string &GetDungeonKey() const { return _dungeon_key; }

	    const std::string &GetMapName() const { return _map_name; }

	    const std::string &GetDungeonType() const { return _dungeon_type; }

	    const std::string &GetRewardType() const { return _reward_type; }

	    const int &GetRewardValue() const { return _reward_value; }

	    const std::string &GetEnemyId() const { return _enemy_id; }

	    const std::string &GetOpenConditionDungeon() const { return _open_condition_dungeon; }

	    const std::vector<std::string> &GetAppearCards() const { return _appear_card; }

    };

    class HeroInfoObject : public GameDataObject {

    private:

	    std::string _hero_id;
	    int _max_level;
	    int _attack;
	    int _defence;
	    int _health;
	    std::vector<std::string> _skill;
	    std::vector<std::string> _spell_category;

    public:

	    HeroInfoObject(const Json &ParseTarget)
			    : GameDataObject(ParseTarget) {
		    _hero_id = ParseTarget["HeroId"].GetString();
		    _max_level = ParseTarget["MaxLevel"].GetInteger();
		    _attack = ParseTarget["Attack"].GetInteger();
		    _defence = ParseTarget["Defence"].GetInteger();
		    _health = ParseTarget["Health"].GetInteger();

		    ParseArrayData(ParseTarget["Skill"].GetString(), _skill);
		    ParseArrayData(ParseTarget["SpellCategory"].GetString(), _spell_category);
	    }

	    virtual const std::string &GetIdColumnValue() final { return GetHeroId(); }

	    const std::string &GetHeroId() const { return _hero_id; }

	    const int &GetMaxLevel() const { return _max_level; }

	    const int &GetAttack() const { return _attack; }

	    const int &GetDefence() const { return _defence; }

	    const int &GetHealth() const { return _health; }

	    const std::string &GetSkill(int idx) const {
		    if (idx < 0 || idx >= _skill.size())
		    {
			    BOOST_ASSERT(false);
		    }
		    return _skill[idx];
	    }

	    bool IsContainSpellCategory(const std::string SpellCategory) {
		    auto itr = std::find(_spell_category.begin(), _spell_category.end(), SpellCategory);
		    if (itr == _spell_category.end())
		    {
			    return false;
		    }
		    return true;
	    }

    };

    class MinionInfoObject : public GameDataObject {

    private:

	    std::string _card_key;
	    int _level;
	    std::string _attribute;

    public:

	    MinionInfoObject(const Json &ParseTarget)
			    : GameDataObject(ParseTarget) {
		    _card_key = ParseTarget["CardKey"].GetString();
		    _level = ParseTarget["Level"].GetInteger();
		    _attribute = ParseTarget["Attribute"].GetString();
	    }

	    virtual const std::string &GetIdColumnValue() final { return GetCardKey(); }

	    const std::string &GetCardKey() const { return _card_key; }

	    const int &GetLevel() const { return _level; }

	    const std::string &GetAttribute() const { return _attribute; }

    };

    class ShopInfoObject : public GameDataObject {

    private:

	    std::string _goods_id;
	    std::string _goods_type;
	    int _quantity;
	    std::string _payment_type;
	    int _price;

    public:

	    ShopInfoObject(const Json &ParseTarget)
			    : GameDataObject(ParseTarget) {
		    _goods_id = ParseTarget["GoodsId"].GetString();
		    _goods_type = ParseTarget["GoodsType"].GetString();
		    _quantity = ParseTarget["Quantity"].GetInteger();
		    _payment_type = ParseTarget["PaymentType"].GetString();
		    _price = ParseTarget["Price"].GetInteger();
	    }

	    virtual const std::string &GetIdColumnValue() final { return GetGoodsId(); }

	    const std::string &GetGoodsId() const { return _goods_id; }

	    const std::string &GetGoodsType() const { return _goods_type; }

	    const int &GetQuantity() const { return _quantity; }

	    const std::string &GetPaymenType() const { return _payment_type; }

	    const int &GetPrice() const { return _price; }
    };

    class ShopHeroInfoObject : public GameDataObject {

    private:

	    std::string _goods_id;
	    std::string _hero_id;
	    int _quantity;
	    std::string _payment_type;
	    int _price;

    public:

	    ShopHeroInfoObject(const Json &ParseTarget)
			    : GameDataObject(ParseTarget) {
		    _goods_id = ParseTarget["GoodsId"].GetString();
		    _hero_id = ParseTarget["HeroId"].GetString();
		    _quantity = ParseTarget["Quantity"].GetInteger();
		    _payment_type = ParseTarget["PaymentType"].GetString();
		    _price = ParseTarget["Price"].GetInteger();
	    }

	    virtual const std::string &GetIdColumnValue() final { return GetGoodsId(); }

	    const std::string &GetGoodsId() const { return _goods_id; }

	    const std::string &GetHeroId() const { return _hero_id; }

	    const int &GetQuantity() const { return _quantity; }

	    const std::string &GetPaymentType() const { return _payment_type; }

	    const int &GetPrice() const { return _price; }

    };
}

#endif //IFUN_TEST_GAME_DATA_OBJECTS_H
