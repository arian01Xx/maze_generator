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

                if(neighbor>=4) continue;

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
        }
    }

    void draw(sf::RenderWindow& window){
        sf::RectangleShape wall(sf::Vector2f(TILE, TILE));
        wall.setFillColor(sf::Color::Cyan);

        for(int i=0; i<50; i++){
            for(int j=0; j<50; j++){
                if(world[i][j]==1){
                    wall.setPosition({float(j*TILE), float(i*TILE)});
                    window.draw(wall);
                }
            }
        }
    }
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

    while(window.isOpen()){
        while(const std::optional event=window.pollEvent()){
            if(event->is<sf::Event::Closed>()) window.close();
            
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
