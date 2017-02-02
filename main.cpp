#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;

const float CASA = 50;
const int LARGURA = 800;
const int ALTURA = 600;
const int N_LINHAS = ALTURA / CASA;
const int N_COLUNAS = LARGURA / CASA;

struct Par{
    int x;
    int y;
    sf::Color cor;

    Par(int x, int y){
        this->x = x;
        this->y = y;
        this->cor = sf::Color::Black;
    }

    bool operator==(const Par &par){
        return this->x == par.x && this->y == par.y;
    }
};

struct Cobra{
    vector<Par> gomos;
    sf::Keyboard::Key direcao = sf::Keyboard::Right;
    sf::Keyboard::Key old_direcao = sf::Keyboard::Left;
};

sf::Keyboard::Key inverter(sf::Keyboard::Key tecla){
    if (tecla == sf::Keyboard::Up)
        return sf::Keyboard::Down;
    else if (tecla == sf::Keyboard::Down)
        return sf::Keyboard::Up;
    else if (tecla == sf::Keyboard::Left)
        return sf::Keyboard::Right;
    return sf::Keyboard::Left;
}

bool colisao_com_corpo(Cobra cobra){
    Par cabeca = cobra.gomos[0];
    int tam = cobra.gomos.size();
    for (int i = 1; i < tam; i++)
        if (cabeca == cobra.gomos[i])
            return true;
    return false;
}

void limitar_cobra(Cobra &cobra){
    //Erquuerda e direita
    if (cobra.gomos[0].x < 0)
        cobra.gomos[0].x = N_COLUNAS - 1;
    if (cobra.gomos[0].x > N_COLUNAS)
        cobra.gomos[0].x = 0;
    //Cima e Embaixo
    if (cobra.gomos[0].y < 0)
        cobra.gomos[0].y = N_LINHAS - 1;
    if (cobra.gomos[0].y > N_LINHAS)
        cobra.gomos[0].y = 0;
}

void update_cobra(Cobra &cobra){
    if (cobra.direcao == cobra.old_direcao && cobra.gomos.size() > 1)
        cobra.direcao = inverter(cobra.old_direcao);

    Par aux = cobra.gomos[0];
    //Atualiza a cabeça
    if (cobra.direcao == sf::Keyboard::Up){
        cobra.gomos[0].y -= 1;
    } else if (cobra.direcao == sf::Keyboard::Down){
        cobra.gomos[0].y += 1;
    } else if(cobra.direcao == sf::Keyboard::Left){
        cobra.gomos[0].x -= 1;
    } else if(cobra.direcao == sf::Keyboard::Right){
        cobra.gomos[0].x += 1;
    }

    cobra.old_direcao = inverter(cobra.direcao);

    //Atualiza os gomos restantes
    int tam = cobra.gomos.size();
    for(int i = 1; i < tam; i++){
        std::swap(aux.x, cobra.gomos[i].x);
        std::swap(aux.y, cobra.gomos[i].y);
    }

}

int main(){
    srand(time(NULL));
    sf::Clock relogio;
    sf::Clock relogio_marca;

    sf::RenderWindow janela(sf::VideoMode(LARGURA + 200, ALTURA), "Janela");

    Par marca = Par(rand() % N_COLUNAS, rand() % N_LINHAS);

    sf::RectangleShape retangulo;
    retangulo.setSize(sf::Vector2f(CASA - 4, CASA - 4));

    Cobra cobra;
    cobra.gomos.push_back(Par(2, 2));

    while(janela.isOpen()){
        sf::Event evento;

        //Gerencia eventos
        while(janela.pollEvent(evento)){
            if(evento.type == sf::Event::Closed)
                janela.close();
            if(evento.type == sf::Event::KeyPressed){
                if (evento.key.code >= sf::Keyboard::Left && evento.key.code <= sf::Keyboard::Down)
                    cobra.direcao = evento.key.code;
            }
        }

        //Cobra aumenta ao pegar a marca
        if(cobra.gomos[0].x == marca.x && cobra.gomos[0].y == marca.y){
            cobra.gomos.push_back(marca);
            marca = Par(rand() % N_COLUNAS, rand() % N_LINHAS);
            relogio_marca.restart();
        }

        //A cada 5 segundos a marca mudar de local
        if(relogio_marca.getElapsedTime() > sf::milliseconds(5000)){
            relogio_marca.restart();
            marca = Par(rand() % N_COLUNAS, rand() % N_LINHAS);
        }

        //A cada 0,25 segundos a cobra é atualizada em relacao a sua posicao
        if(relogio.getElapsedTime() > sf::milliseconds(250)){
            relogio.restart();
            update_cobra(cobra);
            limitar_cobra(cobra);
            if (colisao_com_corpo(cobra)){
                Par head = cobra.gomos[0];
                cobra.gomos.clear();
                cobra.gomos.push_back(head);
            }
        }

        janela.clear();

        //Abaixo esta apenas acoes relativas ao desing dos gomos em relacao a tabela

        retangulo.setFillColor(sf::Color::Cyan);
        //Cria varios quadrados formando uma tabela
        for(int x = 0; x < N_LINHAS; x++){
            for(int y = 0; y < N_COLUNAS; y++){
                retangulo.setPosition(y * CASA + 2, x * CASA + 2);
                janela.draw(retangulo);
            }
        }

        retangulo.setFillColor(marca.cor);
        retangulo.setPosition(marca.x * CASA + 2, marca.y * CASA + 2);
        janela.draw(retangulo);

        for(auto gomo : cobra.gomos){
            retangulo.setFillColor(gomo.cor);
            retangulo.setPosition(gomo.x * CASA + 2, gomo.y * CASA + 2);
            janela.draw(retangulo);
        }

        janela.display();
    }

    return 0;
}
