// agents/AgenteNivel2.h
// Capataz autonomo para nivel 2
// NO hereda de Agente (para no interferir con nivel 1)

#ifndef AGENTE_NIVEL2_H
#define AGENTE_NIVEL2_H

#include <QObject>
#include <QPainter>
#include <QPixmap>

class AgenteNivel2 : public QObject {
    Q_OBJECT

public:
    explicit AgenteNivel2(QObject *parent = nullptr);
    ~AgenteNivel2();

    // Getters
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    bool estaActivo() const { return activo; }
    bool estaPaleando() const { return paleando; }
    int getFramePaleo() const { return framePaleo; }

    // Setters
    void setPosicion(float x, float y);
    void setActivo(bool estado);

    // Metodos principales
    void actualizar(float deltaTime);
    void dibujar(QPainter& painter);
    void iniciarPaleo();
    void cargarSpritesPaleo();

signals:
    void paleoFinalizado();

private:
    // Posicion
    float posX;
    float posY;

    // Estado
    bool activo;
    bool paleando;

    // Animacion de paleo
    int framePaleo;
    float tiempoPaleo;
    float duracionFramePaleo;
    float tiempoDesdeUltimoPaleo;
    float intervaloEntrepaleos;

    // Sprites (5 frames)
    QPixmap spritesPaleo[5];

    // Metodos privados
    void actualizarPaleo(float deltaTime);
    bool debePalear();
};

#endif // AGENTE_NIVEL2_H
