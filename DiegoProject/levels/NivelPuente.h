// levels/NivelPuente.h
#ifndef NIVELPUENTE_H
#define NIVELPUENTE_H

#include "Nivel.h"
#include <vector>
#include <QPixmap>
#include <QRect>

class Viga;
class Moneda;
class Ladron;
class HUDRenderer;

class NivelPuente : public Nivel {
    Q_OBJECT

public:
    explicit NivelPuente(Jugador* jugador, QWidget *parent = nullptr);
    ~NivelPuente() override;

    void inicializar() override;
    void actualizar() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    // Entidades
    std::vector<Viga*> vigas;
    std::vector<Moneda*> monedas;
    Ladron* ladron;
    HUDRenderer* hudRenderer;

    // Sprites
    QPixmap fondoNivel3;
    QPixmap spriteMoneda;
    QPixmap spriteJugadorCorrDer, spriteJugadorCorrIzq;
    QPixmap spriteJugadorSaltoDer, spriteJugadorSaltoIzq;
    QPixmap spriteLadronCorrDer, spriteLadronCorrIzq;
    QPixmap spriteLadronSaltoDer, spriteLadronSaltoIzq;

    // Estado del juego
    int monedasRecolectadas;
    int monedasObjetivo;
    float tiempoDesdeUltimaMoneda;
    float tiempoTranscurridoTotal;
    bool teclaA, teclaD, teclaW;

    // Game Over
    bool mostrarGameOver;
    QString razonGameOver;
    QRect botonPausaRect;

    // Métodos privados
    void crearVigas();
    void generarMoneda();
    void actualizarJugador(float deltaTime);
    void actualizarLadron(float deltaTime);
    void actualizarMonedas(float deltaTime);
    void verificarColisionesVigas();
    void verificarColisionesMonedas();
    void verificarColisionLadron();
    void verificarCondicionesFinales();
    void aumentarDificultad();
    void verificarColisionesLadronVigas();

    void dibujarFondo(QPainter& painter);
    void dibujarVigas(QPainter& painter);
    void dibujarMonedas(QPainter& painter);
    void dibujarLadron(QPainter& painter);
    void dibujarJugador(QPainter& painter);
    void dibujarHUD(QPainter& painter);
    void dibujarGameOver(QPainter& painter);
    void mostrarPantallaGameOver(const QString& razon);

    // Constantes
    static constexpr int MONEDAS_OBJETIVO = 30;
    static constexpr float TIEMPO_GENERAR_MONEDA = 4.0f;
    static constexpr float INCREMENTO_VELOCIDAD_LADRON = 10.0f;
    static constexpr float INTERVALO_AUMENTO_DIFICULTAD = 15.0f;
    static constexpr int MONEDAS_PERDIDAS_POR_TOQUE = 2;
};

#endif
