// agents/TrenRival.cpp
// Implementación del agente TrenRival

#include "TrenRival.h"
#include "../entities/Tren.h"
#include <QDebug>
#include <cmath>

TrenRival::TrenRival(QObject *parent)
    : Agente(parent),
    trenJugador(nullptr),
    velocidad(50.0f),
    velocidadMaxima(100.0f),
    viaActual(1),
    viaObjetivo(1),
    evitandoColision(false),
    distanciaSeguridad(150.0f) {

    qDebug() << "TrenRival creado";
}

TrenRival::~TrenRival() {
    qDebug() << "TrenRival destruido - Evasiones:" << colisionesPasadas.size();
}

void TrenRival::setTrenJugador(Tren* tren) {
    trenJugador = tren;
}

// ============================================
// CICLO DE INTELIGENCIA ARTIFICIAL
// ============================================

void TrenRival::percibir() {
    // 1. PERCEPCIÓN: Detectar distancia al jugador y recursos
    if (trenJugador != nullptr) {
        float distancia = calcularDistanciaAlJugador();

        if (distancia < distanciaSeguridad) {
            evitandoColision = true;
        } else {
            evitandoColision = false;
        }

        // Detectar si hay recursos (carbón) cerca
        bool hayRecursos = hayRecursosCerca();

        if (hayRecursos) {
            qDebug() << "TrenRival detecta recursos cercanos";
        }
    }
}

void TrenRival::razonar() {
    // 2. RAZONAMIENTO: Decidir acción basándose en percepción

    if (evitandoColision) {
        // Prioridad 1: Evitar colisión
        decidirCambioVia();
        qDebug() << "TrenRival decide cambiar de vía para evitar colisión";
    } else {
        // Buscar recursos o mantener curso
        if (hayRecursosCerca()) {
            // Competir por recursos
            ajustarVelocidad();
        }
    }

    // Consultar historial: ¿Esta zona es peligrosa?
    if (esZonaPeligrosa(posX, viaActual)) {
        qDebug() << "TrenRival reconoce zona peligrosa (aprendizaje previo)";
        decidirCambioVia();
    }
}

void TrenRival::actuar() {
    // 3. ACCIÓN: Ejecutar decisión

    // Cambiar de vía gradualmente
    if (viaActual != viaObjetivo) {
        viaActual = viaObjetivo;
        emit accionRealizada(2); // Tipo 2: cambio de vía
    }

    // Mover el tren
    posX += velocidad * 0.016f; // Avanzar

    // Ajustar velocidad según situación
    if (evitandoColision) {
        velocidad = std::max(30.0f, velocidad - 5.0f);
    } else {
        velocidad = std::min(velocidadMaxima, velocidad + 2.0f);
    }
}

void TrenRival::aprender() {
    // 4. APRENDIZAJE: Registrar situaciones peligrosas

    if (evitandoColision) {
        // Guardar esta posición y vía como peligrosa
        registrarColision(posX, viaActual);
        registrarEvento(2, posX); // Evento tipo 2: evasión
    }
}

// ============================================
// MÉTODOS AUXILIARES
// ============================================

float TrenRival::calcularDistanciaAlJugador() {
    if (trenJugador == nullptr) return 999.0f;

    // TODO: Usar posición real del tren jugador cuando esté implementado
    // Por ahora retorna distancia simulada
    float distancia = 200.0f;
    return distancia;
}

bool TrenRival::hayPeligroColision() {
    return calcularDistanciaAlJugador() < distanciaSeguridad;
}

bool TrenRival::hayRecursosCerca() {
    // TODO: Implementar detección de carbón cercano
    // Por ahora retorna false
    return false;
}

void TrenRival::decidirCambioVia() {
    // Cambiar a vía alternativa (simplificado: vía 1 <-> vía 2)
    viaObjetivo = (viaActual == 1) ? 2 : 1;
}

void TrenRival::ajustarVelocidad() {
    // Aumentar velocidad para competir por recursos
    velocidad = std::min(velocidadMaxima, velocidad + 10.0f);
}

void TrenRival::registrarColision(float posX, int via) {
    colisionesPasadas.emplace_back(posX, via);

    // Limitar historial a últimas 20 colisiones
    if (colisionesPasadas.size() > 20) {
        colisionesPasadas.pop_front();
    }

    qDebug() << "TrenRival registra zona peligrosa en X=" << posX << "Vía=" << via;
}

bool TrenRival::esZonaPeligrosa(float posX, int via) {
    // Consultar historial: ¿Ya chocamos aquí antes?
    for (const auto& colision : colisionesPasadas) {
        float distancia = std::abs(colision.posicionX - posX);

        if (distancia < 50.0f && colision.viaID == via) {
            return true; // ¡Zona peligrosa conocida!
        }
    }

    return false;
}
