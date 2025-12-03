// utils/ColisionManager.cpp
// Implementación del gestor de colisiones

#include "ColisionManager.h"
#include <cmath>

ColisionManager::ColisionManager() {
    caldera = {0, 0, 0, 0};
    banda = {0, 0, 0, 0};
}

ColisionManager::~ColisionManager() {
}

void ColisionManager::configurarCaldera(float x, float y, float ancho, float alto) {
    caldera = {x, y, ancho, alto};
}

void ColisionManager::configurarBanda(float x, float y, float ancho, float alto) {
    banda = {x, y, ancho, alto};
}

void ColisionManager::agregarTelar(float x, float y, float ancho, float alto) {
    telares.append({x, y, ancho, alto});
}

void ColisionManager::limpiarTelares() {
    telares.clear();
}

bool ColisionManager::colisionConRectangulo(float x, float y, float radio, const Colisionable& rect) const {
    // Verifica si un círculo (jugador) colisiona con un rectángulo (objeto)
    // Encuentra el punto más cercano del rectángulo al centro del círculo
    float closestX = std::max(rect.x, std::min(x, rect.x + rect.ancho));
    float closestY = std::max(rect.y, std::min(y, rect.y + rect.alto));

    // Calcula la distancia entre el punto más cercano y el centro del círculo
    float distanciaX = x - closestX;
    float distanciaY = y - closestY;
    float distanciaCuadrada = (distanciaX * distanciaX) + (distanciaY * distanciaY);

    // Hay colisión si la distancia es menor que el radio
    return distanciaCuadrada < (radio * radio);
}

bool ColisionManager::hayColisionConTelar(float x, float y) const {
    for (const auto& telar : telares) {
        if (colisionConRectangulo(x, y, 5.0f, telar)) {
            return true;
        }
    }
    return false;
}

bool ColisionManager::hayColisionConCaldera(float x, float y) const {
    return colisionConRectangulo(x, y, 25.0f, caldera);
}

bool ColisionManager::hayColisionConBanda(float x, float y) const {
    // Usar colisión estándar de rectángulo
    return colisionConRectangulo(x, y, 5.0f, banda);
}

bool ColisionManager::hayColisionConObjeto(float x, float y, float radio) const {
    // Verifica colisión con cualquier objeto
    if (hayColisionConCaldera(x, y)) return true;
    if (hayColisionConBanda(x, y)) return true;
    if (hayColisionConTelar(x, y)) return true;

    return false;
}

void ColisionManager::validarMovimiento(float& x, float& y, float radio) const {
    // Intenta mover a la posición deseada
    // Si hay colisión, ajusta la posición
    if (hayColisionConObjeto(x, y, radio)) {
        // Posición inválida - mantener posición anterior
    }
}
