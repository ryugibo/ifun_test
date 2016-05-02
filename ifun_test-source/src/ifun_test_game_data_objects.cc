//
// Created by gibo on 8/12/15.
//

#include "ifun_test_game_data_objects.h"

using namespace ifun_test;

GameDataObject *GameDataObject::CreateGameDataObject(EGameDataTable::Type DataTableType, const Json &data)
{
	GameDataObject *CreatedObject = nullptr;
	switch (DataTableType)
	{
		case EGameDataTable::kAreaInfo:
		{
			CreatedObject = new AreaInfoObject(data);
		} break;

		case EGameDataTable::kCardInfo:
		{
			CreatedObject = new CardInfoObject(data);
		} break;

		case EGameDataTable::kDungeonInfo:
		{
			CreatedObject = new DungeonInfoObject(data);
		} break;

		case EGameDataTable::kHeroInfo:
		{
			CreatedObject = new HeroInfoObject(data);
		} break;

		case EGameDataTable::kMinionInfo:
		{
			CreatedObject = new MinionInfoObject(data);
		} break;

		case EGameDataTable::kShopInfo:
		{
			CreatedObject = new ShopInfoObject(data);
		} break;

		case EGameDataTable::kShopHeroInfo:
		{
			CreatedObject = new ShopHeroInfoObject(data);
		} break;
	}
	return CreatedObject;
}