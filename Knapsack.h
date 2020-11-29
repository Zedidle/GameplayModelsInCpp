#include "iostream"
#include<sstream>
#include "algorithm"
#include "vector"
#include "map"
using namespace std;



namespace Knapsack
{
	struct Item;
	static map<int, Item*> Items;  // ID查询列表

	// 关于Item，最好有个数据表，可以明确配表；
	struct Item
	{
		int ID;
		string Name;
		int Limit = 1;
		float Wight = 1;

		Item(int id, string name, int limit = 1, float wight = 1) :ID(id), Name(name), Limit(limit), Wight(wight) {
			Items.insert(pair<int, Item*>(id, this));
		};
		~Item()
		{
			Items.erase(ID);
		}
		void SetLimit(int limit)
		{
			Limit = limit;
		}
		void SetWight(float wight)
		{
			Wight = wight;
		}
	};


	void ShowAllItems()
	{
		for (pair<int, Item*> item_pair : Items)
		{
			Item* item = item_pair.second;
			cout << item->ID << " " << item->Name << endl;
		}
	}


	struct Space
	{
		Space();
		Space(int itemID, int num=1): ItemID(itemID), Num(num){};
		//Space(int itemID, pair<int, int> place, int num=1): ItemID(itemID), Place(place), Num(num){};
		void SetItemID(int itemID);
		float AddNum(int num);

		void Clear();

		int ItemID;
		//pair<int, int> Place;
		int Num;

		void Swap(Space& space);
	};
	
	Space::Space()
	{
		ItemID = -1;
		Num = 0;
	}
	void Space::SetItemID(int itemID)
	{
		ItemID = itemID;
	}

	void Space::Clear()
	{
		ItemID = -1;
		Num = 0;
	}

	inline void Space::Swap(Space& space)
	{
		int id = ItemID;
		int num = Num;

		ItemID = space.ItemID;
		Num = space.Num;

		space.ItemID = id;
		space.Num = num;
	}

	float Space::AddNum(int num)
	{
		int sup = 0;
		if (ItemID >= 0)
		{
			Num += num;

			int limit = Items[ItemID]->Limit;
			//cout << "Space::AddNum ItemID, limit:" << ItemID << "," << limit << endl;
			if (Num > limit)
			{
				sup = Num - limit;
				Num = limit;
			}
		}

		return sup;
	}

	

	
	class Knapsack
	{
		public:
			Knapsack(int row, int line);
			vector<vector<Space>> Containers;
			int Row;
			int Line;

			float CurWight = 0;
			float LimitWight = 100;

			void Push(Item* item, int row, int line, int num = 1);
			void Pop(int row, int line, int num=1);
			void Move(int row, int line, int row2, int line2);
			void CheckAll();


		private:
			string GetPos(int row, int line);
			string GetWight();
			void IterPush(Item* item, int num = 1);
			int PushToSpace(Item* item, int row, int line, int Num);
			void PopFromSpace(int row, int line, int num);



	};



	Knapsack::Knapsack(int row, int line)
	{
		vector<vector<Space>> _Containers(row, vector<Space>(line));
		Containers = _Containers;

		Row = row;
		Line = line;
	}


	inline string Knapsack::GetPos(int row, int line)
	{
		string s = "(";
		s.insert(s.length(), to_string(row));
		s.insert(s.length(), ",");
		s.insert(s.length(), to_string(line));
		s.insert(s.length(), ")");
		return s;
	}

	inline string Knapsack::GetWight()
	{
		string s = "重量：";
		s.insert(s.length(), to_string(CurWight));
		s.insert(s.length(), "/");
		s.insert(s.size(), to_string(LimitWight));
		return s;
	}

	void Knapsack::IterPush(Item* item, int num)
	{
		for (int i = 0; i < Row; ++i)
		{
			for (int j = 0; j < Line; ++j)
			{
				if(num <= 0) return;
				Space& space = Containers[i][j];
				if (space.Num == 0)
				{
					space.SetItemID(item->ID);
					num = PushToSpace(item, i, j, num);
				}
				else if (space.ItemID == item->ID)
				{
					num = PushToSpace(item, i, j, num);
				}
			}
		}
		cout << "背包空间不足" << endl;
	}


	void Knapsack::Push(Item* item, int row, int line, int num)
	{
		if (row > Containers.size() - 1)
		{
			cout << "超出行数" << endl;
			return;
		}
		if (line > Containers[0].size() - 1)
		{
			cout << "超出列数" << endl;
			return;
		}

		Space& space = Containers[row][line];

		// 如果已经有物品
		if (space.Num)
		{
			// 所放物品相同，则直接加数量
			if (space.ItemID == item->ID)
			{
				IterPush(item, PushToSpace(item, row, line, num));
			}
			// 所放物品不同，则遍历检查背包，放到第一个空位，如果没有空位，则提示“背包空间不足，放入失败。”
			else
			{
				IterPush(item, num);
			}
		}
		else
		{
			space.SetItemID(item->ID);
			IterPush(item, PushToSpace(item, row, line, num));
		}
	}
	
	int Knapsack::PushToSpace(Item* item, int row, int line, int num)
	{
		Space& space = Containers[row][line];
		// 根据重量 计算可以容纳这个物品的数量
		float supWight = LimitWight - CurWight;
		int n = supWight / item->Wight;
		bool OutWight = n < num;

		cout <<  GetPos(row, line) <<"放入" << item->Name << " " << num << "个，"<< (OutWight?"超重;":"") << endl;
		n = min(n, num);
		int sup = space.AddNum(n);

		if (n > 0)
		{
			CurWight += (n - sup) * item->Wight;
			cout << "成功放入" << item->Name << " " << n-sup << "个，" << GetWight() << endl;
		}

		// 还能继续放同一东西
		if (item->Wight < LimitWight - CurWight)
		{
			return num - (n - sup);
		}
		else
		{
			return 0;
		}

	}



	void Knapsack::Pop(int row, int line, int num)
	{
		if(num < 1) return;
		if (row > Containers.size() - 1)
		{
			cout << "超出行数" << endl;
			return;
		}
		if (line > Containers[0].size() - 1)
		{
			cout << "超出列数" << endl;
			return;
		}

		PopFromSpace(row, line, num);
	}

	inline void Knapsack::Move(int row, int line, int row2, int line2)
	{
		cout << "交换位置：" << GetPos(row, line) << " <--> "<< GetPos(row2, line2);
		Containers[row][line].Swap(Containers[row2][line2]);
	}

	void Knapsack::PopFromSpace(int row, int line, int num)
	{
		Space& space = Containers[row][line];
		if(space.ItemID < 0) return;

		int spaceNum = space.Num;
		int n = min(space.Num, num);
		space.Num -= n;
		CurWight -= n * Items[space.ItemID]->Wight;
		cout << "拿出" << "(" << row << "," << line << ")的" << Items[space.ItemID]->Name << " " << n << "个,  " << GetWight() << endl;
		if (spaceNum < num)
		{
			space.Clear();
		}
	}


	void Knapsack::CheckAll()
	{
		cout << endl << endl << "CheckAll: " << endl;
		cout << "CurWight：" << CurWight << endl;
		for (int i = 0; i < Row; ++i)
		{
			for (int j = 0; j < Line; ++j)
			{
				if (Containers[i][j].Num > 0)
				{
					cout << "(" << i << "," << j << ")  " ;
					cout << "ID：" << Containers[i][j].ItemID << "  ";
					cout << "Num：" << Containers[i][j].Num << endl ;
				}
			}
		}
		cout << endl;
	}



	void Test()
	{
		Item* item0 = new Item(0,"Apple", 3, 0.2);
		Item* item1 = new Item(1,"Banana", 5, 0.1);
		Item* item2 = new Item(2,"Cat", 1, 5);
		Item* item3 = new Item(3,"Dog", 1, 7);
		Item* item4 = new Item(4,"Egg", 12, 0.1);
		Item* item5 = new Item(5,"Forge", 2, 2);
		Item* item6 = new Item(6,"Gold", 20, 3);
		//item1(1, "Banana"), item2(2,"Cat"), item3(3, "Dog"), item4(4, "Egg"), item5(5, "Forge"), item6(6, "Gold");

		//ShowAllItems();
		Knapsack* knapsack = new Knapsack(5, 5);

		knapsack->Push(item0, 2, 2, 4);
		knapsack->Push(item1, 3, 2, 9);
		knapsack->Push(item2, 4, 2, 2);
		knapsack->Push(item3, 2, 2, 1);
		knapsack->Push(item4, 0, 0, 10);
		knapsack->Push(item5, 4, 4, 3);
		knapsack->Push(item6, 2, 4, 100);

		knapsack->CheckAll();
		knapsack->Pop(2, 2, 2);
		knapsack->Move(2, 2, 3, 3);
		

		knapsack->CheckAll();



		cout << endl;
	}



}