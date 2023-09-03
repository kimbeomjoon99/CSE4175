#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <string>
#include <cmath>
#include <queue>
#include <stack>
#include <deque>
using namespace std;
typedef pair<int, int> pii;
// g++ -Wall -o distvec_20180492 distvec_20180492.cc

#define inf 2147483647
#define disconnected -999

typedef struct router{
  vector<int> neighbor; // neighbor
  vector<pii> dist; // next, dist
  vector<vector<int> > path;
}router;

FILE *topology, *message, *change, *output;

int nodeNum;
vector<router> routingTable;
vector<vector<int> > graph;
vector<pair<pii, string> > contents;
vector<vector<vector<int> > > path;
vector<pair<pii, int> > changes;

void initTable(){
  fscanf(topology, "%d", &nodeNum);

  routingTable.resize(nodeNum);
  graph.resize(nodeNum);
  for (int i = 0; i < nodeNum; i++){
    routingTable[i].neighbor.resize(nodeNum);
    routingTable[i].dist.resize(nodeNum);
    routingTable[i].path.resize(nodeNum);
    graph[i].resize(nodeNum);
  }

  for (int i = 0; i < nodeNum; i++){
    for (int j = 0; j < nodeNum; j++){
      if (i == j){
        routingTable[i].dist[j].first = i;
        routingTable[i].dist[j].second = 0;
        graph[i][j] = 0;
      }
      else{
        routingTable[i].dist[j].first = -1;
        routingTable[i].dist[j].second = inf;
        graph[i][j] = inf;
      }
    }
  }

  while (1){
    int from, to, dist;
    fscanf(topology, "%d", &from);
    if (feof(topology)) break;
    fscanf(topology, "%d %d", &to, &dist);
    routingTable[from].neighbor[to] = routingTable[to].neighbor[from] = 1;
    routingTable[from].dist[to].first = to;
    routingTable[to].dist[from].first = from;
    routingTable[from].dist[to].second = routingTable[to].dist[from].second = dist;
    graph[from][to] = graph[to][from] = dist;
  }
}

void updateTable(int start, int init = 0){
  vector<int> checker(nodeNum);
  int flag = 0;
  queue<int> q;
  q.push(start);
  int prev = -2;
  if (init){
    checker[0] = 1;
  }
  while(!q.empty()){
    int curr = q.front();
    q.pop();
    if (curr == prev) continue;
    else prev = curr;

    for (int i = 0; i < nodeNum; i++){
      if (!routingTable[curr].neighbor[i]) continue;
      if (curr == i) continue;
      for (int j = 0; j < nodeNum; j++){
        if (routingTable[curr].dist[j].second == inf) continue;
        if (curr == j) continue;
        int prevdist = routingTable[i].dist[j].second;
        int newdist = routingTable[curr].dist[j].second + 
          routingTable[curr].dist[i].second;
        if (prevdist > newdist){
          routingTable[i].dist[j].first = curr;
          routingTable[i].dist[j].second = newdist;
          q.push(i);
          if (init && !checker[i]) checker[i] = 1;
        }
        
        else if ((prevdist == newdist) && (routingTable[i].dist[curr].first < routingTable[i].dist[j].first)){
          routingTable[i].dist[j].first = routingTable[i].dist[curr].first;
          q.push(i);
          if (init && !checker[i]) checker[i] = 1;
        }
        
      }
    }
    if (init && !flag && q.empty()){
      int f = 0;
      for (int i = 0; i < nodeNum; i++){
        if (!checker[i]){
          f++;
          q.push(i);
          break;
        }
      }
      if (!f) flag = 1;
    }
  }
}

void makePathTable(){
  path.resize(nodeNum);
  for (int i = 0; i < nodeNum; i++){
    path[i].resize(nodeNum);
  }
}

void calculatePath(){
  for (int i = 0; i < path.size(); i++){
    for (int j = 0; j < path.size(); j++){
      path[i][j].clear();
    }
  }

  for (int i = 0; i < nodeNum; i++){
    for (int j = 0; j < nodeNum; j++){
      if (routingTable[i].dist[j].second == inf) continue;
      int curr = i;
      path[i][j].push_back(curr);
      while(curr != j){
        curr = routingTable[curr].dist[j].first;
        if (curr == j) break;
        path[i][j].push_back(curr);
      }
    }
  }
}

void readMessage(){
  while(1){
    int from, to;
    char tmpstr[10000];
    fscanf(message, "%d", &from);
    if (feof(message)) break;
    fscanf(message, "%d", &to);
    if (feof(message)) break;
    fgets(tmpstr, 10000, message);
    if (feof(message)) break;
    if (tmpstr[strlen(tmpstr) - 1] == '\n')
      tmpstr[strlen(tmpstr) - 1] = '\0';
    string str(tmpstr);
    contents.push_back(make_pair(make_pair(from, to), str));
  }
}

void writeMessage(){
  for (int i = 0; i < contents.size(); i++){
    if (routingTable[contents[i].first.first].dist[contents[i].first.second].second == inf){
      fprintf(output, "from %d to %d cost infinite hops unreachable message%s\n", contents[i].first.first,
        contents[i].first.second, contents[i].second.c_str());
    }
    else{
      fprintf(output, "from %d to %d cost %d hops ", contents[i].first.first, contents[i].first.second,
        routingTable[contents[i].first.first].dist[contents[i].first.second].second);
      for (int j = 0; j < path[contents[i].first.first][contents[i].first.second].size(); j++){
        fprintf(output, "%d ", path[contents[i].first.first][contents[i].first.second][j]);
      }
      fprintf(output, "message%s\n", contents[i].second.c_str());
    }
  }
  fprintf(output, "\n");
}

void writeOutput(){
  for (int i = 0; i < nodeNum; i++){
    for (int j = 0; j < nodeNum; j++){
      if (routingTable[i].dist[j].second == inf){
        continue;
      }
      else{
        fprintf(output, "%d %d %d\n", j, routingTable[i].dist[j].first,
          routingTable[i].dist[j].second);
      }
    }
    fprintf(output, "\n");
  }
  writeMessage();
}

void readChange(){
  while(1){
    int from, to, dist;
    fscanf(change, "%d", &from);
    if (feof(change)) break;
    fscanf(change, "%d", &to);
    if (feof(change)) break;
    fscanf(change, "%d", &dist);
    if (feof(change)) break;
    changes.push_back(make_pair(make_pair(from, to), dist));
  }
}

void clearTable(char *tName){
  fclose(topology);
  topology = fopen(tName, "r");
  routingTable.clear();
  routingTable.resize(nodeNum);
  initTable();
}

void updateChange(char *tName){
  for (int i = 0; i < changes.size(); i++){
    clearTable(tName);
    int from, to, dist;
    for (int j = 0; j < i; j++){
      from = changes[i].first.first;
      to = changes[i].first.second;
      dist = changes[i].second;
      if (dist == disconnected){
        routingTable[from].neighbor[to] = routingTable[to].neighbor[from] = 0;
        routingTable[from].dist[to].first = routingTable[to].dist[from].first = -1;
        routingTable[from].dist[to].second = routingTable[to].dist[from].second = inf;
      }
      else{
        routingTable[from].neighbor[to] = routingTable[to].neighbor[from] = 1;
        routingTable[from].dist[to].first = to;
        routingTable[to].dist[from].first = from;
        routingTable[from].dist[to].second = routingTable[to].dist[from].second = dist;
      }
    }
    from = changes[i].first.first;
    to = changes[i].first.second;
    dist = changes[i].second;
    if (dist == disconnected){
      routingTable[from].neighbor[to] = routingTable[to].neighbor[from] = 0;
      routingTable[from].dist[to].first = routingTable[to].dist[from].first = -1;
      routingTable[from].dist[to].second = routingTable[to].dist[from].second = inf;
    }
    else{
      routingTable[from].neighbor[to] = routingTable[to].neighbor[from] = 1;
      routingTable[from].dist[to].first = to;
      routingTable[to].dist[from].first = from;
      routingTable[from].dist[to].second = routingTable[to].dist[from].second = dist;
    }
    updateTable(from, 1);
    calculatePath();
    writeOutput();
  }
}

int main(int argc, char* argv[]){
  if (argc != 4){
    printf("usage: distvec topologyfile messagesfile changesfile\n");
    return 1;
  }
 
  topology = fopen(argv[1], "r");
  message = fopen(argv[2], "r");
  change = fopen(argv[3], "r");
  output = fopen("output_dv.txt", "w");

  if (!topology || !message || !change){
    printf("Error: open input file.\n");
    return 1;
  }

  initTable();
  updateTable(0, 1);
  makePathTable();
  calculatePath();
  readMessage();
  writeOutput();
  readChange();
  updateChange(argv[1]);

  fclose(topology);
  fclose(message);
  fclose(change);
  fclose(output);

  printf("Complete. Output file written to output_dv.txt.\n");

  return 0;
}