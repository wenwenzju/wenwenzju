#include <iostream>
#include <queue>
#include <vector>

template<class T>
class GraphNode
{
public:
	GraphNode() : node(nullptr), visited(false), backtracking(nullptr) {}
	GraphNode(T* n) : node(n), visited(false), backtracking(nullptr) {}
	T* node;
	std::vector<GraphNode<T>*> children;
	bool visited;
	GraphNode<T>* backtracking;
};

template<class T>
void bfs(GraphNode<T>* startNode, GraphNode<T>* endNode, std::vector<T*>& path)
{
	if (startNode == nullptr || endNode == nullptr)
		return;
	if (startNode == endNode || startNode->node == endNode->node)
	{
		path.push_back(startNode->node);
		return;
	}
	std::queue<GraphNode<T>*> que;
	startNode->visited = true;
	que.push(startNode);
	bool endReached = false;
	while (!que.empty())
	{
		GraphNode<T>* curNode = que.front();
		que.pop();
		for (int i = 0; i < curNode->children.size(); ++i)
		{
			GraphNode<T>* child = curNode->children[i];
			if (child->visited == false)
			{
				child->visited = true;
				child->backtracking = curNode;
				if (child == endNode)
				{
					endReached = true;
					break;
				}
				que.push(child);
			}
		}
		if (endReached)
			break;
	}
	if (endReached)
	{
		GraphNode<T>* curNode = endNode;
		do
		{
			path.push_back(curNode->node);
			curNode = curNode->backtracking;
		}while (curNode != nullptr);
	}
	std::reverse(path.begin(), path.end());
}

/*
* 牛虎过河问题
* 有 M 只牛和 N 只虎，船最多只能容纳两只动物，且船在往返途中不能为空。
* 在任一岸边，若牛的数量少于虎的数量，则牛会被老虎吃掉。
* 是否能使动物全部过河且无损失，若能，制定合理渡河方案
*/

/*
* 以左岸牛虎数量及船是否在左岸为状态，共(M+1)*(N+1)*2种状态
*/
class State
{
public:
	int cattle;
	int tiger;
	int boat;
	State() : cattle(0), tiger(0), boat(0) {}
	State(int m, int n, int b) : cattle(m), tiger(n), boat(b) {}
	bool operator == (const State& right) const
	{
		return (cattle==right.cattle)&&(tiger==right.tiger)&&(boat==right.boat);
	}
};

bool isStateFeasible(const State& state, int M, int N)
{
	if (state.cattle < state.tiger && state.cattle > 0)
		return false;
	if (M-state.cattle < N-state.tiger && M-state.cattle > 0)
		return false;

	// 左岸无动物而船在左岸，不可行
	if (state.cattle==0 && state.tiger==0 && state.boat==0)
		return false;
	// 右岸无动物而船在右岸，不可行
	if (state.cattle==M && state.tiger==N && state.boat==1)
		return false;

	return true;
}

bool isTowStatesTransferable(const State& s1, const State& s2, int k = 2)
{
	// 相同两个状态认为不可达
	if (s1==s2)
		return false;
	// 相邻两个状态，船必定在不同岸
	if (s1.boat == s2.boat)
		return false;

	if (s1.tiger==s2.tiger && s1.cattle==s2.cattle)
		return false;

	// s1->s2, 船从右岸带动物到左岸，左岸动物数量增多
	if (s1.boat == 1)
	{
		int dc = s2.cattle - s1.cattle, dt = s2.tiger - s1.tiger;
		if (dc >= 0 && dt >= 0					// 动物数量增加
			&& (dc+dt <= k)						// 增加的数量不能大于船的容量
			&& ((dc>0 && dc>=dt) || dc==0))		// 船上牛的数量不能小于虎的数量
			return true;
		else
			return false;
	}
	// s1->s2, 船从左岸带动物到右岸，左岸动物数量减少
	else
	{
		int dc = s1.cattle - s2.cattle, dt = s1.tiger - s2.tiger;
		if (dc >= 0 && dt >= 0					// 动物数量减少
			&& (dc+dt <= k)						// 减少的数量不能大于船的容量
			&& ((dc>0 && dc>=dt) || dc==0))		// 船上牛的数量不能小于虎的数量
			return true;
		else
			return false;
	}
}

int main()
{
	int M, N, K;
	while (std::cin >> M >> N >> K)
	{
		std::vector<State> all_states;
		for (int m = 0; m <= M; ++m)
		{
			for (int n = 0; n <= N; ++n)
			{
				State s1(m,n,0);
				if (isStateFeasible(s1, M, N))
					all_states.push_back(s1);
				State s2(m,n,1);
				if (isStateFeasible(s2, M, N))
					all_states.push_back(s2);
			}
		}

		std::vector<GraphNode<State>*> state_nodes(all_states.size(), nullptr);
		for (int i = 0; i < all_states.size(); ++i)
			state_nodes[i] = new GraphNode<State>(&all_states[i]);
		for (int i = 0; i < all_states.size(); ++i)
		{
			for (int j = 0; j < all_states.size(); ++j)
			{
				if (j == i) continue;
				if (isTowStatesTransferable(all_states[i], all_states[j], K))
				{
					state_nodes[i]->children.push_back(state_nodes[j]);
				}
			}
		}

		std::vector<State*> path;
		bfs(state_nodes.back(), state_nodes[0], path);

		for (int i = 0; i < path.size(); ++i)
			std::cout << path[i]->cattle << ", " << path[i]->tiger << ", " << path[i]->boat << std::endl;
		if (path.size() > 0)
			std::cout << "最少过河次数： " << path.size()-1 << std::endl;
	}

	return 0;
}