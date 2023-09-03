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
// g++ -Wall -o linkstate_20180492 linkstate_20180492.cc

#define disconnected -999
#define inf 2147483647

typedef struct router{
  vector<int> spt;
  vector<pii> dist; // next, dist
}router;

FILE *topology, *message, *change, *output;

int nodeNum;
vector<vector<int> > graph;
vector<router> routingTable;
vector<pair<pii, string> > contents;
vector<pair<pii, int> > changes;

void initTable(){
  fscanf(topology, "%d", &nodeNum);
  routingTable.resize(nodeNum);
  graph.resize(nodeNum);
  for (int i = 0; i < nodeNum; i++){
    routingTable[i].spt.resize(nodeNum);
    routingTable[i].dist.resize(nodeNum);
    routingTable[i].spt[i] = 1;
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
        graph[i][j] = graph[j][i] = inf;
      }
    }
  }

  while(1){
    int from, to, dist;
    fscanf(topology, "%d", &from);
    if (feof(topology)) break;
    fscanf(topology, "%d %d", &to, &dist);
    graph[from][to] = graph[to][from] = dist;
    routingTable[from].dist[to].first = to;
    routingTable[to].dist[from].first = from;
    routingTable[from].dist[to].second = routingTable[to].dist[from].second = dist;
  }
}

void updateTable(){
  for (int i = 0; i < nodeNum; i++){
    for (int j = 0; j < nodeNum; j++){
      if (i == j) continue;
      int edge = 0;
      while(routingTable[i].spt[edge]){
        edge++;
      }
      
      int st = edge + 1;
      for (int k = st; k < nodeNum; k++){
        if (k == i || routingTable[i].spt[k]) continue;
        int prevcost = routingTable[i].dist[edge].second;
        int newcost = routingTable[i].dist[k].second;
        if (newcost < prevcost) edge = k;
      }

      if (routingTable[i].dist[edge].second == inf) continue;

      routingTable[i].spt[edge] = 1;
      for (int k = 0; k < nodeNum; k++){
        if (graph[edge][k] == inf || routingTable[i].spt[k]) continue;
        int prevdist = routingTable[i].dist[k].second;
        int newdist = routingTable[i].dist[edge].second + graph[edge][k];
        if (newdist < prevdist){
          routingTable[i].dist[k].first = routingTable[i].dist[edge].first;
          routingTable[i].dist[k].second = newdist;
        }
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
      int curr = contents[i].first.first;
      while(curr != contents[i].first.second){
        fprintf(output, "%d ", curr);
        curr = routingTable[curr].dist[contents[i].first.second].first;
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
      fprintf(output, "%d %d %d\n", j, routingTable[i].dist[j].first, routingTable[i].dist[j].second);
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

void clearTable(){
  for (int i = 0; i < nodeNum; i++){
    routingTable[i].spt.clear();
    routingTable[i].dist.clear();
  }
  for (int i = 0; i < nodeNum; i++){
    routingTable[i].spt.resize(nodeNum);
    routingTable[i].spt[i] = 1;
  }
  for (int i = 0; i < nodeNum; i++){
    for (int j = 0; j < nodeNum; j++){
      if (i == j){
        routingTable[i].dist[j].first = j;
        routingTable[i].dist[j].second = 0;
      }
      else{
        routingTable[i].dist[j].first = -1;
        routingTable[i].dist[j].second = inf;
      }
    }
  }
}

void updateChange(){
  for (int i = 0; i < changes.size(); i++){
    clearTable();

    if (changes[i].second == disconnected){
      graph[changes[i].first.first][changes[i].first.second] =
        graph[changes[i].first.second][changes[i].first.first] = 
        inf;
    }
    else{
      graph[changes[i].first.first][changes[i].first.second] =
        graph[changes[i].first.second][changes[i].first.first] = 
        changes[i].second;
    }

    for (int j = 0; j < nodeNum; j++){
      for (int k = 0; k < nodeNum; k++){
        if (j == k || graph[j][k] == inf) continue;
        routingTable[j].dist[k].first = k;
        routingTable[j].dist[k].second = graph[j][k];
      }
    }

    updateTable();
    writeOutput();
  }
}

int main(int argc, char* argv[]){
  if (argc != 4){
    printf("usage: linkstate topologyfile messagesfile changesfile\n");
    return 1;
  }
 
  topology = fopen(argv[1], "r");
  message = fopen(argv[2], "r");
  change = fopen(argv[3], "r");
  output = fopen("output_ls.txt", "w");

  if (!topology || !message || !change){
    printf("Error: open input file.\n");
    return 1;
  }

  initTable();
  updateTable();
  readMessage();
  writeOutput();
  readChange();
  updateChange();

  fclose(topology);
  fclose(message);
  fclose(change);
  fclose(output);

  printf("Complete. Output file written to output_ls.txt.\n");

  return 0;
}