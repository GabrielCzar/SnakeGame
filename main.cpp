#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio.hpp>

using namespace std;

const float CASA = 50;
const int LARGURA = 800;
const int EXTRA = 200;
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

void desenhar_cubos(sf::RectangleShape &retangulo, sf::RenderWindow &janela, Par &marca, Cobra &cobra){
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
}

bool aumentar_corpo(Cobra &cobra, Par &marca, sf::Clock &relogio_marca, int &pontuacao){
    //Cobra aumenta ao pegar a marca
    if(cobra.gomos[0].x == marca.x && cobra.gomos[0].y == marca.y){
        cobra.gomos.push_back(marca);
        marca = Par(rand() % N_COLUNAS, rand() % N_LINHAS);
        relogio_marca.restart();
        pontuacao++;
        return true;
    }
    return false;
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
    if (cobra.gomos[0].x >= N_COLUNAS)
        cobra.gomos[0].x = 0;
    //Cima e Embaixo
    if (cobra.gomos[0].y < 0)
        cobra.gomos[0].y = N_LINHAS - 1;
    if (cobra.gomos[0].y >= N_LINHAS)
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

void gerar_marca(Par &marca, sf::Clock &relogio_marca){
    //A cada 5 segundos a marca mudar de local
    if(relogio_marca.getElapsedTime() > sf::milliseconds(5000)){
        relogio_marca.restart();
        marca = Par(rand() % N_COLUNAS, rand() % N_LINHAS);
    }
}

string formatar_tempo(sf::Clock cronometro){
    stringstream cronometro_str;
    int tempo = cronometro.getElapsedTime().asSeconds();
    int seconds = tempo % 60;
    tempo /= 60;
    int minutos = tempo % 60;
    tempo /= 60;
    int horas = tempo % 60;

    if (horas > 0) {cronometro_str << horas << "h" << ":";}

    if (minutos > 0) cronometro_str << minutos << "m" << ":";

    cronometro_str << seconds << "s";

    return cronometro_str.str();
}

vector<int> ler_scores(fstream &file){
    int i = 0;
    int score;
    vector<int> scores;
    scores.reserve(10);
    while (file >> score){
        scores.push_back(score);
        if (i >= 10)
            break;
    }
    return scores;
}

int main(){
    srand(time(NULL));
    sf::Clock relogio;
    sf::Clock relogio_marca;
    sf::Clock cronometro;

    int pontuacao = 0;

    sf::RenderWindow janela(sf::VideoMode(LARGURA + EXTRA, ALTURA), "Snake Game");

//    ofstream file;
  //  file.open("/home/gabriel/github/snake_game/scores.txt", ios::out | ios::in);
    //vector<int> scores = ler_scores(file);

    sf::Music musica;
    if (!musica.openFromFile("/home/gabriel/github/snake_game/soundtrack.ogg")){cout << "ERROR ao carregar musica!\n";}
    musica.play();

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("/home/gabriel/github/snake_game/score.wav")){cout << "ERROR ao carregar sound!\n";}
    sf::Sound sound;
    sound.setBuffer(buffer);

    sf::Font font;
    if (!font.loadFromFile("/home/gabriel/github/snake_game/DroidSansMono.ttf")){cout << "ERROR ao carregar font!\n";}

    sf::Text texto = sf::Text("Snake Game", font), pontos = sf::Text("Pontos: ", font), score = sf::Text("0", font),  time = sf::Text("Tempo: 0s", font);
    texto.setFillColor(sf::Color::Black);
    pontos.setFillColor(sf::Color::Black);
    score.setFillColor(sf::Color::Black);
    time.setFillColor(sf::Color::Black);
    texto.setStyle(sf::Text::Bold);
    texto.setPosition(LARGURA, CASA);
    pontos.setPosition(LARGURA, CASA * 2);
    score.setPosition(LARGURA + 125, CASA * 2);
    time.setPosition(LARGURA, CASA * 3);

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

        if (aumentar_corpo(cobra, marca, relogio_marca, pontuacao))
            sound.play();

        gerar_marca(marca, relogio_marca);

        //A cada 0,25 segundos a cobra é atualizada em relacao a sua posicao
        if(relogio.getElapsedTime() > sf::milliseconds(250)){
            relogio.restart();
            update_cobra(cobra);
            limitar_cobra(cobra);
            if (colisao_com_corpo(cobra)){
                Par head = cobra.gomos[0];
                cobra.gomos.clear();
                cobra.gomos.push_back(head);
                //file << pontuacao;

                pontuacao = 0;
            }
        }

        janela.clear(sf::Color::White);

        //Abaixo esta apenas acoes relativas ao desing dos gomos em relacao a tabela
        desenhar_cubos(retangulo, janela, marca, cobra);

        stringstream score_str;
        score_str << pontuacao;
        score.setString(score_str.str());

        time.setString(formatar_tempo(cronometro));

        janela.draw(texto);
        janela.draw(pontos);
        janela.draw(score);
        janela.draw(time);
        janela.display();
    }

    musica.stop();
    file.close();

    return 0;
}
