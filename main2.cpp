#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>
#include <random>
#include <vector>

using namespace std;

constexpr int TILE=14;

struct World{
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
                }
            }
        }
    }
};

struct A_path{
    //f(n)=g(n)+h(n)
    //start -g(n)-> current -h(n)-> goal
    //coste real g(n(k))=sumatoria de i=0 hasta k-1 en la forma w(n(i), n(i+1))
    //w(n(i), n(i+1)) representa el peso de la arista que conecta el nodo n(i) al nodo n(i+1)
    //funcion heuristica h(n)=
    //distancia manhattan h(n)=|x1-x2|+|y1-y2|
    //distancia euclidiana h(n)=raiz cuadrada((x1-x2)^2+(y1-y2)^2)
    /*
     coste total estimado f(n)
     g(n) coste real desde el inicio hasta el nodo actual
     h(n) coste estimado desde el nodo actual hasta el objetivo
     * */
};

void execute(){
    World _w;
    _w.init();

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
                                _w.world[row][col]=2;
                                tar=end;
                            }else{
                                _w.world[row][col]=3;
                                tar=start;
                            }
                        }
                    }
                }
            }
        }
        
        window.clear();
        _w.draw(window);
        window.display();
    }
}

int main(){
    execute();
    return 0;
}

//g++ -o m1 main1.cpp -lsfml-graphics -lsfml-window -lsfml-system
