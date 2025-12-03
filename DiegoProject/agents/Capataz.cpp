// agents/Capataz.cpp
// Implementación completa del Capataz con IA, movimiento y dibujo

#include "Capataz.h"
#include "../entities/Telar.h"
#include "../utils/ResourceManager.h"
#include <QDebug>
#include <cmath>

Capataz::Capataz(QObject *parent)
    : Agente(parent),
    telares(nullptr),
    telarObjetivo(nullptr),
    posX(0.0f),
    posY(0.0f),
    velocidad(100.0f),
    direccion(FRENTE),
    frameAnimacion(0),
    tiempoAnimacion(0.0f),
    tiempoReparacion(2.0f),
    tiempoReparacionActual(0.0f),
    reparando(false),
    estaQuieto(true) {  // distinguir quieto/caminando

    qDebug() << "Capataz creado";
}

Capataz::~Capataz() {
    qDebug() << "Capataz destruido - Reparaciones:" << historial.size();
}

void Capataz::setTelares(std::vector<Telar*>* t) {
    telares = t;
}

// ACTUALIZACIÓN PRINCIPAL
void Capataz::actualizar(float deltaTime) {
    // Ciclo de IA
    percibir();
    razonar();
    actuar();
    aprender();

    // Actualizar animación solo si se está moviendo
    if (!estaQuieto) {
        actualizarAnimacion(deltaTime);
    } else {
        frameAnimacion = 0;  //Frame de quieto
    }
}

// CICLO DE INTELIGENCIA ARTIFICIAL
void Capataz::percibir() {
    // 1. PERCEPCIÓN: Detectar telares con temperatura > 70%
    if (!reparando && telares != nullptr) {
        int indiceTelarCritico = detectarTelarCritico();

        if (indiceTelarCritico >= 0 && indiceTelarCritico < static_cast<int>(telares->size())) {
            telarObjetivo = (*telares)[indiceTelarCritico];
        } else {
            telarObjetivo = nullptr;
        }
    }
}

void Capataz::razonar() {
    // 2. RAZONAMIENTO: Decidir si ir a reparar
    if (telarObjetivo != nullptr && !reparando) {
        float prioridad = calcularPrioridad(telarObjetivo);

        if (prioridad > 0.7f) {
            qDebug() << "Capataz decide reparar telar (prioridad:" << prioridad << ")";
        }
    }
}

void Capataz::actuar() {
    // 3. ACCIÓN: Moverse y reparar
    if (telarObjetivo != nullptr) {
        float distancia = std::sqrt(
            std::pow(telarObjetivo->getPosX() - posX, 2) +
            std::pow(telarObjetivo->getPosY() - posY, 2)
            );

        if (distancia > 50.0f && !reparando) {
            // Moverse hacia el telar
            moverHacia(telarObjetivo->getPosX(), telarObjetivo->getPosY(), 0.016f);
            estaQuieto = false;
        } else {
            estaQuieto = true;  // Se detuvo al llegar

            // Llegó al telar, comenzar reparación
            if (!reparando) {
                reparando = true;
                tiempoReparacionActual = 0.0f;
                qDebug() << "Capataz comienza reparación";
            }

            // Reparar durante 2 segundos
            tiempoReparacionActual += 0.016f;

            if (tiempoReparacionActual >= tiempoReparacion) {
                repararTelar(telarObjetivo);
                reparando = false;
                tiempoReparacionActual = 0.0f;
                telarObjetivo = nullptr;
                qDebug() << "Capataz terminó reparación";
            }
        }
    } else {
        estaQuieto = true;  // Sin objetivo, está quieto
    }
}

void Capataz::aprender() {
    // 4. APRENDIZAJE: Actualizar frecuencias de fallos
    actualizarFrecuencias();
}

// MÉTODOS AUXILIARES
int Capataz::detectarTelarCritico() {
    if (telares == nullptr || telares->empty()) {
        return -1;
    }

    // Buscar el telar con mayor temperatura
    int indiceCritico = -1;
    float maxTemperatura = 70.0f; // Umbral mínimo

    for (size_t i = 0; i < telares->size(); i++) {
        Telar* telar = (*telares)[i];
        if (telar->getTemperatura() > maxTemperatura) {
            maxTemperatura = telar->getTemperatura();
            indiceCritico = static_cast<int>(i);
        }
    }

    return indiceCritico;
}

float Capataz::calcularPrioridad(Telar* telar) {
    if (telar == nullptr) return 0.0f;

    // Prioridad basada en temperatura
    float prioridadBase = telar->getTemperatura() / 100.0f;

    // Ajustar por historial
    int vecesReparado = frecuenciasFallos[0]; // Simplificado
    float ajustePorHistorial = 1.0f + (vecesReparado * 0.1f);

    return prioridadBase * ajustePorHistorial;
}

void Capataz::moverHacia(float x, float y, float deltaTime) {
    float dx = x - posX;
    float dy = y - posY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if (distancia > 1.0f) {
        posX += (dx / distancia) * velocidad * deltaTime;
        posY += (dy / distancia) * velocidad * deltaTime;

        // Actualizar dirección
        if (std::abs(dx) > std::abs(dy)) {
            direccion = (dx > 0) ? DERECHA : IZQUIERDA;
        } else {
            direccion = (dy > 0) ? FRENTE : ESPALDA;
        }
    }
}

void Capataz::repararTelar(Telar* telar) {
    if (telar == nullptr) return;

    telar->reparar();

    // Registrar evento para aprendizaje
    registrarEvento(1, telar->getTemperatura());

    emit accionRealizada(1);
}

void Capataz::actualizarFrecuencias() {
    for (const auto& registro : historial) {
        if (registro.evento == 1) {
            frecuenciasFallos[0]++;
        }
    }
}

// ANIMACIÓN Y DIBUJO
void Capataz::actualizarAnimacion(float deltaTime) {
    // Solo animar si está caminando
    if (estaQuieto) {
        frameAnimacion = 0;  // Frame de quieto
        return;
    }

    tiempoAnimacion += deltaTime;

    // Cambiar frame cada 0.15 segundos (más lento = más natural)
    if (tiempoAnimacion >= 0.15f) {
        cambiarFrame();
        tiempoAnimacion = 0.0f;
    }
}

void Capataz::cambiarFrame() {
    if (direccion == FRENTE || direccion == ESPALDA) {
        // Intercalar entre frame 1 y 2
        if (frameAnimacion == 1) {
            frameAnimacion = 2;
        } else {
            frameAnimacion = 1;
        }
    } else {  // IZQUIERDA o DERECHA
        // Intercalar entre frame 0 y 1
        frameAnimacion = (frameAnimacion == 0) ? 1 : 0;
    }
}

void Capataz::cargarSprites(const QString& spriteFrente, const QString& spriteEspalda,
                            const QString& spriteIzquierda, const QString& spriteDerecha) {
    this->spriteFrente = ResourceManager::getInstance().getSprite(spriteFrente);
    this->spriteEspalda = ResourceManager::getInstance().getSprite(spriteEspalda);
    this->spriteIzquierda = ResourceManager::getInstance().getSprite(spriteIzquierda);
    this->spriteDerecha = ResourceManager::getInstance().getSprite(spriteDerecha);

    qDebug() << "Sprites del capataz cargados";
}

QPixmap Capataz::getSpriteActual() const {
    switch (direccion) {
    case FRENTE:    return spriteFrente;
    case ESPALDA:   return spriteEspalda;
    case IZQUIERDA: return spriteIzquierda;
    case DERECHA:   return spriteDerecha;
    default:        return spriteFrente;
    }
}

void Capataz::dibujar(QPainter& painter) {
    QPixmap sprite = getSpriteActual();

    if (!sprite.isNull()) {
        int numFrames = 3;  // Por defecto 3 frames

        // Laterales solo tienen 2 frames
        if (direccion == IZQUIERDA || direccion == DERECHA) {
            numFrames = 2;
        }

        int frameWidth = sprite.width() / numFrames;
        int frameHeight = sprite.height();

        QRect sourceRect(frameAnimacion * frameWidth, 0, frameWidth, frameHeight);
        QRect targetRect(posX - 30, posY - 40, 60, 80);

        painter.drawPixmap(targetRect, sprite, sourceRect);
    } else {
        // Fallback
        painter.setBrush(QColor(100, 150, 200));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(posX, posY), 30, 40);
    }

    // Indicador de reparación
    if (reparando) {
        painter.setBrush(QColor(255, 200, 0, 150));
        painter.drawEllipse(QPointF(posX, posY - 50), 20, 20);
    }
}
