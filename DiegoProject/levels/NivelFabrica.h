// levels/NivelFabrica.h
// Nivel 1: La Fábrica Textil

#ifndef NIVELFABRICA_H
#define NIVELFABRICA_H

#include "Nivel.h"
#include <vector>
#include <list>
#include <QRect>

// Forward declarations
class Telar;
class Carrete;
class Capataz;
class ColisionManager;

class NivelFabrica : public Nivel {
    Q_OBJECT

public:
    explicit NivelFabrica(Jugador* jugador, QWidget *parent = nullptr);
    ~NivelFabrica() override;

    void inicializar() override;
    void actualizar() override;

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    // Creación de entidades
    void crearTelares();
    void generarCarrete();

    // Actualización de componentes
    void actualizarCarretes(float deltaTime);
    void actualizarTelares(float deltaTime);
    void actualizarPresionYCaldera(float deltaTime);
    void actualizarGameOver();

    // Verificaciones
    void verificarCondicionesFinales();
    void verificarColisionJugadorCarrete();
    void asignarCarreteATelar(Carrete* carrete, Telar* telar);

    // Dibujo
    void dibujarFondo(QPainter& painter);
    void dibujarBanda(QPainter& painter);
    void dibujarCaldera(QPainter& painter);
    void dibujarBarraPresion(QPainter& painter);
    void dibujarHUD(QPainter& painter);
    void dibujarGameOver(QPainter& painter);

    // Game Over
    void mostrarPantallaGameOver();
    void explotarCaldera();
    void repararCaldera();  // Sistema de reparación de caldera
    void perderVida(const QString&);

    // Entidades del nivel
    std::vector<Telar*> telares;
    std::list<Carrete*> carretes;
    Capataz* capataz;
    ColisionManager* colisionManager;

    // Variables de gameplay
    int metrosProducidos;
    int metasObjetivo;
    float presionVapor;
    int vidasRestantes;
    int telaresAveriados;

    // Sistema de banda y carretes
    int frameAnimacionBanda;
    float tiempoAnimacionBanda;  // para animar banda más lento
    float tiempoDesdeUltimoCarrete;
    float intervaloCarretes;
    Carrete* carreteSeleccionado;

    // Control de movimiento WASD
    bool teclaW, teclaA, teclaS, teclaD;

    // Sistema de caldera
    float tiempoCalderaSobrecalentada;
    bool calderaExplotada;

    // Game Over
    bool mostrarGameOver;
    float tiempoGameOver;
    QRect botonPausaRect;
};

#endif // NIVELFABRICA_H
