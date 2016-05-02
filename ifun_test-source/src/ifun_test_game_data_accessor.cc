//
// Created by gibo on 8/12/15.
//

#include "ifun_test_game_data_accessor.h"
using namespace ifun_test;

GameDataAccessor::GameDataAccessor()
	: _bIsInitData(false)
{
	InitData();
}

void GameDataAccessor::InitData()
{
	if (_bIsInitData)
	{
		return;
	}
	_bIsInitData = true;
	SetTableData(EGameDataTable::kAreaInfo);
	SetTableData(EGameDataTable::kCardInfo);
	SetTableData(EGameDataTable::kDungeonInfo);
	SetTableData(EGameDataTable::kHeroInfo);
	SetTableData(EGameDataTable::kMinionInfo);
	SetTableData(EGameDataTable::kShopInfo);
	SetTableData(EGameDataTable::kShopHeroInfo);
}

void GameDataAccessor::SetTableData(EGameDataTable::Type table_type)
{
	const std::string TableName = GameDataObject::GetTableName(table_type);

	Ptr<const Json> json_table = ResourceManager::GetJsonData(TableName);
	for (size_t i = 0; i < json_table->Size(); ++i)
	{
		const Json &row = (*json_table)[i];
		auto CreatedObject = GameDataObject::CreateGameDataObject(table_type, row);
		BOOST_ASSERT(CreatedObject);

		_GameData[table_type][CreatedObject->GetIdColumnValue()] = CreatedObject;
	}
}

const GameDataObject *GameDataAccessor::GetData(EGameDataTable::Type table_type, std::string Id)
{
	GameDataObject *FoundValue = nullptr;
	std::map<std::string, GameDataObject *>::iterator FoundItr = _GameData[table_type].find(Id);

	if (FoundItr != _GameData[table_type].end()) {
		FoundValue = FoundItr->second;
	}

	return FoundValue;
}