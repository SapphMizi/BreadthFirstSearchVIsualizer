# include <Siv3D.hpp>

using NodeID = int32;
using ActiveNode = int32;

struct Node
{
	NodeID id;
	ActiveNode active;
	Vec2 pos;

	Circle getCircle() const
	{
		return Circle{ pos, 40 };
	}

	void drawNode() const
	{
		getCircle()
			.drawShadow(Vec2{ 1, 1 }, 8, 1)
			.draw();
	}

	void drawNodeActive() const
	{
		getCircle()
			.drawShadow(Vec2{ 1, 1 }, 8, 1)
			.draw(ColorF{ 1.0, 0.9, 0.8 });
	}

	void drawNodeDiscovered() const
	{
		getCircle()
			.drawShadow(Vec2{ 1, 1 }, 8, 1)
			.draw(Palette::Lemonchiffon);
	}

	void drawNodeVisited() const
	{
		getCircle()
			.drawShadow(Vec2{ 1, 1 }, 8, 1)
			.draw(Palette::Lightgreen);
	}

	void drawLabel(const Font& font) const
	{
		font(id).drawAt(pos, ColorF{ 0.25 });
	}
};

struct Edge
{
	NodeID from;
	NodeID to;
};

// 2 つのノード間に線を引く関数
void DrawEdge(const Node& from, const Node& to)
{
	Line{ from.pos, to.pos }.draw(3, ColorF{ 0.25 });
}



void Main()
{
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	const Font font{ 40, Typeface::Bold };

	//----------- BFS に使用するデータ構造・変数-------------

	// queue の先頭
	NodeID v;
	// BFSの処理どっちやるのか
	bool Flag = true;
	// TextBox で使うやつ
	Optional<bool> result;
	TextEditState Start;
	/// @brief 探索の始点
	NodeID s = 0;
	/// @brief 各頂点からの最短距離
	Array<NodeID> dist(5, -1);
	/// @brief 次に探索する頂点を管理するキュー（という名の配列）
	Array<NodeID> todo;
	/// @brief 隣接リスト
	Array<Array<NodeID>> G =
	{
		{ 1, 2 },
		{ 0, 2, 4 },
		{ 0, 1, 3, 4 },
		{ 2, 3 },
		{ 1, 2, 3 }, 
	};

	HashTable<NodeID, Node> nodes =
	{
		/*-------------------------------------------
		  active (2番目の数字) はノードがどの常態かを表す
		  0：普通の状態
		  1：探索中のノード
		  2：探索中のノードから行くことができるノード
		  3：探索済みのノード
		--------------------------------------------*/
		
		{ 0, Node{ 0, 0, Vec2{ 100, 300 } }},
		{ 1, Node{ 1, 0, Vec2{ 300, 100 } }},
		{ 2, Node{ 2, 0, Vec2{ 400, 500 } }},
		{ 3, Node{ 3, 0, Vec2{ 600, 470 } }},
		{ 4, Node{ 4, 0, Vec2{ 600, 200 } }},
	};

	Array<Edge> edges =
	{
		Edge{ 0, 1 },
		Edge{ 0, 2 },
		Edge{ 1, 2 },
		Edge{ 1, 4 },
		Edge{ 2, 3 },
		Edge{ 2, 4 },
		Edge{ 3, 4 },
	};

	while (System::Update())
	{
		ClearPrint();
		Print << U"始点：" << s;
		Print << U"探索する点：" << todo;
		Print << U"始点からの最短距離：" << dist;
		Print << U"Flag：" << Flag;
		Print << U"v：" << v;

		// range-based for で各エッジにアクセス
		for (const auto& edge : edges)
		{
			// 始点のノードと終点のノードを渡す
			DrawEdge(nodes[edge.from], nodes[edge.to]);
		}

		for (const auto [nodeID, node] : nodes)
		{
			if (node.active == 0)
			{
				node.drawNode();
			}
			else if (node.active == 1)
			{
				node.drawNodeActive();
			}
			else if (node.active == 2)
			{
				node.drawNodeDiscovered();
			}
			else if (node.active == 3)
			{
				node.drawNodeVisited();
			}
			node.drawLabel(font);
		}

		/// @brief 始点の設定
		if (SimpleGUI::TextBox(Start, Vec2{ 300, 50 }))
		{
			result.reset();
		}
		if (SimpleGUI::Button(U"確定", Vec2{ 500, 50 }, unspecified, (not Start.text.isEmpty())))
		{
			int32 temp = Parse<int32>(Start.text);
			result = (0 <= temp && temp <= 4);
			if (*result == true)
			{
				s = temp;
				todo.push_back(s);
			}
			else
			{
				s = 0;
				todo.push_back(s);
			}
		}

		// BreadthFirstSearch
		// 奇数回目は queue の先頭を取り出す処理
		// 偶数回目は探索中のノードから行くことができる頂点を探索する
		if (SimpleGUI::Button(U"一手進める", Vec2{200, 25}))
		{
			result.reset();
			if (!todo.isEmpty())
			{
				if (Flag == true)
				{
					for (auto [nodeID, node] : nodes)
					{
						/// @brief 探索中だったノードは探索済みにする
						if (node.active == 1)
						{
							node.active = 3;
						}
						/// @brief 探索で見つかったノードは元に戻す
						if (node.active == 2)
						{
							node.active = 0;
						}
					}
					v = todo.front();
					todo.pop_front();
					/// @brief 次に探索するノードを探索中にする
					nodes[v].active = 1;
					Flag = false;
				}
				// ここからがうまくいかない（処理されてない）
				else
				{
					for (NodeID x : G[v])
					{
						if (dist[x] > -1)
						{
							continue;
						}
						/// @brief 見つかったノードを色付けする
						nodes[x].active = 2;
						/// @brief 最短距離の更新
						dist[x] = dist[v] + 1;
						todo.push_back(x);
					}
					Flag = true;
				}
			}
		}
	}
}
