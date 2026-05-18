#ifndef SIMPLE_GAME_DIALOG_H
#define SIMPLE_GAME_DIALOG_H

#include <QDialog>
#include <QRectF>
#include <QVector>
#include <QWidget>

class QTimer;

class SimpleBreakoutWidget : public QWidget {
    Q_OBJECT

public:
    explicit SimpleBreakoutWidget(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void tick();

private:
    void resetGame();
    void resetBall();
    void buildBricks();
    void movePaddle(qreal amount);
    void updateBall();
    void drawCenteredText(QPainter& painter, const QString& text, qreal y);

    QTimer* timer_;
    QVector<QRectF> bricks_;
    QRectF paddle_;
    QPointF ballPosition_;
    QPointF ballVelocity_;
    int score_;
    int lives_;
    bool running_;
    bool gameOver_;
    bool victory_;
};

class SimpleGameDialog : public QDialog {
    Q_OBJECT

public:
    explicit SimpleGameDialog(QWidget* parent = nullptr);
};

#endif
