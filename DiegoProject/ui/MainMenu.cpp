// ui/MainMenu.cpp

#include "MainMenu.h"
#include "../core/GameController.h"
#include <QPainter>
#include <QMessageBox>
#include <QFont>
#include <QMouseEvent>
#include <QtMath>
#include <QDebug>
#include <cstdlib>
#include <ctime>

MainMenu::MainMenu(QWidget *parent)
    : QWidget(parent),
    reproductorMusica(nullptr),
    salidaAudio(nullptr),
    botonMute(nullptr),
    opcionSeleccionada(-1),
    nivelesExpandido(false),
    musicaMuteada(false),
    controladorJuego(nullptr) {

    srand(time(nullptr));

    setupUI();

    // Cargar imagen de fondo
    fondoImagen = QPixmap(":/recursos/images/menu.png");

    if (fondoImagen.isNull()) {
        qWarning() << "No se pudo cargar la imagen de fondo";
    } else {
        qDebug() << "Imagen de fondo cargada correctamente";
    }

    // Inicializar música
    inicializarMusica();

    // Inicializar controlador
    controladorJuego = new GameController(this);
    connect(controladorJuego, &GameController::nivelTerminado,
            this, [this]() {
                show();
                if (reproductorMusica) reproductorMusica->play();  // ← NUEVO
            });

    // Inicializar animaciones
    inicializarHumo();

    // Timer para animaciones (60 FPS)
    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &MainMenu::actualizarAnimaciones);
    timerAnimacion->start(16);
}

MainMenu::~MainMenu() {
}

void MainMenu::setupUI() {
    setWindowTitle("Revolución Industrial");
    setFixedSize(1280, 720);
    setMouseTracking(true);

    // Opciones del menú principal
    opcionesMenu << "Iniciar Juego"
                 << "Niveles"
                 << "Créditos"
                 << "Salir";

    // Opciones de niveles (para el submenú)
    opcionesNiveles << "Nivel 1: Fábrica Textil"
                    << "Nivel 2: Red Ferroviaria"
                    << "Nivel 3: Construcción";

    // Crear botón de mute (esquina superior izquierda)
    botonMute = new QPushButton(this);
    botonMute->setGeometry(20, 20, 50, 50);
    botonMute->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(0, 0, 0, 150);"
        "   border: 2px solid rgba(255, 215, 0, 200);"
        "   border-radius: 25px;"
        "   color: white;"
        "   font-size: 24px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 215, 0, 100);"
        "   border: 2px solid rgba(255, 215, 0, 255);"
        "}"
        );
    botonMute->setText("🔊");
    botonMute->setCursor(Qt::PointingHandCursor);

    connect(botonMute, &QPushButton::clicked, this, &MainMenu::toggleMute);
}

void MainMenu::inicializarHumo() {
    // Crear solo 20 partículas (menos denso)
    for (int i = 0; i < 20; i++) {
        HumoParticula p;

        // Posición inicial (esquina inferior izquierda, más esparcida)
        p.posicion = QPointF(-80 + (rand() % 150), height() + (rand() % 150));

        // Velocidad más variable (más esparcido, menos diagonal)
        float velX = 0.3f + (rand() % 120) / 100.0f;  // X: 0.3 a 1.5 (más variación)
        float velY = -0.6f - (rand() % 120) / 100.0f; // Y: -0.6 a -1.8 (más variación)

        p.velocidad = QPointF(velX, velY);

        p.opacidad = 120 + (rand() % 60);  // Menos opaco
        p.escala = 1.0f + (rand() % 80) / 100.0f;
        p.velocidadRotacion = -0.3f + (rand() % 60) / 100.0f;

        crearFormaHumo(p);
        humo.append(p);
    }
}

void MainMenu::crearFormaHumo(HumoParticula& p) {
    // Crear forma completamente suave (sin esquinas)
    p.forma = QPainterPath();

    QPointF centro(0, 0);
    float radioBase = 50 + (rand() % 50);

    // Crear forma orgánica con múltiples curvas Bézier suaves
    int numPuntos = 8;  // Más puntos = más suave

    // Punto inicial
    float angulo0 = 0;
    float radio0 = radioBase * (0.8f + (rand() % 40) / 100.0f);
    QPointF p0(qCos(qDegreesToRadians(angulo0)) * radio0,
               qSin(qDegreesToRadians(angulo0)) * radio0);

    p.forma.moveTo(p0);

    // Crear curvas suaves conectadas
    for (int i = 0; i < numPuntos; i++) {
        float anguloActual = i * (360.0f / numPuntos);
        float anguloSiguiente = (i + 1) * (360.0f / numPuntos);

        // Variación suave del radio
        float r1 = radioBase * (0.7f + (rand() % 50) / 100.0f);
        float r2 = radioBase * (0.7f + (rand() % 50) / 100.0f);

        // Puntos en la circunferencia
        QPointF pActual(qCos(qDegreesToRadians(anguloActual)) * r1,
                        qSin(qDegreesToRadians(anguloActual)) * r1);

        QPointF pSiguiente(qCos(qDegreesToRadians(anguloSiguiente)) * r2,
                           qSin(qDegreesToRadians(anguloSiguiente)) * r2);

        // Dos puntos de control para curva cúbica (más suave)
        float anguloCtrl1 = anguloActual + (anguloSiguiente - anguloActual) * 0.33f;
        float anguloCtrl2 = anguloActual + (anguloSiguiente - anguloActual) * 0.66f;

        float rCtrl1 = radioBase * (0.85f + (rand() % 30) / 100.0f);
        float rCtrl2 = radioBase * (0.85f + (rand() % 30) / 100.0f);

        QPointF ctrl1(qCos(qDegreesToRadians(anguloCtrl1)) * rCtrl1,
                      qSin(qDegreesToRadians(anguloCtrl1)) * rCtrl1);

        QPointF ctrl2(qCos(qDegreesToRadians(anguloCtrl2)) * rCtrl2,
                      qSin(qDegreesToRadians(anguloCtrl2)) * rCtrl2);

        // Curva cúbica de Bézier (completamente suave)
        p.forma.cubicTo(ctrl1, ctrl2, pSiguiente);
    }

    p.forma.closeSubpath();
}

void MainMenu::actualizarHumo() {
    for (int i = 0; i < humo.size(); i++) {
        // Mover partícula
        humo[i].posicion += humo[i].velocidad;

        // Rotación más lenta
        humo[i].rotacion += humo[i].velocidadRotacion * 0.5f;

        // Expandir muy lentamente
        humo[i].escala += 0.002f;

        // Desvanecer más lento
        humo[i].opacidad -= 0.2f;

        // Reiniciar si sale de pantalla o desaparece
        if (humo[i].posicion.y() < -150 ||
            humo[i].posicion.x() > width() + 150 ||
            humo[i].opacidad <= 0) {

            // Reiniciar desde esquina inferior izquierda (más esparcido)
            humo[i].posicion = QPointF(-80 + (rand() % 150), height() + (rand() % 150));

            float velX = 0.3f + (rand() % 120) / 100.0f;
            float velY = -0.6f - (rand() % 120) / 100.0f;
            humo[i].velocidad = QPointF(velX, velY);

            humo[i].opacidad = 120 + (rand() % 60);
            humo[i].escala = 1.0f;
            humo[i].rotacion = 0;
            crearFormaHumo(humo[i]);
        }
    }
}

void MainMenu::actualizarAnimaciones() {
    actualizarHumo();
    update();
}

void MainMenu::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    dibujarFondo(painter);
    dibujarHumo(painter);
    dibujarMenu(painter);
}

void MainMenu::dibujarFondo(QPainter& painter) {
    if (!fondoImagen.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), fondoImagen);
    } else {
        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0, QColor(20, 30, 40));
        gradient.setColorAt(1, QColor(10, 15, 20));
        painter.fillRect(rect(), gradient);
    }

    // Overlay oscuro más sutil
    painter.fillRect(rect(), QColor(0, 0, 0, 80));
}

void MainMenu::dibujarHumo(QPainter& painter) {
    painter.save();

    for (const HumoParticula& p : humo) {
        painter.save();
        painter.translate(p.posicion);
        painter.rotate(p.rotacion);
        painter.scale(p.escala, p.escala);

        // Color del humo (blanco/grisáceo)
        QColor colorHumo(240, 245, 250, qMax(0.0f, p.opacidad));
        painter.setBrush(colorHumo);
        painter.setPen(Qt::NoPen);

        painter.drawPath(p.forma);

        painter.restore();
    }

    painter.restore();
}

void MainMenu::dibujarMenu(QPainter& painter) {
    painter.save();

    // OPCIONES DEL MENÚ
    QFont fuenteMenu("Georgia", 28, QFont::Bold);
    painter.setFont(fuenteMenu);

    int y = 360;
    int espaciado = 70;

    // Menú principal
    for (int i = 0; i < opcionesMenu.size(); i++) {
        QRect rectOpcion(width() / 2 - 200, y, 400, 55);

        bool isHovered = (i == opcionSeleccionada);

        // Efecto hover
        if (isHovered) {
            painter.setFont(QFont("Georgia", 30, QFont::Bold));
            painter.setPen(QColor(255, 215, 0));

            // Engranaje mejorado a la izquierda
            dibujarEngranajeHover(painter, QPointF(rectOpcion.left() - 35, rectOpcion.center().y()));
        } else {
            painter.setFont(QFont("Georgia", 28, QFont::Bold));
            painter.setPen(QColor(240, 240, 240));
        }

        // Sombra
        painter.setPen(QColor(0, 0, 0, 220));
        painter.drawText(rectOpcion.adjusted(2, 2, 2, 2), Qt::AlignCenter, opcionesMenu[i]);

        // Texto principal
        painter.setPen(isHovered ? QColor(255, 215, 0) : QColor(240, 240, 240));
        painter.drawText(rectOpcion, Qt::AlignCenter, opcionesMenu[i]);

        y += espaciado;

        // Si "Niveles" está expandido, mostrar submenú (ahora es índice 1)
        if (i == 1 && nivelesExpandido) {
            QFont fuenteSubMenu("Georgia", 22, QFont::Normal);
            painter.setFont(fuenteSubMenu);

            int subY = y;
            for (int j = 0; j < opcionesNiveles.size(); j++) {
                QRect rectSubOpcion(width() / 2 - 180, subY, 360, 50);

                bool subHovered = (opcionSeleccionada == 11 + j); // 11, 12, 13

                if (subHovered) {
                    painter.setFont(QFont("Georgia", 24, QFont::Bold));
                    painter.setPen(QColor(255, 200, 80));
                    dibujarEngranajeHover(painter, QPointF(rectSubOpcion.left() - 30, rectSubOpcion.center().y()));
                } else {
                    painter.setFont(QFont("Georgia", 22, QFont::Normal));
                    painter.setPen(QColor(200, 200, 200));
                }

                // Sombra
                painter.setPen(QColor(0, 0, 0, 200));
                painter.drawText(rectSubOpcion.adjusted(2, 2, 2, 2), Qt::AlignCenter, opcionesNiveles[j]);

                // Texto
                painter.setPen(subHovered ? QColor(255, 200, 80) : QColor(200, 200, 200));
                painter.drawText(rectSubOpcion, Qt::AlignCenter, opcionesNiveles[j]);

                subY += 55;
            }

            y = subY;
        }
    }

    // Crédito abajo centro
    painter.setFont(QFont("Georgia", 12, QFont::Light));
    painter.setPen(QColor(180, 180, 180));
    painter.drawText(QRect(0, height() - 40, width(), 30), Qt::AlignCenter,
                     "Created by Jesús & Diego");

    painter.restore();
}

void MainMenu::dibujarEngranajeHover(QPainter& painter, const QPointF& pos) {
    painter.save();
    painter.translate(pos);

    float radio = 12;
    int dientes = 8;

    // Color dorado
    QColor colorEngranaje(255, 215, 0, 200);
    painter.setPen(QPen(colorEngranaje, 2));
    painter.setBrush(QColor(255, 215, 0, 50));

    // Círculo base
    painter.drawEllipse(QPointF(0, 0), radio, radio);

    // Dientes
    float anguloEntreDientes = 360.0f / dientes;
    for (int i = 0; i < dientes; i++) {
        float angulo = i * anguloEntreDientes;
        float rad = qDegreesToRadians(angulo);

        QPointF p1(qCos(rad) * radio, qSin(rad) * radio);
        QPointF p2(qCos(rad) * (radio + 5), qSin(rad) * (radio + 5));

        painter.drawLine(p1, p2);
    }

    // Centro
    painter.setBrush(colorEngranaje);
    painter.drawEllipse(QPointF(0, 0), 3, 3);

    painter.restore();
}

void MainMenu::mouseMoveEvent(QMouseEvent *event) {
    opcionSeleccionada = obtenerOpcionEnPosicion(event->pos());
    update();
}

void MainMenu::mousePressEvent(QMouseEvent *event) {
    int opcion = obtenerOpcionEnPosicion(event->pos());

    if (opcion == 0) onNivel1Clicked();       // Iniciar Juego (= Nivel 1)
    else if (opcion == 1) onNivelesClicked(); // Niveles (toggle)
    else if (opcion == 11) onNivel1Clicked(); // Nivel 1 (submenú)
    else if (opcion == 12) onNivel2Clicked(); // Nivel 2 (submenú)
    else if (opcion == 13) onNivel3Clicked(); // Nivel 3 (submenú)
    else if (opcion == 2) onCreditosClicked();
    else if (opcion == 3) onSalirClicked();
}

int MainMenu::obtenerOpcionEnPosicion(const QPoint& pos) {
    int y = 360;
    int espaciado = 70;

    // Opciones principales
    for (int i = 0; i < opcionesMenu.size(); i++) {
        QRect rectOpcion(width() / 2 - 200, y, 400, 55);

        if (rectOpcion.contains(pos)) {
            return i;
        }

        y += espaciado;

        // Submenú de niveles (solo si está expandido y es la opción "Niveles" = 1)
        if (i == 1 && nivelesExpandido) {
            int subY = y;
            for (int j = 0; j < opcionesNiveles.size(); j++) {
                QRect rectSubOpcion(width() / 2 - 180, subY, 360, 50);

                if (rectSubOpcion.contains(pos)) {
                    return 11 + j; // 11, 12, 13
                }

                subY += 55;
            }
            y = subY;
        }
    }

    return -1;
}

// SLOTS

void MainMenu::onNivelesClicked() {
    nivelesExpandido = !nivelesExpandido;
    update();
}

void MainMenu::onNivel1Clicked() {
    try {
        if (reproductorMusica) reproductorMusica->pause();  // ← NUEVO
        hide();
        controladorJuego->iniciarNivel(1);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
                              QString("Error al iniciar Nivel 1: %1").arg(e.what()));
        if (reproductorMusica) reproductorMusica->play();  // ← NUEVO
        show();
    }
}

void MainMenu::onNivel2Clicked() {
    try {
        if (reproductorMusica) reproductorMusica->pause();
        hide();
        controladorJuego->iniciarNivel(2);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
                              QString("Error al iniciar Nivel 2: %1").arg(e.what()));
        if (reproductorMusica) reproductorMusica->play();
        show();
    }
}

void MainMenu::onNivel3Clicked() {
    try {
        if (reproductorMusica) reproductorMusica->pause();
        hide();
        controladorJuego->iniciarNivel(3);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
                              QString("Error al iniciar Nivel 3: %1").arg(e.what()));
        if (reproductorMusica) reproductorMusica->play();
        show();
    }
}

void MainMenu::onCreditosClicked() {
    QMessageBox::information(this, "Créditos",
                             "<h2>REVOLUCIÓN INDUSTRIAL</h2>"
                             "<p><b>Desarrolladores:</b></p>"
                             "<p>Jesús Alberto Córdoba Delgado</p>"
                             "<p>Diego Andrés Páez Mendoza</p>"
                             "<br>"
                             "<p><b>Universidad de Antioquia</b></p>"
                             "<p>Informática II - 2025-2</p>"
                             "<br>"
                             "<p>Proyecto Final - Momento III</p>"
                             "<p>Videojuego educativo sobre la Revolución Industrial (1760-1840)</p>");
}

void MainMenu::onSalirClicked() {
    QMessageBox::StandardButton respuesta = QMessageBox::question(
        this,
        "Salir",
        "¿Estás seguro de que deseas salir?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (respuesta == QMessageBox::Yes) {
        close();
    }
}

void MainMenu::inicializarMusica() {
    // Crear reproductor de audio
    reproductorMusica = new QMediaPlayer(this);
    salidaAudio = new QAudioOutput(this);

    reproductorMusica->setAudioOutput(salidaAudio);

    // Configurar volumen (0.0 a 1.0)
    salidaAudio->setVolume(0.3f);  // 30% del volumen máximo

    // Cargar música
    reproductorMusica->setSource(QUrl("qrc:/recursos/sounds/menu_music.mp3"));

    // Configurar loop (repetir)
    connect(reproductorMusica, &QMediaPlayer::playbackStateChanged,
            [this](QMediaPlayer::PlaybackState state) {
                if (state == QMediaPlayer::StoppedState) {
                    reproductorMusica->setPosition(0);
                    if (!musicaMuteada) {
                        reproductorMusica->play();
                    }
                }
            });

    // Reproducir
    reproductorMusica->play();

    if (reproductorMusica->error() != QMediaPlayer::NoError) {
        qWarning() << "Error al cargar música:" << reproductorMusica->errorString();
    } else {
        qDebug() << "Música de menú cargada y reproduciendo";
    }
}

void MainMenu::toggleMute() {
    musicaMuteada = !musicaMuteada;

    if (musicaMuteada) {
        salidaAudio->setVolume(0.0f);
        botonMute->setText("🔇");
        qDebug() << "🔇 Música silenciada";
    } else {
        salidaAudio->setVolume(0.3f);
        botonMute->setText("🔊");
        qDebug() << "🔊 Música activada";
    }
}
