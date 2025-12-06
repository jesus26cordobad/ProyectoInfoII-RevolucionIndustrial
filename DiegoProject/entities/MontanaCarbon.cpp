// entities/MontanaCarbon.cpp

#include "MontanaCarbon.h"
#include <QDebug>

MontanaCarbon::MontanaCarbon(float x, float y, QObject *parent)
    : QObject(parent),
    posX(x), posY(y),
    frameActual(0),
    cantidadRestante(CANTIDAD_INICIAL) {
    qDebug() << "Montana de carbon creada en posicion:" << x << "," << y;
}

MontanaCarbon::~MontanaCarbon() {
}

void MontanaCarbon::cargarSprites(const QPixmap& spriteSheet) {
    frames.clear();
    extraerFrames(spriteSheet);

    if (frames.isEmpty()) {
        qWarning() << "No se pudieron cargar los frames de la montana de carbon";
    } else {
        qDebug() << "Montana de carbon cargada con" << frames.size() << "frames";
    }
}

void MontanaCarbon::extraerFrames(const QPixmap& spriteSheet) {

    int anchoSprite = spriteSheet.width() / 10;
    int altoSprite = spriteSheet.height();

    qDebug() << "Extrayendo frames del sprite sheet (1 fila × 10 frames):";
    qDebug() << "  - Sprite sheet total:" << spriteSheet.width() << "x" << spriteSheet.height();
    qDebug() << "  - Cada frame original:" << anchoSprite << "x" << altoSprite;

    // Extraer fila única (frames 0-9)
    for (int i = 0; i < 10; i++) {
        QPixmap frame = spriteSheet.copy(i * anchoSprite, 0, anchoSprite, altoSprite);
        frames.append(frame);
        qDebug() << "  Frame" << (i+1) << "extraído desde X=" << (i * anchoSprite);
    }

    qDebug() << "Total frames extraídos:" << frames.size();
}

void MontanaCarbon::disminuir() {

    frameActual++;

    // Si llegamos al frame 10 (índice 9), volver al frame 1 (índice 0)
    if (frameActual >= TOTAL_FRAMES) {
        frameActual = 0;
        qDebug() << "Montana completó ciclo, volviendo al frame 1";
    }

    // Actualizar cantidad restante para consistencia
    // Frame 0 (visual 1) = 100%, Frame 9 (visual 10) = 10%
    cantidadRestante = CANTIDAD_INICIAL - (frameActual * (CANTIDAD_INICIAL / TOTAL_FRAMES));

    qDebug() << "Montana frame:" << (frameActual + 1) << "/" << TOTAL_FRAMES
             << "| Cantidad:" << cantidadRestante << "%";

    emit cantidadCambiada(cantidadRestante);

    // Señal cuando llega al frame 10 (vacía)
    if (frameActual == TOTAL_FRAMES - 1) {
        qDebug() << "Montana llegó al frame 10 (vacía)";
        emit carbonAgotado();
    }
}

void MontanaCarbon::reiniciar() {
    frameActual = 0;
    cantidadRestante = CANTIDAD_INICIAL;
    emit cantidadCambiada(cantidadRestante);
    qDebug() << "Montana reiniciada al frame 1";
}

void MontanaCarbon::dibujar(QPainter& painter) {
    if (frames.isEmpty() || frameActual >= frames.size()) {
        return;
    }

    painter.save();

    QPixmap frameActualPixmap = frames[frameActual];

    const int ancho = 250;  // Tamaño fijo en pantalla
    const int alto = 250;   // Tamaño fijo en pantalla

    // Escalar TODOS los frames al mismo tamaño fijo
    QPixmap frameEscalado = frameActualPixmap.scaled(ancho, alto,
                                                     Qt::IgnoreAspectRatio,  // Fuerza tamaño exacto
                                                     Qt::SmoothTransformation);


    int x = posX - ancho / 2;  // Centrado horizontalmente
    int y = posY - alto;       // Base en posY

    painter.drawPixmap(x, y, frameEscalado);

    painter.restore();
}

bool MontanaCarbon::contienePunto(float x, float y) const {
    // Usar valores explícitos
    const float ancho = 150.0f;
    const float alto = 150.0f;

    // Area clickeable rectangular alrededor de la montana
    float izq = posX - ancho / 2;
    float der = posX + ancho / 2;
    float arr = posY - alto;  // Area va hacia arriba desde la base
    float aba = posY;

    bool contiene = (x >= izq && x <= der && y >= arr && y <= aba);

    if (contiene) {
        qDebug() << "Click en montana detectado en:" << x << "," << y;
    }

    return contiene;
}
