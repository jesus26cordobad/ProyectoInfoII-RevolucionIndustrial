// levels/NivelFabrica.cpp
// Implementación REFACTORIZADA del Nivel 1: La Fábrica Textil

#include "NivelFabrica.h"
#include "../entities/Jugador.h"
#include "../entities/Telar.h"
#include "../entities/Carrete.h"
#include "../agents/Capataz.h"
#include "../utils/ColisionManager.h"
#include "../utils/ResourceManager.h"
#include "../ui/HUDRenderer.h"
#include "../ui/GameOverScreen.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>
#include <cmath>

NivelFabrica::NivelFabrica(Jugador* jugador, QWidget *parent)
    : Nivel(jugador, parent),
    capataz(nullptr),
    colisionManager(nullptr),
    metrosProducidos(0),
    metasObjetivo(50),
    presionVapor(0.0f),
    vidasRestantes(5),
    telaresAveriados(0),
    frameAnimacionBanda(0),
    tiempoAnimacionBanda(0.0f),
    tiempoDesdeUltimoCarrete(0.0f),
    intervaloCarretes(3.0f),
    carreteSeleccionado(nullptr),
    teclaW(false), teclaA(false), teclaS(false), teclaD(false),
    tiempoCalderaSobrecalentada(0.0f),
    calderaExplotada(false),
    mostrarGameOver(false),
    tiempoGameOver(0.0f) {

    setWindowTitle("Nivel 1: La Fábrica Textil");
    tiempoLimite = 300.0f;
    botonPausaRect = QRect(900, 10, 100, 40);
}

NivelFabrica::~NivelFabrica() {
    for (auto telar : telares) delete telar;
    for (auto carrete : carretes) delete carrete;
    delete capataz;
    delete colisionManager;
}

// INICIALIZACIÓN
void NivelFabrica::inicializar() {
    qDebug() << "Inicializando Nivel Fábrica...";

    ResourceManager::getInstance().cargarRecursos();

    // Configurar jugador (USA MÓDULO Jugador.cpp)
    jugador->setPosicion(400, 140);
    jugador->cargarSprites("jugador_frente", "jugador_espalda",
                           "jugador_izquierda", "jugador_derecha");
    vidasRestantes = jugador->getVidas();

    colisionManager = new ColisionManager();
    crearTelares();

    // Crear capataz (USA MÓDULO Capataz.cpp)
    capataz = new Capataz(this);
    capataz->setPosicion(100, 500);
    capataz->setTelares(&telares);
    capataz->cargarSprites("capataz_frente", "capataz_espalda",
                           "capataz_izquierda", "capataz_derecha");

    colisionManager->configurarCaldera(120, 180, 130, 130);
    colisionManager->configurarBanda(650, 550, 350, 120);

    tiempoTranscurrido = 0.0f;
    completado = false;

    relojNivel.start();
    timerJuego->start(16);
    qDebug() << "Nivel Fábrica iniciado";
}

void NivelFabrica::crearTelares() {
    // Crear 9 telares en cuadrícula 3x3
    // Posiciones: 3 columnas x 3 filas
    float posicionesX[] = {250, 460, 670};  // Columnas: izquierda, centro, derecha
    float posicionesY[] = {160, 300, 440};  // Filas: arriba, medio, abajo

    QStringList coloresPorColumna[] = {
        {"verde", "verde", "verde"},    // Columna izquierda
        {"azul", "azul", "azul"},       // Columna centro
        {"rojo", "rojo", "rojo"}        // Columna derecha
    };

    for (int col = 0; col < 3; col++) {
        for (int fila = 0; fila < 3; fila++) {
            float x = posicionesX[col];
            float y = posicionesY[fila];

            Telar* telar = new Telar(x, y, this);
            telar->setColorAsignado(coloresPorColumna[col][fila]);
            telares.push_back(telar);

            colisionManager->agregarTelar(x, y, 120, 120);
        }
    }

    qDebug() << "9 telares creados en cuadrícula 3x3";
}

void NivelFabrica::generarCarrete() {
    QStringList colores = {"rojo", "azul", "verde"};
    QString colorAleatorio = colores[rand() % 3];
    float velocidad = 10.0f + (dificultad * 10.0f);

    Carrete* nuevoCarrete = new Carrete(colorAleatorio, velocidad, this);
    nuevoCarrete->setPosicion(910, 600);

    connect(nuevoCarrete, &Carrete::carretePerdido, this, [this]() {
        perderVida("Carrete perdido");
    });

    carretes.push_back(nuevoCarrete);
}

// ACTUALIZACIÓN
void NivelFabrica::actualizar() {
    if (mostrarGameOver) {
        actualizarGameOver();
        return;
    }

    float deltaTime = 0.016f;

    // Detectar si hay movimiento
    bool hayMovimiento = teclaW || teclaS || teclaA || teclaD;

    // DELEGA a Jugador.cpp
    if (hayMovimiento) {
        jugador->actualizarMovimiento(teclaW, teclaS, teclaA, teclaD, deltaTime);
        jugador->actualizarAnimacion(deltaTime);
    }

    // DELEGA a ColisionManager.cpp
    float velocidadJugador = 200.0f;
    float x = jugador->getPosX();
    float y = jugador->getPosY();

    if (colisionManager->hayColisionConObjeto(x, y, 30.0f)) {
        // Revertir movimiento
        if (teclaW) jugador->mover(0, velocidadJugador * deltaTime);
        if (teclaS) jugador->mover(0, -velocidadJugador * deltaTime);
        if (teclaA) jugador->mover(velocidadJugador * deltaTime, 0);
        if (teclaD) jugador->mover(-velocidadJugador * deltaTime, 0);
    }

    // LÍMITES DE LA FÁBRICA (PAREDES)
    x = std::max(120.0f, std::min(900.0f, jugador->getPosX()));
    y = std::max(140.0f, std::min(630.0f, jugador->getPosY()));
    jugador->setPosicion(x, y);

    actualizarCarretes(deltaTime);
    actualizarTelares(deltaTime);
    actualizarPresionYCaldera(deltaTime);

    // DELEGA a Capataz.cpp (IA completa)
    if (capataz) {
        capataz->actualizar(deltaTime);

        // LÍMITES DE LA FÁBRICA PARA CAPATAZ
        float capX = std::max(120.0f, std::min(900.0f, capataz->getPosX()));
        float capY = std::max(140.0f, std::min(630.0f, capataz->getPosY()));
        capataz->setPosicion(capX, capY);
    }

    verificarColisionJugadorCarrete();
    verificarCondicionesFinales();
}

void NivelFabrica::actualizarCarretes(float deltaTime) {
    tiempoDesdeUltimoCarrete += deltaTime;
    if (tiempoDesdeUltimoCarrete >= intervaloCarretes) {
        generarCarrete();
        tiempoDesdeUltimoCarrete = 0.0f;
        intervaloCarretes = std::max(4.0f, 6.0f - (dificultad * 0.8f));
    }

    // DELEGA a Carrete.cpp
    auto it = carretes.begin();
    while (it != carretes.end()) {
        Carrete* carrete = *it;
        carrete->actualizar(deltaTime);

        // INICIO: Carrete cae al final de la banda
        if (!carrete->estaCayendo() && carrete->getPosX() < 650) {
            carrete->setCayendo(true);
            qDebug() << "Carrete cayendo al final de la banda";
        }

        // PRIMER CONTACTO CON SUELO: Iniciar parábola
        const float SUELO_Y = 625.0f;

        if (carrete->estaCayendo() && !carrete->estaGirado() &&
            !carrete->estaDestruido() && carrete->getPosY() >= SUELO_Y) {
            // Primer toque: iniciar parábola
            carrete->setPosicion(carrete->getPosX(), SUELO_Y);
            carrete->iniciarParabola();
            qDebug() << "Carrete toca suelo - iniciando parábola";
        }

        // SEGUNDO CONTACTO CON SUELO: Destruir (colisión inelástica)
        if (carrete->estaCayendo() && carrete->estaGirado() &&
            !carrete->estaDestruido() && carrete->getPosY() >= SUELO_Y) {
            // Segundo toque: DESTRUIR
            carrete->setPosicion(carrete->getPosX(), SUELO_Y);
            carrete->destruir();

            // PERDER VIDA JUSTO AL DESTRUIRSE
            if (carrete != carreteSeleccionado) {
                perderVida("¡Carrete destruido!");
                qDebug() << "¡Carrete destruido! Vida perdida";
            }
        }

        // ELIMINAR: Cuando termine de desvanecerse
        if (carrete->estaDestruido() && carrete->getPosY() >= SUELO_Y) {
            // Esperar 2 segundos antes de eliminar (para que se vea desvanecerse)
            static QMap<Carrete*, float> tiemposDestruccion;

            if (!tiemposDestruccion.contains(carrete)) {
                tiemposDestruccion[carrete] = 0.0f;
            }

            tiemposDestruccion[carrete] += deltaTime;

            if (tiemposDestruccion[carrete] >= 2.0f) {
                tiemposDestruccion.remove(carrete);
                delete carrete;
                it = carretes.erase(it);
                continue;
            }
        }

        // Eliminar carretes que salgan muy fuera de la pantalla
        if (carrete->getPosX() < 300 || carrete->getPosY() > 800) {
            delete carrete;
            it = carretes.erase(it);
        } else {
            ++it;
        }
    }

    // ANIMACIÓN DE BANDA
    tiempoAnimacionBanda += deltaTime;
    if (tiempoAnimacionBanda >= 0.3f) {
        frameAnimacionBanda = (frameAnimacionBanda + 1) % 2;
        tiempoAnimacionBanda = 0.0f;
    }
}

void NivelFabrica::actualizarTelares(float deltaTime) {
    telaresAveriados = 0;

    // DELEGA a Telar.cpp
    for (auto telar : telares) {
        telar->actualizar(deltaTime);
        if (!telar->estaFuncionando()) {
            telaresAveriados++;
        }
    }

    // Sistema de dificultad
    if (metrosProducidos > 20 && dificultad == 1) dificultad = 2;
    else if (metrosProducidos > 35 && dificultad == 2) dificultad = 3;
}

void NivelFabrica::actualizarPresionYCaldera(float deltaTime) {
    float incremento = 1.0f * deltaTime;

    for (auto telar : telares) {
        if (telar->estaFuncionando() && telar->getTemperatura() > 50.0f) {
            incremento += 0.5f * deltaTime;
        }
    }

    presionVapor = std::min(100.0f, presionVapor + incremento);

    if (presionVapor >= 100.0f) {
        tiempoCalderaSobrecalentada += deltaTime;
        if (tiempoCalderaSobrecalentada >= 5.0f && !calderaExplotada) {
            explotarCaldera();
        }
    } else {
        tiempoCalderaSobrecalentada = 0.0f;
    }
}

void NivelFabrica::verificarColisionJugadorCarrete() {
    // Implementado en mousePressEvent
}

void NivelFabrica::asignarCarreteATelar(Carrete* carrete, Telar* telar) {
    if (!carrete || !telar) {
        qDebug() << "Error: carrete o telar nulo";
        return;
    }

    QString colorCarrete = carrete->getColor();
    QString colorTelar = telar->getColorAsignado();

    qDebug() << " Comparando:";
    qDebug() << "   Carrete:" << colorCarrete;
    qDebug() << "   Telar:" << colorTelar;
    qDebug() << "   Posición telar: (" << telar->getPosX() << "," << telar->getPosY() << ")";

    if (colorCarrete == colorTelar) {
        // COLOR CORRECTO: Procesar
        qDebug() << "¡Colores coinciden! Procesando...";

        // DELEGA a Telar.cpp para reiniciar temperatura
        telar->procesarCarrete(colorCarrete);

        // Aumentar metros producidos
        metrosProducidos += 10;
        jugador->agregarPuntaje(10);

        // Reducir presión de la caldera
        presionVapor = std::max(0.0f, presionVapor - 15.0f);

        // ELIMINAR CARRETE Y LIMPIAR SELECCIÓN
        delete carrete;
        carreteSeleccionado = nullptr;

        qDebug() << " Carrete procesado exitosamente. Metros:" << metrosProducidos << "/" << metasObjetivo;
    } else {
        //  COLOR INCORRECTO: No hacer nada
        qDebug() << " Colores NO coinciden (" << colorCarrete << "≠" << colorTelar << "). Carrete sigue seleccionado.";
    }
}

void NivelFabrica::verificarCondicionesFinales() {
    if (metrosProducidos >= metasObjetivo) {
        completarNivel(true);
    }
    if ((vidasRestantes <= 0 || calderaExplotada) && !mostrarGameOver) {
        mostrarPantallaGameOver();
    }
}

// GAME OVER
void NivelFabrica::mostrarPantallaGameOver() {
    mostrarGameOver = true;
    pausado = true;
}

void NivelFabrica::explotarCaldera() {
    calderaExplotada = true;
    mostrarPantallaGameOver();
}

void NivelFabrica::repararCaldera() {
    // Sistema simplificado: 1 click = presión a 0%

    if (presionVapor > 0.1f) {
        float presionAntes = presionVapor;
        presionVapor = 0.0f;
        tiempoCalderaSobrecalentada = 0.0f;  // Resetear contador de explosión

        qDebug() << "Caldera reparada:" << presionAntes << "% → 0%";
        jugador->agregarPuntaje(15);
    } else {
        qDebug() << "Caldera ya está en 0%, no necesita reparación";
    }
}

void NivelFabrica::perderVida(const QString&) {
    vidasRestantes--;
    jugador->perderVida();

    if (vidasRestantes <= 0) {
        mostrarPantallaGameOver();
    }
}

void NivelFabrica::actualizarGameOver() {
    tiempoGameOver += 0.016f;
}

// DIBUJO
void NivelFabrica::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (mostrarGameOver) {
        dibujarGameOver(painter);
        return;
    }

    dibujarFondo(painter);
    dibujarCaldera(painter);
    dibujarBanda(painter);

    // DELEGA a cada entidad su propio dibujo
    for (auto telar : telares) telar->dibujar(painter);
    for (auto carrete : carretes) carrete->dibujar(painter);
    jugador->dibujar(painter);
    if (capataz) capataz->dibujar(painter);

    dibujarBarraPresion(painter);
    dibujarHUD(painter);
}

void NivelFabrica::dibujarFondo(QPainter& painter) {
    QPixmap fondo = ResourceManager::getInstance().getSprite("fondo_fabrica");
    if (!fondo.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), fondo);
    } else {
        painter.fillRect(rect(), QColor(60, 50, 45));
    }
}

void NivelFabrica::dibujarBanda(QPainter& painter) {
    QString spriteKey = (frameAnimacionBanda == 0) ?
                            "banda_transportadora1" : "banda_transportadora2";
    QPixmap banda = ResourceManager::getInstance().getSprite(spriteKey);

    // POSICIÓN DE LA BANDA
    int bandaX = 650;  // Posición X de la banda
    int bandaY = 550;  // Posición Y de la banda
    int bandaAncho = 350;  // Ancho de la banda
    int bandaAlto = 120;   // Alto de la banda

    if (!banda.isNull()) {
        painter.drawPixmap(bandaX, bandaY, bandaAncho, bandaAlto, banda);
    } else {
        painter.fillRect(bandaX, bandaY, bandaAncho, bandaAlto, QColor(80, 80, 80));
        painter.setPen(QPen(Qt::white, 2));
        for (int i = 0; i < 4; i++) {
            painter.drawLine(670 + i * 80, 610, 700 + i * 80, 710);
        }
    }
}

void NivelFabrica::dibujarCaldera(QPainter& painter) {
    QPixmap caldera = ResourceManager::getInstance().getSprite("caldera_principal");
    if (!caldera.isNull()) {
        painter.drawPixmap(120, 180, 130, 130, caldera);
    }

    // INDICADOR VISUAL: Mostrar si el jugador puede reparar
    float distanciaJugadorCaldera = std::sqrt(
        std::pow(jugador->getPosX() - 185, 2) +  // Centro X de caldera
        std::pow(jugador->getPosY() - 245, 2)    // Centro Y de caldera
        );

    if (distanciaJugadorCaldera < 100.0f && presionVapor > 0.1f) {
        // Dibujar indicador de que se puede reparar
        painter.setPen(QPen(QColor(255, 255, 0), 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(118, 178, 134, 134);

        // Texto de ayuda
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.setPen(QColor(255, 255, 100));
        painter.drawText(90, 325, "Click para reparar");

        // Mostrar presión actual
        QString etapaTexto = QString("Presión: %.0f%% → 0%%").arg(presionVapor);
        painter.drawText(85, 340, etapaTexto);
    }
}

void NivelFabrica::dibujarBarraPresion(QPainter& painter) {
    // DELEGA a HUDRenderer.cpp
    HUDRenderer hud;
    hud.dibujarBarraPresion(painter, presionVapor, tiempoCalderaSobrecalentada);
}

void NivelFabrica::dibujarHUD(QPainter& painter) {
    // DELEGA a HUDRenderer.cpp
    HUDRenderer hud;

    // Dibujar corazones de vidas arriba
    hud.dibujarCorazonesVidas(painter, vidasRestantes, 400);

    hud.dibujarEstadisticas(painter, metrosProducidos, metasObjetivo,
                            vidasRestantes, presionVapor,
                            tiempoLimite - tiempoTranscurrido);

    if (carreteSeleccionado) {
        QColor colorCarrete;
        if (carreteSeleccionado->getColor() == "rojo") colorCarrete = Qt::red;
        else if (carreteSeleccionado->getColor() == "azul") colorCarrete = Qt::blue;
        else if (carreteSeleccionado->getColor() == "verde") colorCarrete = Qt::green;

        hud.dibujarIndicadorCarrete(painter, colorCarrete);
    }

    hud.dibujarInstrucciones(painter);
    hud.dibujarBotonPausa(painter, botonPausaRect);
}

void NivelFabrica::dibujarGameOver(QPainter& painter) {
    // DELEGA a GameOverScreen.cpp
    GameOverScreen gameOver;
    QString razon = calderaExplotada ? "¡LA CALDERA EXPLOTÓ!" : "SIN VIDAS";
    gameOver.dibujar(painter, width(), height(), razon,
                     metrosProducidos, metasObjetivo, tiempoTranscurrido);
}

// EVENTOS
void NivelFabrica::keyPressEvent(QKeyEvent *event) {
    if (mostrarGameOver) {
        Nivel::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_W: teclaW = true; break;
    case Qt::Key_A: teclaA = true; break;
    case Qt::Key_S: teclaS = true; break;
    case Qt::Key_D: teclaD = true; break;
    }
    Nivel::keyPressEvent(event);
}

void NivelFabrica::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_W: teclaW = false; break;
    case Qt::Key_A: teclaA = false; break;
    case Qt::Key_S: teclaS = false; break;
    case Qt::Key_D: teclaD = false; break;
    }
    Nivel::keyReleaseEvent(event);
}

void NivelFabrica::mousePressEvent(QMouseEvent *event) {
    if (mostrarGameOver) {
        // DELEGA a GameOverScreen.cpp
        GameOverScreen gameOver;
        auto boton = gameOver.detectarClic(event->pos());

        if (boton == GameOverScreen::VOLVER_MENU) {
            close();
            emit nivelCompletado(false);
        } else if (boton == GameOverScreen::SALIR) {
            QApplication::quit();
        } else if (boton == GameOverScreen::REINTENTAR) {
            mostrarGameOver = false;
            inicializar();
        }
        return;
    }

    if (botonPausaRect.contains(event->pos())) {
        pausado ? reanudar() : pausar();
        return;
    }

    float mouseX = event->pos().x();
    float mouseY = event->pos().y();

    // NUEVO: Reparar caldera
    // Caldera en posición (120, 180) con tamaño 130x130
    QRect calderaRect(120, 180, 130, 130);

    if (calderaRect.contains(event->pos())) {
        // Verificar que el jugador esté cerca de la caldera
        float distanciaJugadorCaldera = std::sqrt(
            std::pow(jugador->getPosX() - 185, 2) +
            std::pow(jugador->getPosY() - 245, 2)
            );

        if (distanciaJugadorCaldera < 100.0f) {
            // El jugador está cerca, puede reparar
            repararCaldera();
            return;  // No procesar otros clicks
        } else {
            qDebug() << "Jugador muy lejos de la caldera para repararla";
            return;
        }
    }

    // RECOGER CARRETE
    if (!carreteSeleccionado) {
        for (auto carrete : carretes) {
            if (carrete->estaCayendo()) continue;
            if (carrete->estaDestruido()) continue;

            float dx = carrete->getPosX() - mouseX;
            float dy = carrete->getPosY() - mouseY;
            float dist = std::sqrt(dx * dx + dy * dy);

            float jugadorDx = carrete->getPosX() - jugador->getPosX();
            float jugadorDy = carrete->getPosY() - jugador->getPosY();
            float jugadorDist = std::sqrt(jugadorDx * jugadorDx + jugadorDy * jugadorDy);

            if (dist < 50.0f && jugadorDist < 100.0f) {
                // SELECCIONAR Y ELIMINAR INMEDIATAMENTE DE LA BANDA
                carreteSeleccionado = carrete;
                carretes.remove(carrete);  // Eliminar de la lista para que no se dibuje
                qDebug() << "Carrete seleccionado y eliminado de la banda - Color:" << carrete->getColor();
                break;
            }
        }
    }
    // ASIGNAR CARRETE A TELAR
    else {
        bool asignado = false;
        for (auto telar : telares) {
            // Usar área rectangular del telar (120x120) para mejor detección
            float telarX = telar->getPosX();
            float telarY = telar->getPosY();
            float telarTamaño = 120.0f;

            // Verificar si el click está dentro del rectángulo del telar
            bool dentroX = (mouseX >= telarX && mouseX <= telarX + telarTamaño);
            bool dentroY = (mouseY >= telarY && mouseY <= telarY + telarTamaño);

            if (dentroX && dentroY) {
                qDebug() << "Click detectado en telar en (" << telarX << "," << telarY << ")";
                asignarCarreteATelar(carreteSeleccionado, telar);
                asignado = true;
                break;  // Salir después de asignar
            }
        }

        if (!asignado) {
            qDebug() << "Click fuera de todos los telares. Posición:" << mouseX << "," << mouseY;
        }
    }

    Nivel::mousePressEvent(event);
}
