#include "TraineeRole1.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/RobotDefs.h"
#include "Core/Utils/CartesianCoord.h"

#define kickDistance 0.18

// ./build //dentro da pasta mari
// scp src/build-rinobot-robot/sdk/bin/rinobot nao@169.254.44.123:/home/nao
// ssh nao@<ip do robo>
// ./rinobot

TraineeRole1::TraineeRole1(SpellBook *spellBook) : Role(spellBook)
{
    onStart = false;
    timerKick = 0;
    timerHead = 0;
    direcao = 0;
    distanciaParaChutar = 100000;
}
TraineeRole1::~TraineeRole1()
{
}

void TraineeRole1::Tick(float ellapsedTime, const SensorValues &sensor)
{
    // spellBook->strategy.MoveHead = false;
    //  spellBook->motion.HeadYaw = 0;
    spellBook->motion.HeadSpeedYaw = 0.2f; // 5 graus por execucao
    spellBook->motion.HeadSpeedPitch = 0.2f;

    if ((spellBook->strategy.GameState == STATE_READY || spellBook->strategy.GameState == STATE_PLAYING) && !onStart)
    {
        if (spellBook->perception.vision.localization.Enabled)
        {
            spellBook->strategy.WalkForward = true;
            spellBook->strategy.TargetX = -1.0f;
            spellBook->strategy.TargetY = 0.0f;
            spellBook->strategy.TargetTheta = 0;

            CartesianCoord current(spellBook->perception.vision.localization.X, spellBook->perception.vision.localization.Y);
            CartesianCoord desired(spellBook->strategy.TargetX, spellBook->strategy.TargetY);
            if (current.distance(desired) < 0.2f)
            {
                spellBook->strategy.WalkForward = false;
                onStart = true;
            }
        }
        else
        {
            onStart = true;
        }
    }
    if (spellBook->strategy.GameState == STATE_PLAYING && onStart)
    {
        // cout<<"Codigo novo"<<endl;
        // cout << "na role trainee1, " << endl;
        /*spellBook->motion.Vth = Deg2Rad(0); // SETA A VELOCIDADE ANGULAR PARA 0 GRAUS
        spellBook->motion.Vx = 0; // SETA A VELOCIDADE LINEAR em x PARA 0 m/s (NAO COLOQUE MAIS QUE 0.2m/s!!!)
        spellBook->motion.Vy = 0; // SETA A VELOCIDADE LINEAR em y PARA 0 m/s (NAO COLOQUE MAIS QUE 0.2m/s!!!)
       // spellBook->motion.HeadPitch = Deg2Rad(0.0f); // ANGULACAO DA CABECA DO ROBO, POSITIVO O ROBO OLHA PRA BAIXO, NEGATIVO PRA CIMA
       // spellBook->motion.HeadYaw = Deg2Rad(0.0f); // ANGULACAO DA CABECA DO ROBO, POSITIVO O ROBO OLHA PRA BAIXO, NEGATIVO PRA CIMA
        spellBook->motion.KickLeft = false; // CUIDADO! se verdadeiro o robo chuta com a esquerda. CUIDADO! O robo pode cair!
        spellBook->motion.KickRight = false; // CUIDADO! se verdadeiro o robo chuta com a direita. CUIDADO! O robo pode cair!
        */
        // informacoes disponiveis:
        // spellBook->perception.vision.ball.BallDetected // SE ESTA VENDO A BOLA
        // spellBook->perception.vision.ball.BallLostCount // a quantas iteracoes o robo nao ve bola
        // spellBook->perception.vision.ball.TimeSinceBallSeen // a quanto tempo o robo nao ve a bola
        // spellBook->perception.vision.ball.BallDistance // DISTANCIA ATE A BOLA em metros
        // spellBook->perception.vision.ball.BallPitch // ANGULACAO DA BOLA EM pra cima e pra baixo
        // (spellBook->perception.vision.ball.BallYaw > Deg2Rad(10.0f)){ // ANGULACAO DA BOLA pros lados
        // 	cout << "hello!" << endl;
        // }
        // spellBook->motion.HeadPitch = Deg2Rad(24.0f); // OLHA PRA BAIXO

        penalti();
    }
}






// função principal onde se ele esta longe da bola ele tem duas opções, ou ele vai atras dela se a
// esta vendo, ou procura ela caso não esteja a vendo. E se ele está perto da bola realiza os processos de chute ao gol.


//como não conseguimos testar bem os codigos da vth e ficamos mudando bastante eles não sabiamos se estava correto, e tambem eles eram mais uteis em situação de jogo, entao retiramos ele daqui. Mas nos videos do slide esta um pouco da exercusão com os codigos de vth.
void TraineeRole1::penalti()
{
    if (spellBook->perception.vision.ball.BallDistance > kickDistance)
    {
        if (spellBook->perception.vision.ball.BallDetected)
        {
            perseguir();
        }
        else
        {
            spellBook->motion.Vy = 0.0;
            spellBook->motion.Vx = 0.0;
            if(spellBook->perception.vision.ball.TimeSinceBallSeen > 60){ //considerando o tempo em segundos, 1 minutos sem ver a bola 
                procura();
            }
        }
    }
    else
    {
        if (spellBook->perception.vision.ball.BallDistance <= kickDistance)
        {
            distanciaParaChutar = spellBook->perception.vision.ball.BallDistance;
            
        }
    }

    if (distanciaParaChutar <= kickDistance)
    {
        chuta();
    }
}











// FUNÇÃO PRA PROCURAR
bool TraineeRole1::procura()
{
    timerHead++;
    if (timerHead > 45)
    {
        direcao++;
        timerHead = 0;
    }
    switch (direcao)
    {
    case 0:
        if (ProcuraCentro())
        {
            return true;
        }
        break;

    case 1:
        if (ProcuraCentroEmbaixo())
        {
            return true;
        }
        break;

    case 2:
        centralizar();
        break;

    case 3:
        if (ProcuraDireita())
        {
            return true;
        }
        break;

    case 4:
        if (ProcuraDireitaEmbaixo())
        {
            return true;
        }
        break;

    case 5:
        centralizar();
        break;

    case 6:
        if (ProcuraEsquerda())
        {
            return true;
        }
        break;

    case 7:
        if (ProcuraEsquerdaEmbaixo())
        {
            return true;
        }
        break;

    case 8:
        centralizar();
        break;

    default:
        direcao = 0;
        break;
    }
}

// direções para procura
void TraineeRole1::centralizar()
{
    
    spellBook->motion.HeadPitch = Deg2Rad(0.0f);
    spellBook->motion.HeadYaw = Deg2Rad(0.0f);
}
bool TraineeRole1::ProcuraCentro()
{
    if (spellBook->perception.vision.ball.BallDetected)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool TraineeRole1::ProcuraCentroEmbaixo()
{
    abaixaCabeca();
    if (spellBook->perception.vision.ball.BallDetected)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool TraineeRole1::ProcuraDireita()
{
    spellBook->motion.HeadYaw += Deg2Rad(1.0f);
    if (spellBook->perception.vision.ball.BallDetected)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool TraineeRole1::ProcuraDireitaEmbaixo()
{
    abaixaCabeca();
    if (spellBook->perception.vision.ball.BallDetected)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool TraineeRole1::ProcuraEsquerda()
{
    spellBook->motion.HeadYaw += Deg2Rad(-1.0f);
    if (spellBook->perception.vision.ball.BallDetected)
    {
        return true;
    }
    else{
        return false;
    }
}

bool TraineeRole1::ProcuraEsquerdaEmbaixo()
{
    abaixaCabeca();
    if (spellBook->perception.vision.ball.BallDetected)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void TraineeRole1::abaixaCabeca()
{
    spellBook->motion.HeadPitch += Deg2Rad(1.0f);
}












// anda atrás da bola para chegar perto dela
void TraineeRole1::perseguir()
{
    
    spellBook->motion.HeadPitch = -(spellBook->perception.vision.ball.BallPitch); //ir abaixando a cabeça cada vez mais conforme chega mais proximo da bola

    if(spellBook->perception.vision.ball.BallYaw > Deg2Rad(105.0f) || spellBook->perception.vision.ball.BallYaw < Deg2Rad(95.0f)){ //intervalo para estar com pé esquerdo alinhado com a bola // faltou testar mais os valores
        if(spellBook->perception.vision.ball.BallYaw > Deg2Rad(105.0f))
        {
            spellBook->motion.Vy = -0.05;
           
        }
        if(spellBook->perception.vision.ball.BallYaw < Deg2Rad(95.0f)){
            spellBook->motion.Vy = 0.05;
          
        }
    }
    spellBook->motion.Vy = 0.0;
    spellBook->motion.Vx = 0.08;
}











// função para realizar os processos de chute ao gol
void TraineeRole1::chuta()
{
    
    if (getTimerKick() <= 70)
    {
        attTimerKick();
        chute();
    }
    else
    {
        pararDeChutar();
    }
}

// chuta a bola
void TraineeRole1::chute()
{
    spellBook->motion.Vx = 0;
    spellBook->motion.Vy = 0;
    spellBook->motion.Vth = Deg2Rad(0);
    spellBook->motion.HeadPitch = Deg2Rad(0.0f);
    spellBook->motion.HeadYaw = Deg2Rad(0.0f);
    spellBook->motion.KickLeft = true;
}

// para de chutar a bola
void TraineeRole1::pararDeChutar()
{
    spellBook->motion.Vth = Deg2Rad(0);
    spellBook->motion.Vx = 0;
    spellBook->motion.Vy = 0;
    spellBook->motion.HeadPitch = Deg2Rad(0.0f);
    spellBook->motion.HeadYaw = Deg2Rad(0.0f);
    spellBook->motion.KickLeft = false;
}
