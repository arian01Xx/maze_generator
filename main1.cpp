#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>
#include <random>
#include <vector>

using namespace std;

constexpr int TILE=14;

struct World{ 

    int amount=0;

    std::vector<int> x2={-2,2,0,0};
    std::vector<int> y2={0,0,-2,2};

    std::vector<int> x1={-1,1,0,0,1,1,-1,-1}; //LE FALTAN LAS ESQUINAS!!!
    std::vector<int> y1={0,0,-1,1,1,-1,-1,1};

    std::vector<std::vector<int>> world{
        50, std::vector<int>(50,0)
    };

    std::vector<std::vector<bool>> visited{
        50, std::vector<bool>(50,false)
    };

    void init(){

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 49);

        for(int i=0; i<50; i++){
            world[0][i]=1;
            world[49][i]=1;
            world[i][0]=1;
            world[i][49]=1;

            visited[0][i]=true;
            visited[49][i]=true;
            visited[i][0]=true;
            visited[i][49]=true;
        }

        //ELIGE DE FORMA ALEATORIA TODAS LAS POSICIONES DE LA MATRIZ!!!
        amount=inspect();
        while(amount!=0){
            amount=inspect();

            int i=dis(gen);
            int j=dis(gen);
            visited[i][j]=true; //YA VISITADO sin importar si es cero o 1
                                
            if(world[i][j]==1) continue; //LOS MUROS PUESTOS son innamovibles
            if(world[i][j]==0){

                if(empty_space(i,j)){
                    world[i][j]=1;
                    int k=dis(gen)%4;

                    int new_i=i+x2[k]; //AHORA SI REVISA LOS EXTREMOS!
                    int new_j=j+y2[k];

                    if(new_i<=0 || new_j<=0 || new_i>=49 || new_j>=49){
                        if(world[new_i][new_j]==0){
                            int mid_i = i + x2[k]/2;
                            int mid_j = j + y2[k]/2;

                            world[mid_i][mid_j] = 1;
                            world[new_i][new_j] = 1;
                        }
                    } 
                }else continue;
            }
        }
    }

    bool empty_space(int i, int j){
        for(int k=0; k<4; k++){

            int next_i=i+x1[k]; //RECORDAR X1 Y Y1 son los de a lado
            int next_j=j+y1[k];

            if(world[next_i][next_j]==1) return false;
        }

        return true;
    }

    int inspect(){
        int c=0;
        for(int i=0; i<50; i++){
            for(int j=0; j<50; j++){
                if(visited[i][j]==false) c++;
            }
        }

        return c;
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
/*INICIALIZACIONES
 
std::vector<std::vector<int>> world = std::vector<std::vector<int>>(50, std::vector<int>(50,0));

std::vector<std::vector<int>> world = std::vector(50, std::vector<int>(50,0));*/
