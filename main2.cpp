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

constexpr int TILE=14;

struct World{
    std::vector<int> start;
    std::vector<int> end;

    std::vector<int> x={-1,1,-1,0,1,-1,0,1};
    std::vector<int> y={0,0,-1,-1,-1,1,1,1};

    std::vector<int> center_x={-1,1,0,0};
    std::vector<int> center_y={0,0,-1,1};

    std::vector<std::vector<int>> world={
        50, std::vector<int>(50,0)
    };

    void init(){

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 3);

        for(int i=0;i<50;i++)
            for(int j=0;j<50;j++)
                world[i][j]=0;

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
                    wall.setFillColor(sf::Color::Magenta);
                    wall.setPosition({float(j*TILE), float(i*TILE)});
                    window.draw(wall);
                }
            }
        }
    }
};

struct A_path{

    struct Node{ //cada nodo tiene sus coordenadas, peso y heuristica
                 //siempre se usan struct COMO PUDISTE OLVIDARLO!!!
        int x,y;
        int g;
        int f;

        bool operator<(const Node& other) const{
            return f > other.f; // para priority_queue (min heap)
        }
    };

    std::vector<int> start;
    std::vector<int> target;

    std::vector<int> dx={-1,1,0,0};
    std::vector<int> dy={0,0,-1,1};

    std::priority_queue<Node> open; //el de mayor valor siempre esta en la cima

    bool closed[50][50]={false}; //matriz de nodos ya visitados
    int gscore[50][50]; //matriz de menor peso
    std::pair<int,int> parent[50][50]; //matriz que en cada nodo guarda las
                                       //coordenadas de su nodo anterior

    bool finished=false;

    void init(World& w){
        //AQUI AUN NO HACE USO DEL VECTOR DE VECTORES PARENT!!
        start=w.start; //ACTUALIZA COORDENADAS AL DAR CLICK EN EL WHILE
        target=w.end;

        //w.world[start[0]][start[1]]=4;

        while(!open.empty()) open.pop(); //remueve el primer elemento

        for(int i=0;i<50;i++){ //inicializador
            for(int j=0;j<50;j++){
                closed[i][j]=false;
                gscore[i][j]=INT_MAX;
            }
        }

        Node s;
        s.x=start[0]; //actualiza coordenadas
        s.y=start[1]; //de forma puntual
        s.g=0;
        s.f=heuristic(s.x,s.y); //distancia manhattan

        open.push(s); //lo agrega al priority_queue
        gscore[s.x][s.y]=0;
    }

    int heuristic(int x,int y){
        return abs(target[0]-x)+abs(target[1]-y);
    }

    void reconstruct(World& w){
        //FUNCION PARA VERIFICAR SI YA LLEGÒ AL OBJETIVO
        int x=target[0];
        int y=target[1];

        while(!(x==start[0] && y==start[1])){ //si coordenadas no es igual a objetivo
            //DIBUJA 4 EN EL MAPA MAPEANDO EL CAMINO
            auto p=parent[x][y];
            x=p.first;
            y=p.second;

            if(w.world[x][y]!=2 && w.world[x][y]!=3) w.world[x][y]=4;
        }
    }

    void update(World& w, bool& game){

        if(start.size()<2 || target.size()<2){
            //no inicia afuera del while, sino adentro
            //YA QUE LOS CLICKS SON POSTERIORES AL INICIAL EL PROGRAMA
            start=w.start;
            target=w.end;

            //w.world[start[0]][start[1]]=4;

            if(start.size()==2 && target.size()==2) init(w);
            return;
        }

        if(open.empty()) return; //RECORDAR QUE open es el priority_queue
        if(finished) return;

        Node current=open.top(); //extrae el top que seria el de mas valor
        open.pop(); //procede a eliminarlo

        int cx=current.x; //coordenadas
        int cy=current.y;

        if(closed[cx][cy]) return;
        closed[cx][cy]=true; //vector de ua visitados

        if(cx==target[0] && cy==target[1]){ //si ya llegò al objetivo
            w.world[cx][cy]=3;
            reconstruct(w);
            finished=true;
            game=false;
            return;
        }else game=true;

        for(int k=0;k<4;k++){ //REVISA SUS 8 VECINOS DE ALREDEDOR

            int nx=cx+dx[k];
            int ny=cy+dy[k];

            if(nx<=0 || ny<=0 || nx>=49 || ny>=49) continue;
            if(w.world[nx][ny]==1) continue; //IGNORA LOS MUROS
            if(closed[nx][ny]) continue; //IGNORA LOS VISITADOS
                                         
            //DIBUJAR LOS QUE SE VAN DIBUJANDO
            w.world[nx][ny]=5;

            int cost;

            //es mucho mas inteligente, si ambas coordenadas suman dos
            //en valor absoluto entonces se trata de 1 o -1 en las dos coordenadas
            //si no, entonces hay una coordenada que es cero. por lo tanto es
            //diagonal
            if(std::abs(dx[k])+std::abs(dy[k])==2) cost=14; 
            else cost=10;

            int newg=current.g+cost; //current es un objeto Node
                                     //quien contiene g

            if(newg < gscore[nx][ny]){

                gscore[nx][ny]=newg; //actualiza siempre de menor peso
                //w.world[nx][ny]=4;

                Node next;
                next.x=nx;
                next.y=ny;
                next.g=newg;
                next.f=newg + heuristic(nx,ny); //peso + distancia manhattan

                parent[nx][ny]={cx,cy}; //el nuevo nodo guarda las coordenadas de su nodo anterior

                open.push(next); //lo agrega al priority_queue
            }
        }

        //w.world[cx][cy]=4;
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

    sf::Clock clock;
    float timer=0;
    float delay=0.07;
    bool game=true;

    while(window.isOpen()){
        while(const std::optional event=window.pollEvent()){
            if(event->is<sf::Event::Closed>()) window.close();

            if(const auto* key=event->getIf<sf::Event::KeyPressed>()){
                if(key->code==sf::Keyboard::Key::Enter && !game){
                    _w.start.clear();
                    _w.end.clear();

                    _w.init();
                    _a=A_path();
                }
            }

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

        /*if(game){
            if(timer>delay){
                timer-=delay;
            }
        }else{
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)){
                _w.start.clear();
                _w.end.clear();

                _w.init();
                _a=A_path();
            }
        } */
        
        window.clear();
        _a.update(_w, game);
        _w.draw(window);
        window.display();
    }
}

int main(){
    execute();
    return 0;
}

//g++ -o m1 main1.cpp -lsfml-graphics -lsfml-window -lsfml-system
