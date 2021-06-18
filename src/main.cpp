#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>

// create a grid to represent the map
std::vector<std::vector<int>>
    BOARD = {
        {0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0}};

enum class State
{
  kEmpty,
  kObstacle,
  kClosed,
  kPath,
  kStart,
  kFinish
};

// cells with State type instead of int type.
std::string CellString(State cell)
{
  switch (cell)
  {
  case State::kObstacle:
    return "⛰️   ";
  case State::kPath:
    return "🚗   ";
  case State::kStart:
    return "🚦   ";
  case State::kFinish:
    return "🏁   ";
  default:
    return "0     ";
  }
};

State getCellState(int value)
{
  return value == 1 ? State::kObstacle : State::kEmpty;
}

std::vector<State> ParseLine(std::string line)
{
  std::vector<State> ret;
  std::istringstream stream(line);
  int v;
  char c;
  while (stream >> v >> c && c == ',')
  {
    ret.push_back(getCellState(v));
  }
  return ret;
};

std::vector<std::vector<State>> ReadBoardFile(std::string path)
{
  std::vector<std::vector<State>> board;
  std::ifstream file(path);
  if (file.is_open())
  {
    std::string line;
    while (getline(file, line))
    {
      board.push_back(ParseLine(line));
    }
  }
  return board;
};

void PrintBoard(std::vector<std::vector<State>> &grid)
{
  std::cout << "\n";
  for (auto row : grid)
  {
    for (auto cell : row)
    {
      std::cout << CellString(cell);
    }
    std::cout << "\n";
  }
  std::cout << "\n";
};

void PrintVector(std::vector<int> v)
{
  std::cout << "{ ";
  for (auto item : v)
  {
    std::cout << item << " ";
  }
  std::cout << "}"
            << "\n";
};

int Heuristic(int x1, int x2, int y1, int y2)
{
  return std::abs(x2 - x1) + std::abs(y2 - y1);
};

void AddToOpen(std::vector<std::vector<State>> grid, int x, int y, int g, int h, std::vector<std::vector<int>> &open)
{
  std::vector<int> location{x, y, g, h};
  // push location to open but mark as visited after
  open.push_back(location);
  // mark as visited
  grid[x][y] == State::kClosed;
};

bool Compare(std::vector<int> &nodeOne, std::vector<int> &nodeTwo)
{
  return (nodeOne[2] + nodeOne[3] > nodeTwo[3] + nodeTwo[3]) // using the f = g+h(x,y)
             ? true
             : false;
}
bool CheckValidCell(int x, int y, std::vector<std::vector<State>> &grid)
{
  return x >= 0 && y >= 0 && x < grid.size() && y < grid[x].size() && grid[x][y] == State::kEmpty;
}
void CellSort(std::vector<std::vector<int>> &openNodes, bool (*compare)(std::vector<int> &, std::vector<int> &))
{
  sort(openNodes.begin(), openNodes.end(), compare);
}

void ExpandNeighbors(std::vector<int> &currentNode, std::vector<std::vector<int>> &openNodes, int goal[2], std::vector<std::vector<State>> &grid)
{
  const int delta[4][2] = {{-1, 0},
                           {1, 0},
                           {0, -1},
                           {0, 1}};
  int x = currentNode[0], y = currentNode[1], gval = currentNode[2];
  int hval = Heuristic(x, y, goal[0], goal[1]);
  for (auto d : delta)
  {
    int dx = x + d[0];
    int dy = y + d[1];
    if (CheckValidCell(dx, dy, grid))
    {
      int dg = gval + 1;
      int dh = Heuristic(x, y, goal[0], goal[1]);
      std::vector<int> node = {dx, dy, dg, dh};
      AddToOpen(grid, dx, dy, dg, dh, openNodes);
    }
  }
}

std::vector<std::vector<State>> Search(std::vector<std::vector<State>> grid, int init[2], int goal[2])
{
  // code to search for path using heuristic function
  std::vector<std::vector<int>> openNodes = {};
  int x = init[0];
  int y = init[1];                           // present location
  int g = 0;                                 // cost / distance travelled so far
  int h = Heuristic(x, y, goal[0], goal[1]); // hypotenuse / manharttan distance between goal and location
  AddToOpen(grid, x, y, g, h, openNodes);
  while (!openNodes.empty())
  { // while there are still
    CellSort(openNodes, Compare);
    std::vector<int> currentNode = openNodes.back();
    openNodes.pop_back();
    // mark  currentLocation as part of path
    grid[currentNode[0]][currentNode[1]] = State::kPath;
    // check if we are at the goal
    if (currentNode[0] == goal[0] && currentNode[1] == goal[1])
    {
      grid[init[0]][init[1]] = State::kStart;
      grid[goal[0]][goal[1]] = State::kFinish;
      return grid;
    }
    ExpandNeighbors(currentNode, openNodes, goal, grid);
  }
  std::cout << "No path found!"
            << "\n";
  return std::vector<std::vector<State>>{};
};

void PrintSolutionToFile(std::vector<std::vector<State>> grid, std::string filePath)
{
  std::ofstream output(filePath);
  output << "\n";
  for (auto row : grid)
  {
    for (auto cell : row)
    {
      output << CellString(cell);
    }
    output << "\n";
  }
  output << "\n";
};

int main(int argc, char const *argv[])
{
  int init[2] = {0, 0};
  int goal[2] = {4, 5};
  std::string input = "C:\\Users\\PC\\Documents\\projects\\cpp\\2DXRouterPlanner\\1.board";
  auto board = ReadBoardFile(input);
  auto solution = Search(board, init, goal);
  // Leave the following line commented out.
  PrintBoard(solution);
  std::string output = "C:\\Users\\PC\\Documents\\projects\\cpp\\2DXRouterPlanner\\1-output.board";
  PrintSolutionToFile(solution, output);
  return 0;
};
