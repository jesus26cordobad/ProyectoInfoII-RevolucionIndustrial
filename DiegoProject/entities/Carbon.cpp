// entities/Carbon.cpp
// Implementacion de particula de carbon con fisica

#include "Carbon.h"
#include <QPainter>
#include <cmath>
#include <QDebug>

Carbon::Carbon(float x, float y, float vx, float vy, TipoColision tipo, QObject *parent)
    : QObject(parent),
    posX(x), posY(y),
    velX(vx), velY(vy),
    radio(RADIO_CARBON),
    masa(MASA_CARBON),
    tipoColision(tipo),
    activo(true),
    pegado(false),
    tiempoVida(0.0f) {
}

Carbon::~Carbon() {
}

void Carbon::actualizar(float deltaTime) {
    if (!activo || pegado) return;

    // Actualizar posicion
    posX += velX * deltaTime;
    posY += velY * deltaTime;

    // Actualizar tiempo de vida
    tiempoVida += deltaTime;
    if (tiempoVida > TIEMPO_VIDA_MAX) {
        activo = false;
        emit carbonDesactivado();
    }
}

void Carbon::aplicarGravedad(float g, float deltaTime) {
    if (!activo || pegado) return;

    // Aplicar aceleracion gravitacional
    velY += g * deltaTime;
}

void Carbon::aplicarFriccion(float coef) {
    if (!activo || pegado) return;

    // Aplicar friccion del aire
    velX *= (1.0f - coef);
    velY *= (1.0f - coef);
}

bool Carbon::colisionaCon(const Carbon* otro) const {
    if (!activo || !otro->estaActivo()) return false;

    float dx = posX - otro->getPosX();
    float dy = posY - otro->getPosY();
    float distancia = std::sqrt(dx * dx + dy * dy);

    return distancia < (radio + otro->getRadio());
}

void Carbon::resolverColisionElastica(Carbon* otro) {
    // Colision elastica: conserva momento y energia cinetica
    float dx = otro->getPosX() - posX;
    float dy = otro->getPosY() - posY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if (distancia == 0) return;

    // Vector normal
    float nx = dx / distancia;
    float ny = dy / distancia;

    // Velocidad relativa
    float dvx = velX - otro->getVelX();
    float dvy = velY - otro->getVelY();

    // Velocidad relativa en direccion normal
    float velocidadRelativa = dvx * nx + dvy * ny;

    // No resolver si se alejan
    if (velocidadRelativa > 0) return;

    // Impulso (masas iguales)
    float impulso = 2.0f * velocidadRelativa / (masa + otro->masa);

    // Aplicar impulso
    velX -= impulso * otro->masa * nx;
    velY -= impulso * otro->masa * ny;
    otro->setVelocidad(otro->getVelX() + impulso * masa * nx,
                       otro->getVelY() + impulso * masa * ny);
}

void Carbon::resolverColisionInelastica(Carbon* otro, float coefRestitucion) {
    // Colision inelastica: pierde energia segun coeficiente de restitucion
    float dx = otro->getPosX() - posX;
    float dy = otro->getPosY() - posY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if (distancia == 0) return;

    float nx = dx / distancia;
    float ny = dy / distancia;

    float dvx = velX - otro->getVelX();
    float dvy = velY - otro->getVelY();
    float velocidadRelativa = dvx * nx + dvy * ny;

    if (velocidadRelativa > 0) return;

    // Impulso con coeficiente de restitucion
    float impulso = -(1.0f + coefRestitucion) * velocidadRelativa / (masa + otro->masa);

    velX -= impulso * otro->masa * nx;
    velY -= impulso * otro->masa * ny;
    otro->setVelocidad(otro->getVelX() + impulso * masa * nx,
                       otro->getVelY() + impulso * masa * ny);
}

void Carbon::resolverColisionPerfectamenteInelastica(Carbon* otro) {
    // Colision perfectamente inelastica: quedan pegados
    // Conserva momento pero no energia
    float masaTotal = masa + otro->masa;

    float velFinalX = (masa * velX + otro->masa * otro->getVelX()) / masaTotal;
    float velFinalY = (masa * velY + otro->masa * otro->getVelY()) / masaTotal;

    velX = velFinalX;
    velY = velFinalY;
    otro->setVelocidad(velFinalX, velFinalY);

    // Marcar como pegados
    pegado = true;
    otro->setPegado(true);
}

bool Carbon::colisionaConSuelo(float alturaSuelo) const {
    return (posY + radio) >= alturaSuelo;
}

void Carbon::rebotarEnSuelo(float alturaSuelo, float coefRestitucion) {
    if (!activo) return;

    // Ajustar posicion
    posY = alturaSuelo - radio;

    // Invertir velocidad vertical con perdida de energia
    velY = -velY * coefRestitucion;

    // Si la velocidad es muy baja, detener
    if (std::abs(velY) < 10.0f) {
        velY = 0;
        pegado = true;
    }
}

void Carbon::dibujar(QPainter& painter) {
    if (!activo) return;

    painter.save();

    // Dibujar carbon como circulo negro con borde
    painter.setBrush(QColor(40, 40, 40));
    painter.setPen(QPen(QColor(20, 20, 20), 2));
    painter.drawEllipse(QPointF(posX, posY), radio, radio);

    // Agregar brillo
    painter.setBrush(QColor(80, 80, 80, 100));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(posX - radio/3, posY - radio/3), radio/3, radio/3);

    painter.restore();
}
