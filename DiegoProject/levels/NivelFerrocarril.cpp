// levels/NivelFerrocarril.cpp
// Implementacion completa del Nivel 2: Red Ferroviaria

#include "NivelFerrocarril.h"
#include "../entities/Jugador.h"
#include "../entities/MontanaCarbon.h"
#include "../entities/VagonTren.h"
#include "../entities/Carbon.h"
#include "../agents/Capataz.h"
#include "../ui/HUDRenderer.h"
#include "../ui/GameOverScreen.h"
#include "../utils/ResourceManager.h"
#include <QApplication>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>
#include <cmath>
#include <random>

NivelFerrocarril::NivelFerrocarril(Jugador* jugador, QWidget *parent)
    : Nivel(jugador, parent),
    montanaJugador(nullptr),
    montanaCapataz(nullptr),
    capataz(nullptr),
    hudRenderer(nullptr),
    trenesCompletados(0),
    trenesObjetivo(TRENES_OBJETIVO),
    trenesFallidos(0),
    velocidadTrenBase(VELOCIDAD_TREN_INICIAL),
    velocidadTrenActual(VELOCIDAD_TREN_INICIAL),
    tiempoEntreVagones(TIEMPO_ENTRE_VAGONES_INICIAL),
    tiempoDesdeUltimoVagon(0.0f),
    frameFondoActual(0),
    tiempoAnimacionFondo(0.0f),
    capatazActivo(false),
    tiempoAutoPalearCapataz(0.0f),
    vagonesAyudadosCapataz(0),
    esperandoLanzarCarbonesJugador(false),
    esperandoLanzarCarbonesCapataz(false),
    tiempoEsperaLanzamiento(0.0f),
    origenXGuardado(0.0f),
    origenYGuardado(0.0f),
    esLanzamientoJugador(true) {

    setWindowTitle("Nivel 2: La Red Ferroviaria");
    tiempoLimite = 180.0f;
    botonPausaRect = QRect(900, 10, 100, 40);
    mostrarGameOver = false;
    razonGameOver = "";

    qDebug() << "NivelFerrocarril creado";
}

NivelFerrocarril::~NivelFerrocarril() {
    limpiarNivel();
    qDebug() << "NivelFerrocarril destruido";
}

void NivelFerrocarril::limpiarNivel() {
    // Limpiar entidades
    if (montanaJugador) {
        delete montanaJugador;
        montanaJugador = nullptr;
    }
    if (montanaCapataz) {
        delete montanaCapataz;
        montanaCapataz = nullptr;
    }
    if (capataz) {
        delete capataz;
        capataz = nullptr;
    }
    if (hudRenderer) {
        delete hudRenderer;
        hudRenderer = nullptr;
    }
    for (auto vagon : vagones) {
        delete vagon;
    }
    vagones.clear();
    for (auto carbon : carbones) {
        delete carbon;
    }
    carbones.clear();

    // Resetear TODAS las variables de estado
    trenesCompletados = 0;
    trenesFallidos = 0;
    velocidadTrenActual = VELOCIDAD_TREN_INICIAL;
    tiempoEntreVagones = TIEMPO_ENTRE_VAGONES_INICIAL;
    tiempoDesdeUltimoVagon = 0.0f;
    frameFondoActual = 0;
    tiempoAnimacionFondo = 0.0f;
    capatazActivo = false;
    tiempoAutoPalearCapataz = 0.0f;
    vagonesAyudadosCapataz = 0;  // Para el capataz inteligente

    // Variables de delay de carbones
    esperandoLanzarCarbonesJugador = false;
    esperandoLanzarCarbonesCapataz = false;
    tiempoEsperaLanzamiento = 0.0f;
    origenXGuardado = 0.0f;
    origenYGuardado = 0.0f;

    qDebug() << "Nivel ferrocarril limpiado completamente";
}

void NivelFerrocarril::inicializar() {
    qDebug() << "=== INICIANDO NIVEL FERROCARRIL ===";

    // Resetear variables de la clase base
    completado = false;
    pausado = false;
    mostrarGameOver = false;
    razonGameOver = "";
    tiempoTranscurrido = 0.0f;

    // Resetear variables del nivel 2
    trenesCompletados = 0;
    trenesFallidos = 0;
    velocidadTrenActual = VELOCIDAD_TREN_INICIAL;
    tiempoDesdeUltimoVagon = 0.0f;
    capatazActivo = false;
    tiempoAutoPalearCapataz = 0.0f;
    vagonesAyudadosCapataz = 0;
    frameFondoActual = 0;
    tiempoAnimacionFondo = 0.0f;

    // Variables de delay
    esperandoLanzarCarbonesJugador = false;
    esperandoLanzarCarbonesCapataz = false;
    tiempoEsperaLanzamiento = 0.0f;
    origenXGuardado = 0.0f;
    origenYGuardado = 0.0f;

    // Limpiar listas
    for (auto vagon : vagones) delete vagon;
    vagones.clear();
    for (auto carbon : carbones) delete carbon;
    carbones.clear();

    // Cargar recursos
    ResourceManager::getInstance().cargarRecursos();

    // Cargar sprites de fondo animado
    fondosAnimados.clear();
    for (int i = 1; i <= 5; i++) {
        QString key = QString("fondo_frame%1").arg(i);
        fondosAnimados.append(ResourceManager::getInstance().getSprite(key));
    }

    // Cargar sprites de personajes
    spriteJugadorPalear = ResourceManager::getInstance().getSprite("jugadornivel2");
    spriteCapatazPalear = ResourceManager::getInstance().getSprite("capataznivel2");
    spriteTren = ResourceManager::getInstance().getSprite("tren");

    // Posición Jugador
    jugador->setPosicion(500, 300);
    jugador->cargarSpritesPalear(spriteJugadorPalear);

    // Resetear estado del jugador
    jugador->reiniciarCansancio();
    jugador->reiniciarFramePalear();

    // Montañas de carbón
    crearMontanasCarbon();

    // Capataz - Posición directa
    capataz = new Capataz(this);
    capataz->setPosicion(5, 300);
    capataz->cargarSpritesPalearN2(spriteCapatazPalear);
    capataz->activarAyudaN2(false);

    // Crear HUD renderer
    hudRenderer = new HUDRenderer();

    // Resetear y reiniciar el timer
    if (timerJuego) {
        if (timerJuego->isActive()) {
            timerJuego->stop();
        }
        timerJuego->start(16); // 60 FPS
        relojNivel.restart();
        qDebug() << "Timer del juego reiniciado";
    }

    qDebug() << "Nivel Ferrocarril inicializado correctamente";
}

void NivelFerrocarril::crearMontanasCarbon() {
    montanaJugador = new MontanaCarbon(545, 830, this);
    QPixmap spriteCarbon = ResourceManager::getInstance().getSprite("carbon");
    montanaJugador->cargarSprites(spriteCarbon);

    montanaCapataz = new MontanaCarbon(250, 830, this);
    montanaCapataz->cargarSprites(spriteCarbon);

    qDebug() << "Montanas de carbon creadas en (550,700) y (250,700)";
}
void NivelFerrocarril::crearVagonTren() {
    VagonTren* vagon = new VagonTren(110, velocidadTrenActual, this);
    vagon->cargarSprite(spriteTren);

    connect(vagon, &VagonTren::vagonLleno, this, [this]() {
        trenesCompletados++;
        qDebug() << "¡VAGÓN LLENO! Contador:" << trenesCompletados << "/" << trenesObjetivo;
        verificarCondicionesFinales();
    });

    connect(vagon, &VagonTren::trenSalio, this, [this](bool completado) {
        if (!completado) {  // Solo vagones fallidos
            trenesFallidos++;
            jugador->perderVida();
            qDebug() << "Vagón fallido";
            verificarCondicionesFinales();
        }
    });

    vagones.push_back(vagon);
    qDebug() << "Vagón creado";
}

void NivelFerrocarril::actualizar() {
    if (pausado || completado) return;

    float deltaTime = 1.0f / 60.0f;
    tiempoTranscurrido += deltaTime;


    // Delay para carbones del JUGADOR
    if (esperandoLanzarCarbonesJugador) {
        tiempoEsperaLanzamiento += deltaTime;

        // Esperar 0.6 segundos (frame 4 de la animación)
        if (tiempoEsperaLanzamiento >= DELAY_LANZAMIENTO) {
            // Lanzar 3 carbones desde la posición guardada
            for (int i = 0; i < 3; i++) {
                float offsetX = (i - 1) * 20.0f;  // -20, 0, +20
                lanzarCarbon(origenXGuardado + offsetX, origenYGuardado, true);
            }

            qDebug() << "🪨 Carbones del jugador lanzados en frame 4";
            esperandoLanzarCarbonesJugador = false;
            tiempoEsperaLanzamiento = 0.0f;
        }
    }

    // Delay para carbones del CAPATAZ
    if (esperandoLanzarCarbonesCapataz) {
        tiempoEsperaLanzamiento += deltaTime;

        if (tiempoEsperaLanzamiento >= DELAY_LANZAMIENTO) {
            // Lanzar 3 carbones desde la posición guardada
            for (int i = 0; i < 3; i++) {
                float offsetX = (i - 1) * 20.0f;
                lanzarCarbon(origenXGuardado + offsetX, origenYGuardado, false);
            }

            qDebug() << "🪨 Carbones del capataz lanzados en frame 4";
            esperandoLanzarCarbonesCapataz = false;
            tiempoEsperaLanzamiento = 0.0f;
        }
    }


    // Actualizar componentes
    actualizarJugador(deltaTime);
    actualizarCapataz(deltaTime);
    actualizarVagones(deltaTime);
    actualizarCarbones(deltaTime);
    actualizarFondoAnimado(deltaTime);
    actualizarDificultad(deltaTime);
    verificarColisionCarbonVagon();
    verificarCondicionesFinales();

    update();
}

void NivelFerrocarril::actualizarJugador(float deltaTime) {
    // Actualizar cansancio
    jugador->actualizarCansancio(deltaTime);

    // Actualizar animacion de palear
    jugador->actualizarPalear(deltaTime);

    // Si el cansancio llega al 100%, perder vida y descansar
    if (jugador->getCansancio() >= 100.0f) {
        jugador->perderVida();
        jugador->descansar();
        qDebug() << "Jugador agotado, vida perdida";
    }
}

void NivelFerrocarril::actualizarCapataz(float deltaTime) {
    // Activar capataz si el jugador tiene 2 vidas o menos
    if (jugador->getVidas() <= 2 && !capatazActivo) {
        capatazActivo = true;
        capataz->activarAyudaN2(true);
        qDebug() << "Capataz activado para ayudar";
    }

    if (!capatazActivo) return;

    // Actualizar animación de palear del capataz
    capataz->actualizarPalearN2(deltaTime);

    // Solo ayudar con 2 vagones máximo
    if (vagonesAyudadosCapataz >= MAX_VAGONES_AYUDA_CAPATAZ) {
        return;  // Ya ayudó suficiente, no hace nada más
    }

    // Palear cada cierto tiempo si hay vagón visible
    tiempoAutoPalearCapataz += deltaTime;
    if (tiempoAutoPalearCapataz >= TIEMPO_AUTO_PALEAR_CAPATAZ) {
        tiempoAutoPalearCapataz = 0.0f;

        // Buscar si hay algún vagón en pantalla (mitad de pantalla aprox)
        bool hayVagonVisible = false;
        for (VagonTren* vagon : vagones) {
            float posX = vagon->getPosX();

            // Esto cubre aproximadamente la mitad de la pantalla
            if (posX >= 300.0f && posX <= 800.0f) {
                // Solo si el vagón NO está completo
                if (!vagon->estaCompleto()) {
                    hayVagonVisible = true;
                    qDebug() << "Capataz ve vagón en X:" << posX;
                    break;
                }
            }
        }

        // Si hay vagón visible, el capataz NO está paleando, y hay carbón
        if (hayVagonVisible && !capataz->estaPaleando() && !montanaCapataz->estaVacia()) {
            onClickMontanaCapataz();
            vagonesAyudadosCapataz++;
            qDebug() << "Capataz palea! Ayuda" << vagonesAyudadosCapataz << "/"
                     << MAX_VAGONES_AYUDA_CAPATAZ;

            if (vagonesAyudadosCapataz >= MAX_VAGONES_AYUDA_CAPATAZ) {
                qDebug() << "Capataz terminó de ayudar";
            }
        }
    }
}

void NivelFerrocarril::actualizarVagones(float deltaTime) {
    // Actualizar vagones existentes
    for (auto it = vagones.begin(); it != vagones.end(); ) {
        (*it)->actualizar(deltaTime);

        // Eliminar vagones que ya terminaron
        if ((*it)->haTerminado()) {
            delete *it;
            it = vagones.erase(it);
        } else {
            ++it;
        }
    }

    // Crear nuevo vagon si es tiempo Y no hay vagones muy cerca del inicio
    tiempoDesdeUltimoVagon += deltaTime;
    if (tiempoDesdeUltimoVagon >= tiempoEntreVagones) {
        // Verificar que el ultimo vagon este suficientemente lejos
        bool puedeCrear = true;
        if (!vagones.empty()) {
            VagonTren* ultimoVagon = vagones.back();
            if (ultimoVagon->getPosX() < 200.0f) {
                puedeCrear = false; // Esperar a que se aleje mas
            }
        }

        if (puedeCrear) {
            crearVagonTren();
            tiempoDesdeUltimoVagon = 0.0f;
        }
    }
}

void NivelFerrocarril::actualizarCarbones(float deltaTime) {
    for (auto it = carbones.begin(); it != carbones.end(); ) {
        Carbon* carbon = *it;

        // Aplicar fisica
        carbon->aplicarGravedad(GRAVEDAD, deltaTime);
        carbon->actualizar(deltaTime);

        // Verificar colision con el suelo
        if (carbon->colisionaConSuelo(680.0f)) {
            // Determinar tipo de colision segun tipo del carbon
            if (carbon->getTipoColision() == Carbon::ELASTICA) {
                carbon->rebotarEnSuelo(680.0f, 0.8f);
            } else if (carbon->getTipoColision() == Carbon::INELASTICA) {
                carbon->rebotarEnSuelo(680.0f, 0.4f);
            } else {
                carbon->rebotarEnSuelo(680.0f, 0.0f);
            }
        }

        // Verificar colisiones entre carbones
        for (auto other : carbones) {
            if (carbon != other && carbon->colisionaCon(other)) {
                if (carbon->getTipoColision() == Carbon::ELASTICA) {
                    carbon->resolverColisionElastica(other);
                } else if (carbon->getTipoColision() == Carbon::INELASTICA) {
                    carbon->resolverColisionInelastica(other, 0.5f);
                } else {
                    carbon->resolverColisionPerfectamenteInelastica(other);
                }
            }
        }

        // Eliminar carbones inactivos
        if (!carbon->estaActivo()) {
            delete carbon;
            it = carbones.erase(it);
        } else {
            ++it;
        }
    }
}

void NivelFerrocarril::actualizarFondoAnimado(float deltaTime) {
    tiempoAnimacionFondo += deltaTime;

    // Cambiar frame cada 0.2 segundos
    if (tiempoAnimacionFondo >= 1.0f) {
        frameFondoActual = (frameFondoActual + 1) % fondosAnimados.size();
        tiempoAnimacionFondo = 0.0f;
    }
}

void NivelFerrocarril::actualizarDificultad(float deltaTime) {
    // Aumentar velocidad cada 30 segundos
    static float tiempoAcumulado = 0.0f;
    tiempoAcumulado += deltaTime;

    if (tiempoAcumulado >= 30.0f) {
        velocidadTrenActual += INCREMENTO_VELOCIDAD;
        tiempoAcumulado = 0.0f;
        qDebug() << "Velocidad aumentada a:" << velocidadTrenActual;
    }
}

void NivelFerrocarril::verificarColisionCarbonVagon() {
    // Verificar TODOS los carbones contra TODOS los vagones
    for (auto it = carbones.begin(); it != carbones.end(); ) {
        Carbon* carbon = *it;
        bool carbonRecibido = false;

        if (carbon->estaActivo()) {
            for (auto vagon : vagones) {
                if (vagon->puedeRecibirCarbon(carbon->getPosX(), carbon->getPosY())) {
                    vagon->recibirCarbon();
                    carbon->desactivar();
                    carbonRecibido = true;
                    qDebug() << "*** CARBON RECIBIDO POR VAGON! Total:" << vagon->getCarbonRecibido() << "/" << vagon->getCarbonRequerido() << " ***";
                    break;  // Un carbon solo puede ir a un vagon
                }
            }
        }

        // Si el carbon fue recibido, eliminarlo inmediatamente
        if (carbonRecibido) {
            delete carbon;
            it = carbones.erase(it);
        } else {
            ++it;
        }
    }
}

void NivelFerrocarril::verificarCondicionesFinales() {
    // Victoria
    if (trenesCompletados >= trenesObjetivo) {
        completado = true;
        emit nivelCompletado(true);
        qDebug() << "¡Nivel completado!";
    }

    // Derrota por vidas - Mostrar Game Over
    if (jugador->getVidas() <= 0 && !mostrarGameOver) {
        mostrarPantallaGameOver("SIN VIDAS");
    }

    // Derrota por tiempo - Mostrar Game Over
    if (tiempoTranscurrido >= tiempoLimite && !mostrarGameOver) {
        mostrarPantallaGameOver("TIEMPO AGOTADO");
    }
}

void NivelFerrocarril::mostrarPantallaGameOver(const QString& razon) {
    mostrarGameOver = true;
    razonGameOver = razon;
    pausado = true;
    qDebug() << "Game Over:" << razon;
}

void NivelFerrocarril::dibujarGameOver(QPainter& painter) {
    GameOverScreen gameOver;
    gameOver.dibujar(painter, width(), height(), razonGameOver,
                     trenesCompletados, trenesObjetivo, tiempoTranscurrido);
}

void NivelFerrocarril::paintEvent(QPaintEvent *event) {
    Nivel::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Dibujar componentes en orden
    dibujarFondo(painter);
    dibujarVagones(painter);
    dibujarMontanasCarbon(painter);
    dibujarCarbones(painter);
    dibujarJugador(painter);

    if (capatazActivo) {
        dibujarCapataz(painter);
    }

    dibujarHUD(painter);

    //Dibujar botón de pausa
    if (hudRenderer) {
        hudRenderer->dibujarBotonPausa(painter, botonPausaRect);
    }

    if (mostrarGameOver) {
        dibujarGameOver(painter);
    }
}

void NivelFerrocarril::dibujarFondo(QPainter& painter) {
    if (fondosAnimados.isEmpty() || frameFondoActual >= fondosAnimados.size()) {
        // Fondo de respaldo
        painter.fillRect(rect(), QColor(135, 100, 80));
        return;
    }

    // Dibujar fondo animado escalado a la ventana
    QPixmap fondoActual = fondosAnimados[frameFondoActual];
    painter.drawPixmap(rect(), fondoActual);
}

void NivelFerrocarril::dibujarMontanasCarbon(QPainter& painter) {
    if (montanaJugador) {
        montanaJugador->dibujar(painter);
    }

    if (montanaCapataz) {
        montanaCapataz->dibujar(painter);
    }
}

void NivelFerrocarril::dibujarVagones(QPainter& painter) {
    for (auto vagon : vagones) {
        vagon->dibujar(painter);
    }
}

void NivelFerrocarril::dibujarCarbones(QPainter& painter) {
    for (auto carbon : carbones) {
        carbon->dibujar(painter);
    }
}

void NivelFerrocarril::dibujarJugador(QPainter& painter) {
    if (spriteJugadorPalear.isNull() || jugador->getFramePalear() >= 5) {
        // Fallback
        painter.setBrush(QColor(100, 150, 200));
        painter.drawEllipse(QPointF(jugador->getPosX(), jugador->getPosY()), 30, 40);
        return;
    }

    // Calcular frame actual
    int frameWidth = spriteJugadorPalear.width() / 5;
    int frameHeight = spriteJugadorPalear.height();

    // Usar QRect para source y target
    QRect sourceRect(jugador->getFramePalear() * frameWidth, 0, frameWidth, frameHeight);

    // Tamaño fijo para el sprite de palear
    int anchoSprite = 300;
    int altoSprite = 750;

    // Rectángulo de destino (donde se dibuja en pantalla)
    QRect targetRect(jugador->getPosX(), jugador->getPosY(), anchoSprite, altoSprite);
    painter.drawPixmap(targetRect, spriteJugadorPalear, sourceRect);
}

void NivelFerrocarril::dibujarCapataz(QPainter& painter) {
    if (spriteCapatazPalear.isNull() || capataz->getFramePalear() >= 4) {
        // Fallback
        painter.setBrush(QColor(150, 100, 50));
        painter.drawEllipse(QPointF(capataz->getPosX(), capataz->getPosY()), 30, 40);
        return;
    }

    // Calcular frame actual del sprite sheet
    int frameWidth = spriteCapatazPalear.width() / 4;
    int frameHeight = spriteCapatazPalear.height();
    QRect sourceRect(capataz->getFramePalear() * frameWidth, 0, frameWidth, frameHeight);


    const int ANCHO_SPRITE = 300;
    const int ALTO_SPRITE = 750;

    // Obtener posición del capataz
    float posX = capataz->getPosX();
    float posY = capataz->getPosY();

    QRect targetRect(posX, posY, ANCHO_SPRITE, ALTO_SPRITE);
    painter.drawPixmap(targetRect, spriteCapatazPalear, sourceRect);
}

void NivelFerrocarril::dibujarHUD(QPainter& painter) {
    if (!hudRenderer) return;

    float tiempoRestante = tiempoLimite - tiempoTranscurrido;
    hudRenderer->dibujarEstadisticasN2(painter, jugador->getVidas(),
                                       jugador->getCansancio(),
                                       trenesCompletados, trenesObjetivo,
                                       tiempoRestante);

    // Instrucciones
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    QString instrucciones = "Click en la montana de carbon para palear | ESC: Pausa";
    painter.drawText(10, height() - 20, instrucciones);

    // Advertencia si esta descansando
    if (jugador->estaDescansando()) {
        painter.setPen(QColor(255, 200, 0));
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(width() / 2 - 100, 200, "DESCANSANDO...");
    }

    // Notificacion de capataz
    if (capatazActivo) {
        painter.setPen(QColor(0, 255, 0));
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(width() / 2 - 150, 230, "Capataz ayudando!");
    }

    // Overlay de pausa
    if (pausado) {
        painter.fillRect(rect(), QColor(0, 0, 0, 150));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 48, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "PAUSA\n\nESC para continuar");
    }
}

void NivelFerrocarril::mousePressEvent(QMouseEvent *event) {
    // Manejar clicks en Game Over PRIMERO
    if (mostrarGameOver) {
        GameOverScreen gameOver;

        // Dibujar primero para actualizar posiciones de botones
        QPainter tempPainter(this);
        gameOver.dibujar(tempPainter, width(), height(), razonGameOver,
                         trenesCompletados, trenesObjetivo, tiempoTranscurrido);

        // Detectar el click
        auto boton = gameOver.detectarClic(event->pos());

        if (boton == GameOverScreen::VOLVER_MENU) {
            qDebug() << "Botón VOLVER AL MENÚ presionado";
            close();
            emit nivelCompletado(false);
        } else if (boton == GameOverScreen::SALIR) {
            qDebug() << "Botón SALIR presionado";
            QApplication::quit();
        } else if (boton == GameOverScreen::REINTENTAR) {
            qDebug() << "Botón REINTENTAR presionado";
            mostrarGameOver = false;
            inicializar();
        }
        return;
    }

    // Detectar click en botón de pausa
    if (botonPausaRect.contains(event->pos())) {
        if (pausado) {
            reanudar();
        } else {
            pausar();
        }
        return;
    }

    if (pausado || completado) return;

    float clickX = event->pos().x();
    float clickY = event->pos().y();

    // Verificar click en montana del jugador
    if (montanaJugador && montanaJugador->contienePunto(clickX, clickY)) {
        onClickMontanaJugador();
    }

    // Verificar click en montana del capataz
    if (montanaCapataz && capatazActivo && montanaCapataz->contienePunto(clickX, clickY)) {
        onClickMontanaCapataz();
    }

    Nivel::mousePressEvent(event);
}

void NivelFerrocarril::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        if (pausado) {
            reanudar();
        } else {
            pausar();
        }
        update();
    }

    Nivel::keyPressEvent(event);
}

void NivelFerrocarril::onClickMontanaJugador() {
    qDebug() << "=== CLICK EN MONTANA JUGADOR ===";

    // Validar que el jugador NO esté muy cansado
    if (jugador->getCansancio() >= 100.0f) {
        qDebug() << "Jugador cansado";
        return;
    }

    // Validar que NO esté paleando
    if (jugador->getFramePalear() > 0) {
        qDebug() << "Ya paleando";
        return;
    }

    // Reducir la montaña
    montanaJugador->disminuir();
    esperandoLanzarCarbonesJugador = true;
    tiempoEsperaLanzamiento = 0.0f;

    // Guardar posición actual del jugador
    origenXGuardado = jugador->getPosX();
    origenYGuardado = jugador->getPosY() + 280;  // Desde las manos
    esLanzamientoJugador = true;

    qDebug() << "Esperando al último frame para lanzar carbones...";

    // Aumentar cansancio y animar
    jugador->aumentarCansancio(3.0f);
    jugador->iniciarPalear();
}

void NivelFerrocarril::onClickMontanaCapataz() {
    qDebug() << "=== CLICK EN MONTANA CAPATAZ ===";

    // Reducir la montaña
    montanaCapataz->disminuir();

    // Guardar posición para lanzar después
    esperandoLanzarCarbonesCapataz = true;
    tiempoEsperaLanzamiento = 0.0f;
    origenXGuardado = capataz->getPosX();
    origenYGuardado = capataz->getPosY() + 20;
    esLanzamientoJugador = false;

    qDebug() << "Esperando al último frame para lanzar carbones...";

    if (!capataz->estaPaleando()) {
        capataz->iniciarPalearN2();
    }
}

void NivelFerrocarril::lanzarCarbon(float origenX, float origenY, bool esJugador) {
    // Generar tipo de colisión aleatorio
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 2);

    Carbon::TipoColision tipo;
    int tipoAleatorio = dis(gen);
    if (tipoAleatorio == 0) tipo = Carbon::ELASTICA;
    else if (tipoAleatorio == 1) tipo = Carbon::INELASTICA;
    else tipo = Carbon::PERFECTAMENTE_INELASTICA;


    float velocidadX = -300.0f;  // Hacia la IZQUIERDA
    float velocidadY = -100.0f;  // Hacia ARRIBA

    // Crear carbón con las velocidades ajustadas
    Carbon* carbon = new Carbon(origenX, origenY, velocidadX, velocidadY, tipo, this);
    carbones.push_back(carbon);

    qDebug() << "Carbón lanzado desde:" << (int)origenX << "," << (int)origenY
             << "Vel:(" << (int)velocidadX << "," << (int)velocidadY << ")"
             << "Tipo:" << (esJugador ? "JUGADOR" : "CAPATAZ");
}
