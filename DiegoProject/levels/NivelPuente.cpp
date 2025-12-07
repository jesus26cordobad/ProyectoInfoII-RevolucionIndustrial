// levels/NivelPuente.cpp
#include "NivelPuente.h"
#include "../entities/Jugador.h"
#include "../entities/Viga.h"
#include "../entities/Moneda.h"
#include "../agents/Ladron.h"
#include "../ui/HUDRenderer.h"
#include "../ui/GameOverScreen.h"
#include "../utils/ResourceManager.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>
#include <random>

NivelPuente::NivelPuente(Jugador* jugador, QWidget *parent)
    : Nivel(jugador, parent),
    ladron(nullptr),
    hudRenderer(nullptr),
    monedasRecolectadas(0),
    monedasObjetivo(MONEDAS_OBJETIVO),
    tiempoDesdeUltimaMoneda(0.0f),
    tiempoTranscurridoTotal(0.0f),
    teclaA(false), teclaD(false), teclaW(false),
    mostrarGameOver(false),
    razonGameOver("") {

    setWindowTitle("Nivel 3: El Puente");
    tiempoLimite = 300.0f;
    botonPausaRect = QRect(900, 10, 100, 40);

    qDebug() << "NivelPuente creado";
}

NivelPuente::~NivelPuente() {
    for (auto viga : vigas) delete viga;
    for (auto moneda : monedas) delete moneda;
    delete ladron;
    delete hudRenderer;
}

void NivelPuente::inicializar() {
    qDebug() << "=== INICIANDO NIVEL PUENTE ===";

    // Resetear variables
    completado = false;
    pausado = false;
    tiempoTranscurrido = 0.0f;
    tiempoTranscurridoTotal = 0.0f;
    mostrarGameOver = false;
    razonGameOver = "";

    monedasRecolectadas = 0;
    tiempoDesdeUltimaMoneda = 0.0f;
    teclaA = teclaD = teclaW = false;

    // Limpiar entidades
    for (auto viga : vigas) delete viga;
    vigas.clear();
    for (auto moneda : monedas) delete moneda;
    monedas.clear();

    // Cargar recursos
    ResourceManager::getInstance().cargarRecursos();

    // Cargar sprites
    fondoNivel3 = ResourceManager::getInstance().getSprite("fondolevel3");
    spriteMoneda = ResourceManager::getInstance().getSprite("moneda");

    spriteJugadorCorrDer = ResourceManager::getInstance().getSprite("jcorriendo_derecha");
    spriteJugadorCorrIzq = ResourceManager::getInstance().getSprite("jcorriendo_izquierda");
    spriteJugadorSaltoDer = ResourceManager::getInstance().getSprite("persaltoderecha");
    spriteJugadorSaltoIzq = ResourceManager::getInstance().getSprite("persaltoizquierda");

    spriteLadronCorrDer = ResourceManager::getInstance().getSprite("lcorriendo_derecha");
    spriteLadronCorrIzq = ResourceManager::getInstance().getSprite("lcorriendo_izquierda");
    spriteLadronSaltoDer = ResourceManager::getInstance().getSprite("ladronsaltoderecha");
    spriteLadronSaltoIzq = ResourceManager::getInstance().getSprite("ladronsaltoizquierda");

    // Configurar jugador
    jugador->setPosicion(350, 400);
    jugador->reiniciarCansancio();
    jugador->reiniciarVidas();
    jugador->reiniciarFramePalear();
    jugador->cargarSpritesNivel3(spriteJugadorCorrDer, spriteJugadorCorrIzq,
                                 spriteJugadorSaltoDer, spriteJugadorSaltoIzq);
    jugador->setEnAire(false);

    // Crear vigas
    crearVigas();

    // Crear ladrón
    if (ladron) delete ladron;
    ladron = new Ladron(this);
    ladron->setPosicion(700, 100);
    ladron->cargarSprites(spriteLadronCorrDer, spriteLadronCorrIzq,
                          spriteLadronSaltoDer, spriteLadronSaltoIzq);

    // Crear HUD
    if (hudRenderer) delete hudRenderer;
    hudRenderer = new HUDRenderer();

    // Generar 3 monedas iniciales
    for (int i = 0; i < 3; i++) {
        generarMoneda();
    }

    // Iniciar timer
    if (timerJuego) {
        if (timerJuego->isActive()) timerJuego->stop();
        timerJuego->start(16);
        relojNivel.restart();
    }

    qDebug() << "Nivel Puente iniciado correctamente";
}

void NivelPuente::crearVigas() {
    // Viga izquierda (viga4) - arriba
    vigas.push_back(new Viga(550, 200, Viga::IZQUIERDA, this));

    // Viga centro (viga1) - abajo
    vigas.push_back(new Viga(320, 500, Viga::CENTRO, this));

    // Viga derecha (viga3) - media
    vigas.push_back(new Viga(-30, 300, Viga::DERECHA, this));

    // ✅ SUELO: Plataforma completa en el fondo
    vigas.push_back(new Viga(-100, 700, Viga::SUELO, this));

    qDebug() << "Creadas" << vigas.size() << "vigas (incluye suelo)";
}

void NivelPuente::actualizar() {
    if (pausado || completado || mostrarGameOver) return;

    float deltaTime = relojNivel.restart() / 1000.0f;

    // Si deltaTime es muy pequeño o cero, usar valor fijo
    if (deltaTime <= 0.0f || deltaTime > 0.1f) {
        deltaTime = 0.016f;
    }

    tiempoTranscurrido += deltaTime;
    tiempoTranscurridoTotal += deltaTime;

    // Actualizar entidades
    actualizarJugador(deltaTime);
    actualizarLadron(deltaTime);
    actualizarMonedas(deltaTime);

    // Generar monedas
    tiempoDesdeUltimaMoneda += deltaTime;
    if (tiempoDesdeUltimaMoneda >= TIEMPO_GENERAR_MONEDA) {
        generarMoneda();
        tiempoDesdeUltimaMoneda = 0.0f;
    }

    // Verificar colisiones
    verificarColisionesVigas();
    verificarColisionesMonedas();
    verificarColisionLadron();

    // Aumentar dificultad
    aumentarDificultad();

    // Verificar condiciones de victoria/derrota
    verificarCondicionesFinales();

    update();
}

void NivelPuente::actualizarJugador(float deltaTime) {
    // Resetear estado de movimiento
    jugador->setMoviendose(false);

    // Movimiento lateral
    if (teclaA) {
        jugador->moverLateral(false, deltaTime);
    } else if (teclaD) {
        jugador->moverLateral(true, deltaTime);
    } else {
        // Solo resetear frame si está en el suelo
        if (!jugador->estaEnAire()) {
            jugador->detenerAnimacion();
        }
    }

    // Salto
    if (teclaW && !jugador->estaEnAire()) {
        jugador->saltar();
        teclaW = false;
    }

    verificarColisionesVigas();

    jugador->aplicarGravedad(deltaTime);

    // Límites de pantalla
    if (jugador->getPosX() < 0) {
        jugador->setPosicion(0, jugador->getPosY());
    }
    if (jugador->getPosX() > width() - 80) {
        jugador->setPosicion(width() - 80, jugador->getPosY());
    }

    // Límite inferior (caída al vacío)
    if (jugador->getPosY() > height() + 50) {
        jugador->perderVida();
        jugador->setPosicion(350, 400);
        jugador->setEnAire(false);
        jugador->setVelocidadY(0);
        jugador->detenerAnimacion();
        qDebug() << "Jugador cayó al vacío - Vidas:" << jugador->getVidas();
    }
}

void NivelPuente::actualizarLadron(float deltaTime) {
    if (!ladron) return;

    // Actualizar IA del ladrón
    ladron->actualizar(deltaTime, jugador->getPosX(), jugador->getPosY());

    // Primero verificar colisiones
    verificarColisionesLadronVigas();

    // Después aplicar gravedad
    ladron->aplicarGravedad(deltaTime);

    // Límite inferior (caída al vacío)
    if (ladron->getPosY() > height() + 50) {
        ladron->setPosicion(700, 100);
        ladron->setEnAire(false);
        ladron->setVelocidadY(0);
        qDebug() << "Ladrón cayó al vacío - reposicionado en viga superior";
    }
}

void NivelPuente::actualizarMonedas(float deltaTime) {
    for (auto moneda : monedas) {
        moneda->actualizar(deltaTime);
    }
}

void NivelPuente::generarMoneda() {
    if (vigas.empty()) {
        qDebug() << "No hay vigas para generar monedas";
        return;
    }

    // Elegir viga aleatoria
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, vigas.size() - 1);

    int vigaIndex = dis(gen);
    Viga* vigaElegida = vigas[vigaIndex];

    // Posición sobre la viga
    float vigaX = vigaElegida->getPosX();
    float vigaAncho = vigaElegida->getAncho();
    float vigaY = vigaElegida->getPosY();

    // Posición aleatoria sobre la viga
    std::uniform_real_distribution<> disX(vigaX + 50, vigaX + vigaAncho - 80);
    float monedaX = disX(gen);
    float monedaY = vigaY - 40;

    monedas.push_back(new Moneda(monedaX, monedaY, this));

    qDebug() << "Moneda generada en X:" << monedaX << "Y:" << monedaY;
}

void NivelPuente::verificarColisionesVigas() {
    bool sobreViga = false;

    float jugadorX = jugador->getPosX();
    float jugadorY = jugador->getPosY();
    float velocidadY = jugador->getVelocidadY();

    // Hitbox de los pies del jugador
    QRectF pieJugador(jugadorX + 10, jugadorY + 85, 30, 5);

    for (Viga* viga : vigas) {
        float vigaX = viga->getPosX();
        float vigaY = viga->getPosY();
        float vigaAncho = viga->getAncho();

        float centroJugadorX = jugadorX + 40; // Centro del jugador

        // El jugador debe estar sobre la viga (con margen pequeño)
        if (centroJugadorX >= vigaX - 10 && centroJugadorX <= vigaX + vigaAncho + 10) {
            // Superficie de la viga
            QRectF superficieViga(vigaX, vigaY - 10, vigaAncho, 25);

            // Verificar colisión vertical
            if (pieJugador.intersects(superficieViga)) {
                if (velocidadY >= 0 && jugadorY <= vigaY + 10) {
                    float nuevaPosY = vigaY - 80;

                    jugador->setPosicion(jugadorX, nuevaPosY);
                    jugador->setVelocidadY(0);
                    jugador->setEnAire(false);
                    sobreViga = true;
                    break;
                }
            }
        }
    }

    // Si no está sobre ninguna viga, debe caer
    if (!sobreViga) {
        jugador->setEnAire(true);
    }
}

void NivelPuente::verificarColisionesLadronVigas() {
    if (!ladron) return;

    bool sobreViga = false;
    float ladronX = ladron->getPosX();
    float ladronY = ladron->getPosY();
    float velocidadY = ladron->getVelocidadY();

    QRectF pieLadron(ladronX + 15, ladronY + 85, 30, 5);

    for (Viga* viga : vigas) {
        float vigaX = viga->getPosX();
        float vigaY = viga->getPosY();
        float vigaAncho = viga->getAncho();

        // Verificar rango horizontal (centro del ladrón)
        float centroLadronX = ladronX + 30;  // Centro ajustado (60/2)

        if (centroLadronX >= vigaX - 10 && centroLadronX <= vigaX + vigaAncho + 10) {
            QRectF superficieViga(vigaX, vigaY - 10, vigaAncho, 25);

            if (pieLadron.intersects(superficieViga)) {
                if (velocidadY >= 0 && ladronY <= vigaY + 10) {
                    float nuevaPosY = vigaY - 90;  // Ajustado al nuevo ALTO
                    ladron->setPosicion(ladronX, nuevaPosY);
                    ladron->setVelocidadY(0);
                    ladron->setEnAire(false);
                    sobreViga = true;
                    break;
                }
            }
        }
    }

    if (!sobreViga) {
        ladron->setEnAire(true);
    }
}

void NivelPuente::verificarColisionesMonedas() {
    QRectF jugadorHitbox(jugador->getPosX() + 10, jugador->getPosY() + 10, 60, 80);

    for (auto it = monedas.begin(); it != monedas.end(); ) {
        if ((*it)->estaActiva()) {
            if (jugadorHitbox.intersects((*it)->getHitbox())) {
                monedasRecolectadas++;
                qDebug() << "Moneda recolectada!" << monedasRecolectadas << "/" << monedasObjetivo;
                delete *it;
                it = monedas.erase(it);
                continue;
            }
        }
        ++it;
    }
}

void NivelPuente::verificarColisionLadron() {
    if (!ladron) return;

    QRectF jugadorHitbox(jugador->getPosX() + 15, jugador->getPosY() + 15, 50, 70);
    QRectF ladronHitbox(ladron->getPosX() + 10, ladron->getPosY() + 10, 50, 70);

    if (jugadorHitbox.intersects(ladronHitbox)) {
        jugador->perderVida();

        int monedasPerdidas = std::min(MONEDAS_PERDIDAS_POR_TOQUE, monedasRecolectadas);
        monedasRecolectadas -= monedasPerdidas;

        qDebug() << "¡Ladrón atrapó al jugador! Perdió" << monedasPerdidas << "monedas";

        float nuevaXJugador;

        // Determinar en qué viga está el jugador actualmente
        float jugadorY = jugador->getPosY();

        // Si está en viga superior (Y ~ 100)
        if (jugadorY < 250.0f) {
            nuevaXJugador = 600.0f;  // Lado derecho de viga superior
        }
        // Si está en viga media (Y ~ 200)
        else if (jugadorY < 450.0f) {
            nuevaXJugador = 100.0f;  // Lado izquierdo de viga media
        }
        // Si está en viga inferior (Y ~ 400)
        else {
            nuevaXJugador = 500.0f;  // Centro de viga inferior
        }

        // Mantener la misma altura Y, solo cambiar X
        jugador->setPosicion(nuevaXJugador, jugadorY);
        jugador->setEnAire(false);
        jugador->setVelocidadY(0);

        float nuevaXLadron, nuevaYLadron;

        if (jugadorY < 250.0f) {
            // Jugador arriba → Ladrón abajo
            nuevaXLadron = 400.0f;
            nuevaYLadron = 400.0f;
        } else if (jugadorY < 450.0f) {
            // Jugador en medio → Ladrón arriba
            nuevaXLadron = 700.0f;
            nuevaYLadron = 100.0f;
        } else {
            // Jugador abajo → Ladrón arriba
            nuevaXLadron = 650.0f;
            nuevaYLadron = 100.0f;
        }

        ladron->setPosicion(nuevaXLadron, nuevaYLadron);
        ladron->setEnAire(false);
        ladron->setVelocidadY(0);

        qDebug() << "Jugador reposicionado en X:" << nuevaXJugador << "Y:" << jugadorY;
        qDebug() << "Ladrón reposicionado en X:" << nuevaXLadron << "Y:" << nuevaYLadron;
    }
}

void NivelPuente::aumentarDificultad() {
    int nivelDificultad = static_cast<int>(tiempoTranscurridoTotal / INTERVALO_AUMENTO_DIFICULTAD);
    static int ultimoNivel = 0;

    if (nivelDificultad > ultimoNivel) {
        ultimoNivel = nivelDificultad;
        if (ladron) {
            ladron->aumentarVelocidad(INCREMENTO_VELOCIDAD_LADRON);
        }
    }
}

void NivelPuente::verificarCondicionesFinales() {
    if (monedasRecolectadas >= monedasObjetivo) {
        completado = true;
        emit nivelCompletado(true);
        qDebug() << "Nivel completado!";
    }

    if (jugador->getVidas() <= 0 && !mostrarGameOver) {
        mostrarPantallaGameOver("SIN VIDAS");
    }
}

void NivelPuente::mostrarPantallaGameOver(const QString& razon) {
    mostrarGameOver = true;
    razonGameOver = razon;
    pausado = true;
    qDebug() << "Game Over:" << razon;
}

void NivelPuente::dibujarGameOver(QPainter& painter) {
    GameOverScreen gameOver;
    gameOver.dibujar(painter, width(), height(), razonGameOver,
                     monedasRecolectadas, monedasObjetivo, tiempoTranscurrido);
}

void NivelPuente::paintEvent(QPaintEvent *event) {
    Nivel::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    dibujarFondo(painter);
    dibujarVigas(painter);
    dibujarMonedas(painter);
    dibujarLadron(painter);
    dibujarJugador(painter);
    dibujarHUD(painter);

    if (hudRenderer) {
        hudRenderer->dibujarBotonPausa(painter, botonPausaRect);
    }

    if (mostrarGameOver) {
        dibujarGameOver(painter);
    }

    if (pausado && !mostrarGameOver) {
        painter.fillRect(rect(), QColor(0, 0, 0, 150));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 48, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "PAUSA\n\nESC para continuar");
    }
}

void NivelPuente::dibujarFondo(QPainter& painter) {
    if (fondoNivel3.isNull()) {
        painter.fillRect(rect(), QColor(135, 100, 80));
        return;
    }
    painter.drawPixmap(rect(), fondoNivel3);
}

void NivelPuente::dibujarVigas(QPainter& painter) {
    for (Viga* viga : vigas) {
        viga->dibujar(painter);
    }
}

void NivelPuente::dibujarMonedas(QPainter& painter) {
    for (Moneda* moneda : monedas) {
        if (moneda->estaActiva()) {
            moneda->dibujar(painter);
        }
    }
}

void NivelPuente::dibujarLadron(QPainter& painter) {
    if (ladron) {
        ladron->dibujar(painter);
    }
}

void NivelPuente::dibujarJugador(QPainter& painter) {
    if (!jugador) return;

    QPixmap spriteActual;
    int frameParaDibujar = 0;
    int totalFrames = 5;

    if (jugador->estaEnAire() && std::abs(jugador->getVelocidadY()) > 50.0f) {
        // En el aire CON velocidad significativa: usar sprites de salto
        spriteActual = jugador->getMirandoDerecha() ?
                           spriteJugadorSaltoDer : spriteJugadorSaltoIzq;
        totalFrames = 6;

        float velY = jugador->getVelocidadY();
        if (velY < -200) {
            frameParaDibujar = 1;  // Subiendo rápido
        } else if (velY < 0) {
            frameParaDibujar = 2;  // Subiendo lento
        } else if (velY < 200) {
            frameParaDibujar = 3;  // Pico del salto
        } else {
            frameParaDibujar = 4;  // Cayendo
        }
    } else if (jugador->estaMoviendoseLateral()) {
        // Moviéndose lateralmente: sprites de correr
        spriteActual = jugador->getMirandoDerecha() ?
                           spriteJugadorCorrDer : spriteJugadorCorrIzq;
        totalFrames = 5;
        frameParaDibujar = jugador->getFrameCorrer();
    } else {
        // Quieto: sprites de correr frame 0
        spriteActual = jugador->getMirandoDerecha() ?
                           spriteJugadorCorrDer : spriteJugadorCorrIzq;
        totalFrames = 5;
        frameParaDibujar = 0;
    }

    if (spriteActual.isNull()) {
        qDebug() << "ERROR: Sprite del jugador es NULL";
        return;
    }

    int anchoFrame = spriteActual.width() / totalFrames;
    QRect sourceRect(frameParaDibujar * anchoFrame, 0, anchoFrame, spriteActual.height());
    QRectF destRect(jugador->getPosX(), jugador->getPosY(), 80, 100);

    painter.drawPixmap(destRect.toRect(), spriteActual, sourceRect);
}

void NivelPuente::dibujarHUD(QPainter& painter) {
    if (!hudRenderer) return;

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(20, 30, "Nivel 3: El Puente");

    int vidasJugador = jugador->getVidas();
    hudRenderer->dibujarCorazones(painter, vidasJugador, 20, 50);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16));

    int yPos = 100;

    QString textoMonedas = QString("Monedas: %1/%2").arg(monedasRecolectadas).arg(monedasObjetivo);
    painter.drawText(20, yPos, textoMonedas);
    yPos += 25;

    int minutos = static_cast<int>(tiempoTranscurrido) / 60;
    int segundos = static_cast<int>(tiempoTranscurrido) % 60;
    QString textoTiempo = QString("Tiempo: %1:%2")
                              .arg(minutos, 2, 10, QChar('0'))
                              .arg(segundos, 2, 10, QChar('0'));
    painter.drawText(20, yPos, textoTiempo);
}

void NivelPuente::keyPressEvent(QKeyEvent *event) {
    if (mostrarGameOver) {
        Nivel::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        if (pausado) {
            reanudar();
        } else {
            pausar();
        }
        update();
        return;
    }

    if (pausado || completado) return;

    switch (event->key()) {
    case Qt::Key_A:
        teclaA = true;
        break;
    case Qt::Key_D:
        teclaD = true;
        break;
    case Qt::Key_W:
        teclaW = true;
        break;
    }

    Nivel::keyPressEvent(event);
}

void NivelPuente::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_A:
        teclaA = false;
        break;
    case Qt::Key_D:
        teclaD = false;
        break;
    case Qt::Key_W:
        teclaW = false;
        break;
    }

    Nivel::keyReleaseEvent(event);
}

void NivelPuente::mousePressEvent(QMouseEvent *event) {
    if (mostrarGameOver) {
        GameOverScreen gameOver;

        QPainter tempPainter(this);
        gameOver.dibujar(tempPainter, width(), height(), razonGameOver,
                         monedasRecolectadas, monedasObjetivo, tiempoTranscurrido);

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

    if (botonPausaRect.contains(event->pos())) {
        if (pausado) {
            reanudar();
        } else {
            pausar();
        }
        return;
    }

    Nivel::mousePressEvent(event);
}
