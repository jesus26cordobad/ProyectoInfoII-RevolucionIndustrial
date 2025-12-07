// entities/Carbon.h
// Particula de carbon con fisica de colisiones para Nivel 2

#ifndef CARBON_H
#define CARBON_H

#include <QObject>
#include <QPixmap>
#include <QPainter>
#include <QPointF>

class Carbon : public QObject {
    Q_OBJECT

public:
    enum TipoColision {
        ELASTICA,              // Rebota completamente
        INELASTICA,            // Rebota pero pierde energia
        PERFECTAMENTE_INELASTICA  // No rebota, se queda pegado
    };

    explicit Carbon(float x, float y, float vx, float vy,
                    TipoColision tipo, QObject *parent = nullptr);
    ~Carbon();

    // Getters
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    float getVelX() const { return velX; }
    float getVelY() const { return velY; }
    float getRadio() const { return radio; }
    bool estaActivo() const { return activo; }
    bool estaPegado() const { return pegado; }
    TipoColision getTipoColision() const { return tipoColision; }

    // Setters
    void setVelocidad(float vx, float vy) { velX = vx; velY = vy; }
    void setPegado(bool p) { pegado = p; }
    void desactivar() { activo = false; }

    // Fisica
    void actualizar(float deltaTime);
    void aplicarGravedad(float g, float deltaTime);
    void aplicarFriccion(float coef);

    // Colisiones
    bool colisionaCon(const Carbon* otro) const;
    void resolverColisionElastica(Carbon* otro);
    void resolverColisionInelastica(Carbon* otro, float coefRestitucion);
    void resolverColisionPerfectamenteInelastica(Carbon* otro);
    bool colisionaConSuelo(float alturaSuelo) const;
    void rebotarEnSuelo(float alturaSuelo, float coefRestitucion);

    // Dibujo
    void dibujar(QPainter& painter);

signals:
    void carbonDesactivado();

private:
    // Posicion y velocidad
    float posX, posY;
    float velX, velY;

    // Propiedades fisicas
    float radio;
    float masa;
    TipoColision tipoColision;

    // Estado
    bool activo;
    bool pegado;
    float tiempoVida;

    // Constantes
    static constexpr float RADIO_CARBON = 4.0f;
    static constexpr float MASA_CARBON = 1.0f;
    static constexpr float TIEMPO_VIDA_MAX = 10.0f;
};

#endif // CARBON_H
