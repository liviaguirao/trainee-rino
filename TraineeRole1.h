#ifndef RINO_PLAYER_H
#define RINO_PLAYER_H

#include "Role.h"

using namespace std;

class TraineeRole1: public Role {

private:
    bool onStart;
    int timerKick;
 
public:

    float timerHead;
    int direcao;
    float distanciaParaChutar;
    

    int getTimerKick(){
        return timerKick;
    }

    void attTimerKick(){
        timerKick++;
    }
    
    TraineeRole1(SpellBook *spellBook);
    ~TraineeRole1();
    void Tick(float ellapsedTime, const SensorValues&);
    void penalti();
    bool procura();
    void centralizar();
    bool ProcuraCentro();
    bool ProcuraCentroEmbaixo();
    bool ProcuraDireita();
    bool ProcuraDireitaEmbaixo();
    bool ProcuraEsquerda();
    bool ProcuraEsquerdaEmbaixo();
    void perseguir();
    void chuta();
    void chute();
    void pararDeChutar();
    void abaixaCabeca();


};
#endif