// core/GameController.cpp
// Implementación del controlador principal del juego

#include "GameController.h"
#include "../entities/Jugador.h"
#include "../levels/Nivel.h"
#include "../levels/NivelFabrica.h"
#include "../levels/NivelFerrocarril.h"
#include "../levels/NivelPuente.h"
#include <stdexcept>
#include <QDebug>

GameController::GameController(QObject *parent)
    : QObject(parent),
    nivelActual(0),
    jugador(nullptr),
    nivelEnCurso(nullptr),
    jugandoActualmente(false),
    pausado(false) {

    // Crear el jugador principal
    jugador = new Jugador(this);

    qDebug() << "GameController inicializado correctamente";
}

GameController::~GameController() {
    limpiarNivel();
    // jugador se elimina automáticamente (parent-child)
}

void GameController::iniciarNivel(int numeroNivel) {
    if (numeroNivel < 1 || numeroNivel > 3) {
        throw std::invalid_argument("Número de nivel inválido. Debe ser 1, 2 o 3.");
    }

    // Limpiar nivel anterior si existe
    if (nivelEnCurso != nullptr) {
        limpiarNivel();
    }

    try {
        nivelActual = numeroNivel;
        crearNivel(numeroNivel);
        jugandoActualmente = true;
        pausado = false;

        qDebug() << "Nivel" << numeroNivel << "iniciado correctamente";

        // Inicializar y mostrar el nivel
        nivelEnCurso->inicializar();
        nivelEnCurso->show();

    } catch (const std::exception& e) {
        qCritical() << "Error al iniciar nivel:" << e.what();
        limpiarNivel();
        throw;
    }
}

void GameController::crearNivel(int numero) {
    // Crear el nivel apropiado según el número
    switch (numero) {
    case 1:
        nivelEnCurso = new NivelFabrica(jugador, nullptr);
        break;
    case 2:
        nivelEnCurso = new NivelFerrocarril(jugador, nullptr);
        break;
    case 3:
        nivelEnCurso = new NivelPuente(jugador, nullptr);
        break;
    default:
        throw std::runtime_error("Nivel no implementado");
    }

    // Conectar señales del nivel
    connect(nivelEnCurso, &Nivel::nivelCompletado,
            this, &GameController::finalizarNivel);
    connect(nivelEnCurso, &Nivel::puntajeModificado,
            this, &GameController::puntajeActualizado);
}

void GameController::finalizarNivel(bool victoria) {
    jugandoActualmente = false;

    if (victoria) {
        qDebug() << "¡Nivel" << nivelActual << "completado!";
        guardarProgreso();
    } else {
        qDebug() << "Nivel" << nivelActual << "fallido";
    }

    // Limpiar nivel actual
    limpiarNivel();

    // Emitir señal para volver al menú
    emit nivelTerminado();
}

void GameController::limpiarNivel() {
    if (nivelEnCurso != nullptr) {
        nivelEnCurso->hide();
        nivelEnCurso->deleteLater(); // Eliminación segura en Qt
        nivelEnCurso = nullptr;
    }
    nivelActual = 0;
}

void GameController::pausarJuego() {
    if (jugandoActualmente && !pausado) {
        pausado = true;
        if (nivelEnCurso != nullptr) {
            nivelEnCurso->pausar();
        }
        qDebug() << "Juego pausado";
    }
}

void GameController::reanudarJuego() {
    if (jugandoActualmente && pausado) {
        pausado = false;
        if (nivelEnCurso != nullptr) {
            nivelEnCurso->reanudar();
        }
        qDebug() << "Juego reanudado";
    }
}

void GameController::guardarProgreso() {
    // Aquí se podría implementar guardado de progreso
    // Por ahora solo registramos el evento
    qDebug() << "Progreso guardado - Nivel" << nivelActual << "completado";

    // Actualizar puntaje del jugador
    emit puntajeActualizado(jugador->getPuntaje());
}
