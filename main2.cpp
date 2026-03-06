#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <queue>
#include <iostream>
#include <climits> //INT_MAX y INT_MIN
#include <utility>
#include <random>
#include <vector>
#include <cmath>

using namespace std;

constexpr int TILE=14;

struct World{
    std::vector<int> start;
    std::vector<int> end;

    std::vector<int> x={-1,1,-1,0,1,-1,0,1};
    std::vector<int> y={0,0,-1,-1,-1,1,1,1};

    std::vector<int> center_x={-1,1,0,0};
    std::vector<int> center_y={0,0,-1,1};

    std::vector<std::vector<int>> world{
        50, std::vector<int>(50,0)
    };

    void init(){

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 3);

        for(int i=0; i<50; i++){
            world[0][i]=1;
            world[49][i]=1;
            world[i][0]=1;
            world[i][49]=1;
        }

        for(int i=0; i<50; i++){
            for(int j=0; j<50; j++){
                int neighbor=0;

                for(int k=0; k<8; k++){
                    int new_i=i+x[k];
                    int new_j=j+y[k];

                    if(new_i<0 || new_j<0 || new_i>49 || new_j>49) continue;
                    if(world[new_i][new_j]==1) neighbor++;
                }

                building(neighbor, i, j, gen, dis);  
            }
        }
    }

    void building(int& neighbor, int& i, int& j, std::mt19937& gen, std::uniform_int_distribution<>& dis){
        if(neighbor>=4) return;

        //SOLO SUMA CUADROS, NO HAY DESCARTE AUN
        if(world[i][j]==0 && neighbor<2){
            int c=neighbor;
            int x, y;
            while(c!=2){
                int k=dis(gen);
                int new_i=center_x[k]+i;
                int new_j=center_y[k]+j;
                world[new_i][new_j]=1;
                c++;
            }
        }

        if(world[i][j]==0 && neighbor>=3){
            int c=0;
            int x, y;
            while(c!=2){
                int k=dis(gen);
                int new_i=center_x[k]+i;
                int new_j=center_y[k]+j;

                if(new_i==0 || new_i==49) continue;
                if(new_j==0 || new_j==49) continue;

                world[new_i][new_j]=0;
                c++;
            }
        }
    }

    void draw(sf::RenderWindow& window){
        sf::RectangleShape wall(sf::Vector2f(TILE, TILE));
        sf::RectangleShape Start(sf::Vector2f(TILE-1, TILE-1));
        sf::RectangleShape End(sf::Vector2f(TILE-1, TILE-1));

        sf::RectangleShape Path(sf::Vector2f(TILE, TILE));

        for(int i=0; i<50; i++){
            for(int j=0; j<50; j++){
                if(world[i][j]==1){
                    wall.setFillColor(sf::Color::Cyan);
                    wall.setPosition({float(j*TILE), float(i*TILE)});
                    window.draw(wall);
                }else if(world[i][j]==2){
                    Start.setFillColor(sf::Color::Green);
                    Start.setPosition({float(j*TILE), float(i*TILE)});
                    window.draw(Start);
                }else if(world[i][j]==3){
                    End.setFillColor(sf::Color::Red);
                    End.setPosition({float(j*TILE), float(i*TILE)});
                    window.draw(End);
                }else if(world[i][j]==4){
                    Path.setFillColor(sf::Color::Yellow);
                    Path.setPosition({float(j*TILE), float(i*TILE)});
                    window.draw(Path);
                }else if(world[i][j]==5){
                    wall.setFillColor(sf::Color::Yellow);
                    wall.setPosition({float(j*TILE), float(i*TILE)});
                    window.draw(wall);
                }
            }
        }
    }
};

struct A_path{

    struct Node{
        int x,y;
        int g;
        int f;

        bool operator<(const Node& other) const{
            return f > other.f; // para priority_queue (min heap)
        }
    };

    std::vector<int> start;
    std::vector<int> target;

    std::vector<int> dx={-1,1,0,0,-1,-1,1,1};
    std::vector<int> dy={0,0,-1,1,-1,1,-1,1};

    std::priority_queue<Node> open;

    bool closed[50][50]={false};
    int gscore[50][50];
    std::pair<int,int> parent[50][50];

    bool finished=false;

    void init(World& w){

        start=w.start;
        target=w.end;

        while(!open.empty()) open.pop();

        for(int i=0;i<50;i++){
            for(int j=0;j<50;j++){
                closed[i][j]=false;
                gscore[i][j]=INT_MAX;
            }
        }

        Node s;
        s.x=start[0];
        s.y=start[1];
        s.g=0;
        s.f=heuristic(s.x,s.y);

        open.push(s);
        gscore[s.x][s.y]=0;
    }

    int heuristic(int x,int y){
        return abs(target[0]-x)+abs(target[1]-y);
    }

    void reconstruct(World& w){

        int x=target[0];
        int y=target[1];

        while(!(x==start[0] && y==start[1])){

            auto p=parent[x][y];
            x=p.first;
            y=p.second;

            if(w.world[x][y]==0)
                w.world[x][y]=4;
        }
    }

    void update(World& w){

        if(start.size()<2 || target.size()<2){
            start=w.start;
            target=w.end;

            if(start.size()==2 && target.size()==2) init(w);
            return;
        }

        if(open.empty()) return;
        if(finished) return;

        Node current=open.top();
        open.pop();

        int cx=current.x;
        int cy=current.y;

        if(closed[cx][cy]) return;
        closed[cx][cy]=true;

        if(cx==target[0] && cy==target[1]){
            reconstruct(w);
            finished=true;
            return;
        }

        for(int k=0;k<8;k++){

            int nx=cx+dx[k];
            int ny=cy+dy[k];

            if(nx<=0 || ny<=0 || nx>=49 || ny>=49) continue;
            if(w.world[nx][ny]==1) continue;
            if(closed[nx][ny]) continue;

            int cost;

            if(abs(dx[k])+abs(dy[k])==2)
                cost=14;
            else
                cost=10;

            int newg=current.g+cost;

            if(newg < gscore[nx][ny]){

                gscore[nx][ny]=newg;

                Node next;
                next.x=nx;
                next.y=ny;
                next.g=newg;
                next.f=newg + heuristic(nx,ny);

                parent[nx][ny]={cx,cy};

                open.push(next);
            }
        }
    }
};

void execute(){
    World _w;
    A_path _a;

    _w.init();
    //_a.init(_w);

    sf::RenderWindow window{
        sf::VideoMode({
                static_cast<unsigned>(50*TILE),
                static_cast<unsigned>(50*TILE)
        }), "LABYRINTH!"
    };

    window.setFramerateLimit(60);

    enum Target{start, end};
    Target tar;
    tar=start;

    while(window.isOpen()){
        while(const std::optional event=window.pollEvent()){
            if(event->is<sf::Event::Closed>()) window.close(); 

            if(const auto* mousePressed=event->getIf<sf::Event::MouseButtonPressed>()){
                if(mousePressed->button==sf::Mouse::Button::Left){
                    int col=mousePressed->position.x/TILE;
                    int row=mousePressed->position.y/TILE;

                    if(row>=0 && row<=49 && col>=0 && col<=49){
                        if(_w.world[row][col]==0){
                            if(tar==start){
                                _w.start.push_back(row);
                                _w.start.push_back(col);
                                _w.world[row][col]=2;
                                tar=end;
                            }else{
                                _w.end.push_back(row);
                                _w.end.push_back(col);
                                _w.world[row][col]=3;
                                tar=start;
                            }
                        }
                    }
                }
            }
        }
        
        window.clear();
        _a.update(_w);
        _w.draw(window);
        window.display();
    }
}

int main(){
    execute();
    return 0;
}

//g++ -o m1 main1.cpp -lsfml-graphics -lsfml-window -lsfml-system
