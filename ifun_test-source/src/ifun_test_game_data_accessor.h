//
// Created by gibo on 8/12/15.
//

#ifndef IFUN_TEST_GAME_DATA_ACCESSOR_H
#define IFUN_TEST_GAME_DATA_ACCESSOR_H

#include "ifun_test_game_data_objects.h"

namespace ifun_test{
	class GameDataAccessor {

	private:

		GameDataAccessor();

		void InitData();
		void SetTableData(EGameDataTable::Type table_type);

		bool _bIsInitData;
		std::map<EGameDataTable::Type, std::map<std::string, GameDataObject *>> _GameData;

	public:

		static GameDataAccessor &Instance() {
			static GameDataAccessor instance;
			return instance;
		}

		const GameDataObject *GetData(EGameDataTable::Type DataTable, std::string Id);
	};
}

#endif //IFUN_TEST_GAME_DATA_ACCESSOR_H
